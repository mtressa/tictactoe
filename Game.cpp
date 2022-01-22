//
// Created by Maliq Tressa on 12/28/21.
//

#include <QFuture>
#include "Game.h"
#include "gamearea.h"

#include <iomanip>

class GameArea;

Game::Game()
{
	w = new MainWindow(nullptr, this);
	QObject::connect(w, &MainWindow::onGameStarted, [&](){
		gameLoop = std::thread(&Game::start, this);
		gameLoop.detach();
	});
	gameData = w->getGameData();
	w->show();
}

Game::~Game()
{
	delete w;
	if (gameLoop.joinable())
		gameLoop.join();
}

void Game::promptGameMode()
{
	std::cout << "Enter '1' for 3x3 and input '2' for 5x5" << std::endl;
	int l_gameMode;
	std::cin >> l_gameMode;
	if (l_gameMode < 1 || l_gameMode > 2)
		throw std::exception();
	switch (l_gameMode)
	{
		case 1:
			dimension = 3;
			_field.resize(3 * 3, ' ');
			break;
		case 2:
			dimension = 5;
			_field.resize(5 * 5, ' ');
			break;
		default:
			throw std::exception();
	}
}

void Game::promptFirstMove()
{
	std::cout << "Do you want to move first?('y' for yes , 'n' for no)"
			  << std::endl;
	char response;
	std::cin >> response;
	if (response != 'y' && response != 'n')
		throw std::exception();
	if (response == 'y')
		gameData.firstMove = true;
	else
		gameData.firstMove = false;
}

void Game::promptGameType()
{
	std::cout << "Enter game mode:\n";
	std::cout << "\t'1' - Human VS AI\n";
	std::cout << "\t'2' - Human VS Human\n";
	std::cout << "\t'3' - AI VS AI" << std::endl;
	int l_gameType;
	std::cin >> l_gameType;
	if (l_gameType < 1 || l_gameType > 3)
		throw std::exception();
	switch (l_gameType)
	{
		case 1:
			gameData.gameType = HU_AI;
			promptFirstMove();
			Player1Type = (gameData.firstMove ? Human : AI);
			Player2Type = (gameData.firstMove ? AI : Human);
			break;
		case 2:
			gameData.gameType = HU_HU;
			Player1Type = Player2Type = Human;
			break;
		case 3:
			gameData.gameType = AI_AI;
			Player1Type = Player2Type = AI;
			break;
		default:
			throw std::exception();
	}
}

void Game::setGameData(GameData newGameData) {
	gameData = newGameData;
}

void Game::readInput()
{
//	promptGameMode();
//	promptGameType();
	switch (gameData.gameMode)
	{
		case MODE3X3:
			dimension = 3;
			_field.resize(3 * 3, ' ');
			break;
		case MODE5X5:
			dimension = 5;
			_field.resize(5 * 5, ' ');
			break;
		default:
			throw std::exception();
	}
	switch (gameData.gameType)
	{
		case HU_AI:
			Player1Type = (gameData.firstMove ? Human : AI);
			Player2Type = (gameData.firstMove ? AI : Human);
			break;
		case HU_HU:
			Player1Type = Player2Type = Human;
			break;
		case AI_AI:
			Player1Type = Player2Type = AI;
			break;
		default:
			throw std::exception();
	}
}

void Game::start()
{
	readInput();
	fillBestIndexMap();
	startGameLoop();
	printGameResult();
}

void Game::startGameLoop() {
	fieldMutex.lock();
	try
	{
		while (true)
		{
			std::cout<<"Enter loop"<<std::endl;
			makePlayer1Move();
			std::cout<<"player1 move"<<std::endl;
			renderField();
			if (isGameOver(_field))
				break;
			makePlayer2Move();
			renderField();
			if (isGameOver(_field))
				break;
		}
		emit onGameOver();
	}
	catch (Game::PromptCellCanceled &e){
		std::cout << e.what() << std::endl;
	}
	fieldMutex.unlock();
}

