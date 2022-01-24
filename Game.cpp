#include <QFuture>
#include "Game.h"
#include "gamearea.h"

class GameArea;

Game::Game() {
	w = new MainWindow(nullptr, this);
	QObject::connect(w, &MainWindow::onGameStarted, [&]() {
		gameLoop = std::thread(&Game::start, this);
		gameLoop.detach();
	});
	gameData = w->getGameData();
	w->show();
}

Game::~Game() {
	delete w;
	if (gameLoop.joinable())
		gameLoop.join();
}

int Game::promptCell() const {
	GameArea *gameArea = w->getGameArea();
	QFuture<int> fut = QtFuture::connect(gameArea, &GameArea::cellClicked);
	fut.waitForFinished();
	if (fut.result() == -1) throw PromptCellCanceled();
	return (fut.result());
}

std::string Game::printGameResult() const {
	if (isDraw(_field)) {
		std::cout << "Draw!" << std::endl;
		return "Draw";
	}
	std::string winner;
	switch (gameData.gameType) {
		case HU_AI:
			winner = (!(isPlayerWin(_field, 'X') != !(gameData.firstMove))) ? ("AI") : ("Human");
			break;
		case HU_HU:
			winner = "Player " +
					 ((isPlayerWin(_field, 'X')) ? (static_cast<std::string>("1")) : (static_cast<std::string>("2")));
			break;
		case AI_AI:
			winner = "AI " +
					 ((isPlayerWin(_field, 'X') ? (static_cast<std::string>("1")) : (static_cast<std::string>("2"))));
			break;
	}
	winner += " is WINNER!";
	std::cout << winner << std::endl;
	return (std::move(winner));
}

std::pair<int, int>
Game::computeBestMove(const std::vector<char> &l_field, char sign, bool isCaller) {
	typedef vec<std::pair<int, int>> pair_vec;

	vec<int> freeIndexes = getFreeIndexes(l_field);
	int freeIndexCount = freeIndexes.size();
	pair_vec results;
	vec<vec<char>> fieldCopies(freeIndexes.size(), l_field);

	int winIndex = getInstantWinIndex(l_field, sign);
	if (winIndex != -1) {
		if (freeIndexCount == getFreeIndexes(_field).size()) std::cout << "Win at " << winIndex << std::endl;
		if (isCaller) {
			return (std::make_pair(winIndex, 0));
		} else {
			return (std::make_pair(winIndex, 100));
		}
	}
	if (freeIndexCount == getFreeIndexes(_field).size())
	{
		winIndex = getInstantWinIndex(l_field, getEnemySign(sign));
		if (winIndex != -1) {
			std::cout << "Block enemy at " << winIndex << std::endl;
			if (isCaller) {
				return (std::make_pair(winIndex, 100));
			} else {
				return (std::make_pair(winIndex, 0));
			}
		}
	}
	static auto comp = [](const auto &lhs, const auto &rhs) {
		return lhs.second < rhs.second;
	};

	if ((freeIndexCount > 9)) {
		std::cout << "No minimax" << std::endl;
		int temp = getBestIndexFromMap(l_field, getEnemySign(sign));
		if (temp != -1) {
			if (isCaller) {
				return (std::make_pair(temp, 0));
			} else {
				return (std::make_pair(temp, 100));
			}
		}
	}

	vec<int> newBestIndexes(getBestIndexMap(l_field, getEnemySign(sign)));
	std::sort(freeIndexes.begin(), freeIndexes.end(), [&newBestIndexes](const auto &lhs, const auto &rhs) -> bool {
		return (newBestIndexes[lhs] < newBestIndexes[rhs]);
	});

	for (int i = 0; i < freeIndexes.size(); ++i) {
		vec<char> &copy = fieldCopies[i];
		int idx = freeIndexes[i];

		copy[idx] = sign;
	}

	for (int i = 0; i < freeIndexes.size(); ++i) {
		vec<char> &copy = fieldCopies[i];
		int idx = freeIndexes[i];

		if (isGameOver(copy)) {
			if (!isPlayerWin(copy, sign) != !isCaller) {
//				return (std::make_pair(idx, 100));
				results.emplace_back(idx, 100);
			}
			else if (isDraw(copy))
				results.emplace_back(idx, 0);
			else {
				results.emplace_back(idx, 0);
//				return (std::make_pair(idx, 0));
			}
		} else {
			results.emplace_back(idx, computeBestMove(copy, getEnemySign(sign), !isCaller).second);
		}
	}
	pair_vec::iterator bestValueIter;
	if (isCaller)
		bestValueIter = std::min_element(results.begin(), results.end(),
										 comp);
	else
		bestValueIter = std::max_element(results.begin(), results.end(),
										 comp);
	auto chance = std::accumulate(results.begin(), results.end(), 0,
								  [](const auto &a, const auto &b) { return (a + b.second); });
	chance /= results.size();
	std::pair<int, int> ret = std::make_pair(bestValueIter->first, chance);
//	if (getFreeIndexes(_field).size() == freeIndexCount || getFreeIndexes(_field).size() == freeIndexCount + 1) {
//		std::cout << sign << ' ';
//		for (auto x: results) {
//			std::cout << x.first << ':' << x.second << ' ';
//		}
//		std::cout << '[' << l_field.data() << ']' << std::endl;
//	}
	return (ret);
}

