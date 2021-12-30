//
// Created by Maliq Tressa on 12/28/21.
//

#include "Game.h"

Game::Game() : firstMove(true)
{
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
			width = 3;
			height = 3;
			_field.resize(3 * 3, ' ');
			break;
		case 2:
			width = 5;
			height = 5;
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
		firstMove = true;
	else
		firstMove = false;
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
			gameType = HU_AI;
			promptFirstMove();
			Player1Type = (firstMove ? Human : AI);
			Player2Type = (firstMove ? AI : Human);
			break;
		case 2:
			gameType = HU_HU;
			Player1Type = Player2Type = Human;
			break;
		case 3:
			gameType = AI_AI;
			Player1Type = Player2Type = AI;
			break;
		default:
			throw std::exception();
	}
}

void Game::promptInput()
{
	promptGameMode();
	promptGameType();
}

void Game::start()
{
	promptInput();
	while (true)
	{
		renderField();
		makePlayer1Move();
		renderField();
		if (isGameOver(_field))
			break;
		makePlayer2Move();
		if (isGameOver(_field))
			break;
	}
	printGameResult();
}

void Game::makePlayer1Move()
{
	if (Player1Type == AI)
	{
		int idx = computeBestMove(_field, 'X').first;
		_field[idx] = 'X';
	}
	else
	{
		auto cell = promptCell();
		_field[cell.second - 1 + (cell.first - 1) * width] = 'X';
	}
}

void Game::makePlayer2Move()
{
	if (Player2Type == AI)
	{
		int idx = computeBestMove(_field, 'O').first;
		_field[idx] = 'O';
	}
	else
	{
		auto cell = promptCell();
		_field[cell.second - 1 + (cell.first - 1) * width] = 'O';
	}
}

std::pair<int, int> Game::promptCell() const
{
	std::cout << "Enter [raw] and [column]: ";
	int r, c;
	std::cin >> r >> c;
	if ((r < 1 || r > height) || (c < 1 || c > width))
		throw std::exception();
	return (std::make_pair(r, c));
}

void Game::printGameResult() const
{
	if (isDraw())
		std::cout << "Draw!" << std::endl;
	else
		std::cout << "Player" << (isPlayerWin('X') ? "1" : "2") << " is WINNER!"
				  << std::endl;
}

std::pair<int, int>
Game::computeBestMove(const std::vector<char> &l_field, char sign)
{
	static auto minFunc = [](const auto &lhs, const auto &rhs)
	{
		return rhs.second < lhs.second;
	};
	static auto maxFunc = [](const auto &lhs, const auto &rhs)
	{
		return lhs.second < rhs.second;
	};

	typedef std::vector<std::pair<int, int>> pair_vec;

	std::vector<int>	freeIndexes = getFreeIndexes(l_field);
	pair_vec			results;

	for (int idx: freeIndexes)
	{
		std::vector<char> copy(l_field);
		copy[idx] = sign;
		if (isGameOver(copy))
		{
			if (isPlayerWin(sign))
				return (std::make_pair(idx, 100));
			else
				return (std::make_pair(idx, 0));
		}
		results.emplace_back(idx,
							 computeBestMove(copy, getEnemySign(sign)).second);
	}
	pair_vec::iterator bestValueIter;
	if ((sign != 'X') != (!firstMove)) // A XOR B
		bestValueIter = std::max_element(results.begin(), results.end(),
										 maxFunc);
	else
		bestValueIter = std::min_element(results.begin(), results.end(),
										 minFunc);
	std::pair<int, int> ret = std::make_pair(bestValueIter->first,
											 std::accumulate(results.begin(),
															 results.end(),
															 0,
															 [](const auto &a,
																const auto &b)
															 {
																 return (a +
																		 b.second);
															 }) /
											 results.size());
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
	if (getFreeIndexes(l_field).empty() || isPlayerWin('X') || isPlayerWin('O') || isDraw())
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

bool Game::isPlayerWin(char sign) const
{
	int verticalCounter = 0;
	int horizontalCounter = 0;
	int diagonalCounter = 0;
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (_field[j + width * i] == sign)
				++horizontalCounter;
			if (_field[i + width * j] == sign)
				++verticalCounter;
		}
		if (horizontalCounter == width)
			return (true);
		if (verticalCounter == width)
			return (true);
		horizontalCounter = 0;
		verticalCounter = 0;
	}
	for (int i = 0; i < width; ++i)
	{
		if (_field[i + width * i] == sign)
			++diagonalCounter;
	}
	if (diagonalCounter == width)
		return (true);
	diagonalCounter = 0;
	for (int i = 0; i < width; ++i)
	{
		if (_field[i + width * (width - 1 - i)] == sign)
			++diagonalCounter;
	}
	if (diagonalCounter == width)
		return (true);
	return (false);
}

char Game::getFieldSign(int x, int y)
{
	return (_field[x + width * y]);
}

bool Game::isDraw() const
{
	if (getFreeIndexes(_field).empty() && !isPlayerWin('X') && !isPlayerWin('O'))
		return (true);
	return (false);
}

void Game::renderField() const
{
	for (int i = 0; i < 4 * width + 1; ++i) std::cout<<'_';
	std::cout<<'\n';

	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < width; j++)
		{
			std::cout<<"| "<<_field[j + i * width]<<' ';
		}
		std::cout<<"|\n";
	}

	for (int i = 0; i < width; ++i)
	{
		std::cout<<"|---";
	}
	std::cout<<"|"<<std::endl;
}