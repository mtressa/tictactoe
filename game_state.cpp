//
// Created by ALIBEK on 23-Jan-22.
//
#include "Game.h"

bool Game::isGameOver(const std::vector<char> &l_field) const {
	if (getFreeIndexes(l_field).empty() || isPlayerWin(l_field, 'X') || isPlayerWin(l_field, 'O') || isDraw(l_field))
		return (true);
	return (false);
}

bool Game::isFieldEmpty() const {
	for (char c: _field) {
		if (c != ' ')
			return (false);
	}
	return (true);
}

bool Game::isPlayerWin(const std::vector<char> &l_field, char sign) const {
	int verticalCounter = 0;
	int horizontalCounter = 0;
	int diagonalCounter = 0;
	for (int i = 0; i < dimension; ++i) {
		for (int j = 0; j < dimension; ++j) {
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
	for (int i = 0; i < dimension; ++i) {
		if (l_field[i + dimension * i] == sign)
			++diagonalCounter;
	}
	if (diagonalCounter == dimension)
		return (true);
	diagonalCounter = 0;
	for (int i = 0; i < dimension; ++i) {
		if (l_field[i + dimension * (dimension - 1 - i)] == sign)
			++diagonalCounter;
	}
	if (diagonalCounter == dimension)
		return (true);
	return (false);
}

bool Game::isDraw(const std::vector<char> &l_field) const {
	if (checkPrematureDraw(l_field))
		return (true);
	if (getFreeIndexes(l_field).empty() && !isPlayerWin(l_field, 'X') && !isPlayerWin(l_field, 'O'))
		return (true);
	return (false);
}

bool Game::checkPrematureDraw(const std::vector<char> &l_field) const {
	for (int i = 0; i < dimension; ++i) {
		int vCountX = 0, vCountY = 0, hCountX = 0, hCountY = 0;
		for (int j = 0; j < dimension; ++j) {
			int hIndex = j + dimension * i;
			int vIndex = i + dimension * j;
			if (l_field[hIndex] == 'X') ++hCountX;
			if (l_field[hIndex] == 'O') ++hCountY;
			if (l_field[vIndex] == 'X') ++vCountX;
			if (l_field[vIndex] == 'O') ++vCountY;
		}
		if (!(vCountX && vCountY)) return (false);
		if (!(hCountX && hCountY)) return (false);
	}
	return (true);
}