void Game::makePlayer1Move()
{
	if (Player1Type == AI)
	{
		int idx = computeBestMove(_field, 'X').first;
		_field[idx] = 'X';
		w->markCell(idx, 'X');
	}
	else
	{
		int idx = promptCell();
		_field[idx] = 'X';
		w->markCell(idx, 'X');
	}
}

void Game::makePlayer2Move()
{
	if (Player2Type == AI)
	{
		int idx = computeBestMove(_field, 'O').first;
		_field[idx] = 'O';
		w->markCell(idx, 'O');
	}
	else
	{
		int idx = promptCell();
		_field[idx] = 'O';
		w->markCell(idx, 'O');
	}
}

int Game::promptCell() const
{
	GameArea		*gameArea = w->getGameArea();
	QFuture<int>	fut = QtFuture::connect(gameArea, &GameArea::cellClicked);
	fut.waitForFinished();
	if (fut.result() == -1) throw PromptCellCanceled();
	return (fut.result());
}

void Game::printGameResult() const
{
	if (isDraw(_field))
	{
		std::cout << "Draw!" << std::endl;
		return;
	}
	std::string winner;
	switch (gameData.gameType) {
		case HU_AI:
			winner = (!(isPlayerWin(_field, 'X') != !(gameData.firstMove))) ? ("AI") : ("Human");
			break;
		case HU_HU:
			winner = "Player" + ((isPlayerWin(_field, 'X')) ? (static_cast<std::string>("1")) : (static_cast<std::string>("2")));
			break;
		case AI_AI:
			winner = "AI " + ((isPlayerWin(_field, 'X') ? (static_cast<std::string>("1")) : (static_cast<std::string>("2"))));
			break;
	}
	std::cout << winner << " is WINNER!" << std::endl;
}