vec<int> Game::getFreeIndexes(const std::vector<char> &l_field) {
	vec<int> freeIdxs;
	for (int i = 0; i < l_field.size(); ++i) {
		if (l_field[i] == ' ')
			freeIdxs.push_back(i);
	}
	return (freeIdxs);
}

char Game::getEnemySign(char mySign) {
	if (mySign == 'X')
		return ('O');
	else if (mySign == 'O')
		return ('X');
	else
		throw std::exception();
}

char Game::getFieldSign(int x, int y) {
	return (_field[x + dimension * y]);
}

GameData *Game::getGameData() {
	return (&gameData);
}

int Game::getDimension() {
	return (dimension);
}

cvec<int> Game::computeSignMap(cvec<char> &l_field, char sign) {
	vec<int>	signMap(l_field.size(), 0);
	for (int i = 0; i < l_field.size(); ++i) {
		int raw = i / dimension;
		int col = i % dimension;
		if (l_field[i] == sign) {
			for (int j = 0; j < dimension; ++j) {
				++signMap[col + dimension * j];
				++signMap[j + dimension * raw];
				if (raw == col) ++signMap[j + dimension * j];
				if (dimension - raw - 1 == col) ++signMap[dimension - j - 1 + dimension * j];
			}
		}
	}
	return (std::move(signMap));
}

cvec<int> Game::getBestIndexMap(cvec<char> &l_field, char sign) {
	vec<int> copy(_bestIndexMap);
	cvec<int>	signMap(computeSignMap(l_field, sign));
	std::transform(copy.begin(), copy.end(), signMap.begin(), copy.begin(), [](const auto &lhs, const auto &rhs) {
		return (lhs + rhs);
	});
	return (std::move(copy));
}

int Game::getBestIndexFromMap(cvec<char> &l_field, char sign) {
	const std::vector<int> copy(getBestIndexMap(l_field, 0));

	std::vector<std::pair<int, int>> sortedMap;
	int i = 0;
	std::transform(copy.begin(), copy.end(), std::back_inserter(sortedMap), [&](const auto &data) {
		return (std::move(std::make_pair(i++, data)));
	});
	std::sort(sortedMap.begin(), sortedMap.end(), [](const auto &lhs, const auto &rhs) {
		return lhs.second > rhs.second;
	});
	for (auto &x: sortedMap) {
		if (l_field[x.first] == ' ')
			return (x.first);
	}
	return (-1);
}

int Game::getInstantWinIndex(const std::vector<char> &l_field, char sign) {
	int col = -1;
	int raw = -1;
	int ascDiag = -1;
	int descDiag = -1;

	for (int i = 0; i < dimension; ++i) {
		int colCount = 0, rawCount = 0;
		for (int j = 0; j < dimension; ++j) {
			if (l_field[j + dimension * i] == ' ') {
				col = j + dimension * i;
			} else if (l_field[j + dimension * i] == sign) {
				++colCount;
			}
			if (l_field[i + dimension * j] == ' ') {
				raw = i + dimension * j;
			} else if (l_field[i + dimension * j] == sign) {
				++rawCount;
			}
		}
		if (rawCount == dimension - 1 && raw != -1) {
			if (getFreeIndexes(l_field).size() == 5 && getFreeIndexes(_field).size() == 5)
				std::cout << l_field.data() << std::endl;
			return (raw);
		}
		if (colCount == dimension - 1 && col != -1) {
			if (getFreeIndexes(l_field).size() == 5 && getFreeIndexes(_field).size() == 5)
				std::cout << l_field.data() << std::endl;
			return (col);
		}
		raw = col = -1;
	}
	int ascDiagCount = 0, descDiagCount = 0;
	for (int i = 0; i < dimension; ++i) {
		if (l_field[i + dimension * i] == ' ')
			descDiag = i + dimension * i;
		else if (l_field[i + dimension * i] == sign)
			++descDiagCount;
		if (l_field[i + dimension * (dimension - i - 1)] == ' ')
			ascDiag = i + dimension * (dimension - i - 1);
		else if (l_field[i + dimension * (dimension - i - 1)] == sign)
			++ascDiagCount;
	}
	if (descDiagCount == dimension - 1 && descDiag != -1)
		return (descDiag);
	if (ascDiagCount == dimension - 1 && ascDiag != -1)
		return (ascDiag);
	return (-1);
}


const char *Game::PromptCellCanceled::what() const noexcept {
	return "PromptCell() canceled";
}