#include "Game.h"

void Game::setGameData(GameData newGameData) {
	gameData = newGameData;
}

void Game::start() {
	readInput();
	fillBestIndexMap();
	startGameLoop();
}

void Game::stop() {
	fieldMutex.lock();
	_field.clear();
	_bestIndexMap.clear();
	fieldMutex.unlock();
}

void Game::startGameLoop() {
	fieldMutex.lock();
	try {
		while (true) {
			std::cout << "Enter loop" << std::endl;
			makePlayer1Move();
			std::cout << "player1 move" << std::endl;
			renderField();
			if (isGameOver(_field))
				break;
			makePlayer2Move();
			renderField();
			if (isGameOver(_field))
				break;
		}
		emit onGameOver(printGameResult());
	}
	catch (Game::PromptCellCanceled &e) {
		std::cout << e.what() << std::endl;
	}
	fieldMutex.unlock();
}

void Game::restart() {
	fieldMutex.lock();
	for (auto &x: _field) x = ' ';
	fieldMutex.unlock();
	gameLoop = std::thread(&Game::startGameLoop, this);
	gameLoop.detach();
}

void Game::readInput() {
//	promptGameMode();
//	promptGameType();
	switch (gameData.gameMode) {
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
	switch (gameData.gameType) {
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

void Game::promptGameMode() {
	std::cout << "Enter '1' for 3x3 and input '2' for 5x5" << std::endl;
	int l_gameMode;
	std::cin >> l_gameMode;
	if (l_gameMode < 1 || l_gameMode > 2)
		throw std::exception();
	switch (l_gameMode) {
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

void Game::promptGameType() {
	std::cout << "Enter game mode:\n";
	std::cout << "\t'1' - Human VS AI\n";
	std::cout << "\t'2' - Human VS Human\n";
	std::cout << "\t'3' - AI VS AI" << std::endl;
	int l_gameType;
	std::cin >> l_gameType;
	if (l_gameType < 1 || l_gameType > 3)
		throw std::exception();
	switch (l_gameType) {
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

void Game::promptFirstMove() {
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

void Game::fillBestIndexMap() {
	auto &map = _bestIndexMap;
	map.resize(dimension * dimension);
	for (int i = 0; i < dimension; ++i) {
		for (int j = 0; j < dimension; ++j) {
			++map[i + dimension * j];
			++map[j + dimension * i];
		}
	}
	for (int i = 0; i < dimension; ++i) {
		++map[i + dimension * i];
		++map[(dimension - i - 1) + dimension * i];
	}
}

void Game::makePlayer1Move() {
	if (Player1Type == AI) {
		int idx = computeBestMove(_field, 'X').first;
		_field[idx] = 'X';
		w->markCell(idx, 'X');
	} else {
		int idx = promptCell();
		_field[idx] = 'X';
		w->markCell(idx, 'X');
	}
}

void Game::makePlayer2Move() {
	if (Player2Type == AI) {
		int idx = computeBestMove(_field, 'O').first;
		_field[idx] = 'O';
		w->markCell(idx, 'O');
	} else {
		int idx = promptCell();
		_field[idx] = 'O';
		w->markCell(idx, 'O');
	}
}

void Game::renderField() const {
	for (int i = 0; i < 4 * dimension + 1; ++i) std::cout << '_';
	std::cout << '\n';

	for (int i = 0; i < dimension; ++i) {
		for (int j = 0; j < dimension; j++) {
			std::cout << "| " << _field[j + i * dimension] << ' ';
		}
		std::cout << "|\n";
	}

	for (int i = 0; i < dimension; ++i) {
		std::cout << "|---";
	}
	std::cout << "|" << std::endl;
}