std::pair<int, int>
Game::computeBestMove(const std::vector<char> &l_field, char sign, bool isCaller)
{
	int winIndex = getInstantWinIndex(l_field, getEnemySign(sign));
	if (winIndex != -1)
	{
		if (isCaller)
			return (std::make_pair(winIndex, 100));
		else
			return (std::make_pair(winIndex, 0));
	}
	winIndex = getInstantWinIndex(l_field, sign);
	if (winIndex != -1)
	{
		if (isCaller)
			return (std::make_pair(winIndex, 0));
		else
			return (std::make_pair(winIndex, 100));
	}
	static auto comp = [](const auto &lhs, const auto &rhs)
	{
		return lhs.second < rhs.second;
	};

	typedef std::vector<std::pair<int, int>> pair_vec;

	std::vector<int>	freeIndexes = getFreeIndexes(l_field);
	int                 freeIndexCount = freeIndexes.size();

	if ((freeIndexCount > dimension * dimension / 2) || (freeIndexCount > 9))
	{
		std::cout << "No minimax"<<std::endl;
		int temp = getBestIndexFromMap(l_field);
		if (temp != -1)
		{
			if (isCaller)
				return (std::make_pair(temp, 0));
			else
				return (std::make_pair(temp, 100));
		}
	}
	pair_vec			results;
	std::vector<std::vector<char>> fieldCopies(freeIndexes.size(), l_field);

	for (int i = 0; i < freeIndexes.size(); ++i) {
		std::vector<char> &copy = fieldCopies[i];
		int idx = freeIndexes[i];

		copy[idx] = sign;
	}

	for (int i = 0; i < freeIndexes.size(); ++i) {
		std::vector<char>   &copy = fieldCopies[i];
		int                 idx = freeIndexes[i];

		if (isGameOver(copy))
		{
			if (isDraw(copy))
				results.emplace_back(idx, 0);
			else if (!isPlayerWin(copy, sign) != !isCaller)
			{
//				return (std::make_pair(idx, 100));
				results.emplace_back(idx, 100);
			}
			else
			{
				results.emplace_back(idx, 0);
//				return (std::make_pair(idx, 0));
			}
		}
		else
		{
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
	auto	chance = std::accumulate(results.begin(), results.end(), 0,
									 [](const auto &a, const auto &b){ return (a + b.second); });
	chance /= results.size();
	std::pair<int, int> ret = std::make_pair(bestValueIter->first, chance);
//	if (getFreeIndexes(_field).size() == 9 - 4 )
//	{
//		for (auto x : results)
//		{
//			std::cout << x.first << ':' << x.second << ' ';
//		}
//		std::cout << '[' << l_field.data() << ']' << std::endl;
//	}
	return (ret);
}

std::vector<int> Game::getFreeIndexes(const std::vector<char>& l_field)
{
	std::vector<int> freeIdxs;
	for (int i = 0; i < l_field.size(); ++i)
	{
		if (l_field[i] == ' ')
			freeIdxs.push_back(i);
	}
	return (freeIdxs);
}

char Game::getEnemySign(char mySign)
{
	if (mySign == 'X')
		return ('O');
	else if (mySign == 'O')
		return ('X');
	else
		throw std::exception();
}

bool Game::isGameOver(const std::vector<char>& l_field) const
{
	if (getFreeIndexes(l_field).empty() || isPlayerWin(l_field, 'X') || isPlayerWin(l_field, 'O') || isDraw(l_field))
		return (true);
	return (false);
}

bool Game::isFieldEmpty() const
{
	for (char c: _field)
	{
		if (c != ' ')
			return (false);
	}
	return (true);
}

bool Game::isPlayerWin(const std::vector<char>& l_field, char sign) const
{
	int verticalCounter = 0;
	int horizontalCounter = 0;
	int diagonalCounter = 0;
	for (int i = 0; i < dimension; ++i)
	{
		for (int j = 0; j < dimension; ++j)
		{
			if (l_field[j + dimension * i] == sign)
				++horizontalCounter;
			if (l_field[i + dimension * j] == sign)
				++verticalCounter;
		}
		if (horizontalCounter == dimension)
			return (true);
		if (verticalCounter == dimension)
			return (true);
		horizontalCounter = 0;
		verticalCounter = 0;
	}
	for (int i = 0; i < dimension; ++i)
	{
		if (l_field[i + dimension * i] == sign)
			++diagonalCounter;
	}
	if (diagonalCounter == dimension)
		return (true);
	diagonalCounter = 0;
	for (int i = 0; i < dimension; ++i)
	{
		if (l_field[i + dimension * (dimension - 1 - i)] == sign)
			++diagonalCounter;
	}
	if (diagonalCounter == dimension)
		return (true);
	return (false);
}

char Game::getFieldSign(int x, int y)
{
	return (_field[x + dimension * y]);
}

bool Game::isDraw(const std::vector<char>& l_field) const
{
	if (getFreeIndexes(l_field).empty() && !isPlayerWin(l_field, 'X') && !isPlayerWin(l_field, 'O'))
		return (true);
	return (false);
}

void Game::renderField() const
{
	for (int i = 0; i < 4 * dimension + 1; ++i) std::cout << '_';
	std::cout<<'\n';

	for (int i = 0; i < dimension; ++i)
	{
		for (int j = 0; j < dimension; j++)
		{
			std::cout << "| " << _field[j + i * dimension] << ' ';
		}
		std::cout<<"|\n";
	}

	for (int i = 0; i < dimension; ++i)
	{
		std::cout<<"|---";
	}
	std::cout<<"|"<<std::endl;
}

GameData *Game::getGameData() {
	return (&gameData);
}

int Game::getDimension() {
	return (dimension);
}

void Game::fillBestIndexMap() {
	auto &map = _bestIndexMap;
	map.resize(dimension * dimension);
	for (int i = 0; i < dimension; ++i)
	{
		for (int j = 0; j < dimension; ++j)
		{
			++map[i + dimension * j];
			++map[j +  dimension * i];
		}
	}
	for (int i = 0; i < dimension; ++i)
	{
		++map[i + dimension * i];
		++map[(dimension - i - 1) + dimension * i];
	}
	for (auto i : _bestIndexMap) std::cout << i << '|';
}

int Game::getBestIndexFromMap(const std::vector<char> &l_field) {
	static auto colIsEmpty = [&](int col) -> bool{
		for (int i = 0; i < dimension; ++i)
		{
			if (l_field[col + dimension * i] == 'X' || l_field[col + dimension * i] == 'O')
				return (false);
		}
		return (true);
	};
	static auto rawIsEmpty = [&](int raw) -> bool{
		for (int i = 0; i < dimension; ++i)
		{
			if (l_field[i + dimension * raw] == 'X' || l_field[i + dimension * raw] == 'O')
				return (false);
		}
		return (true);
	};
	static auto upDiagIsEmpty = [&](int index) -> bool{
		int col = index % dimension;
		int raw = index / dimension;
		if (dimension - raw - 1 != col)	return (true);
		for (int i = 0; i < dimension; ++i)
		{
			int idx = (dimension - i - 1) + dimension * i;
			if (l_field[idx] == 'X' || l_field[idx] == 'O' && idx == index)
				return (false);
		}
		return (true);
	};
	static auto downDiagIsEmpty = [&](int index) -> bool{
		int col = index % dimension;
		int raw = index / dimension;
		if (raw != col)	return (true);
		for (int i = 0; i < dimension; ++i)
		{
			int idx = i + dimension * i;
			if (l_field[idx] == 'X' || l_field[idx] == 'O' && idx == index)
				return (false);
		}
		return (true);
	};

	std::vector<int> copy(_bestIndexMap);
	for (int i = 0; i < copy.size(); ++i){
		if (!colIsEmpty(i % dimension))
			++copy[i];
		if (!rawIsEmpty(i / dimension))
			++copy[i];
		if (!upDiagIsEmpty(i))
			++copy[i];
		if (!downDiagIsEmpty(i))
			++copy[i];
	}
	for (auto i : copy) std::cout << i << '|';
	std::vector<std::pair<int, int>> sortedMap;
	int i = 0;
	std::transform(copy.begin(), copy.end(), std::back_inserter(sortedMap), [&](const auto& data){
		return (std::move(std::make_pair(i++, data)));
	});
	std::sort(sortedMap.begin(), sortedMap.end(), [](const auto& lhs, const auto& rhs){
		return lhs.second > rhs.second;
	});
	for (auto &x : sortedMap)
	{
		if (l_field[x.first] == ' ')
			return (x.first);
	}
	return (-1);
}

int Game::getInstantWinIndex(const std::vector<char> &l_field, char sign){
	int col = -1;
	int raw = -1;
	int ascDiag = -1;
	int descDiag = -1;

	for (int i = 0; i < dimension; ++i)
	{
		int colCount = 0, rawCount = 0;
		for (int j = 0; j < dimension; ++j)
		{
			if (l_field[j + dimension * i] == ' '){
				col = j + dimension * i;
			}
			else if (l_field[j + dimension * i] == sign){
				++colCount;
			}
			if (l_field[i + dimension * j] == ' '){
				raw = i + dimension * j;
			}
			else if (l_field[i + dimension * j] == sign){
				++rawCount;
			}
		}
		if (rawCount == dimension - 1 && raw != -1)
			return (raw);
		if (colCount == dimension - 1 && col != -1)
			return (col);
		raw = col = -1;
	}
	int ascDiagCount = 0, descDiagCount = 0;
	for (int i = 0; i < dimension; ++i)
	{
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

void Game::restart() {
	fieldMutex.lock();
	for (auto &x : _field)	x = ' ';
	fieldMutex.unlock();
	gameLoop = std::thread(&Game::startGameLoop, this);
	gameLoop.detach();
}

void Game::stop() {
	fieldMutex.lock();
	_field.clear();
	_bestIndexMap.clear();
	fieldMutex.unlock();
}

const char *Game::PromptCellCanceled::what() const noexcept {
	return "PromptCell() canceled";
}
