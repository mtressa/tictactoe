//
// Created by Maliq Tressa on 12/28/21.
//

#ifndef TICTACTOE_GAME_H
#define TICTACTOE_GAME_H
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <numeric>

class Game
{
public:
	void	start();
	Game();

private:
	void	promptInput();
	void	promptGameMode();
	void	promptGameType();
	void	promptFirstMove();
	bool	isGameOver(const std::vector<char>&) const;
	bool	isFieldEmpty() const;
	bool	isPlayerWin(char sign) const;
	bool	isDraw() const;
	char	getFieldSign(int x, int y);

	/**
	 *
	 * @param l_field field to later copy
	 * @param sign 'X' or 'O'
	 * @return first: index of char in field, second: chance for win
	 */
	std::pair<int, int>	computeBestMove(const std::vector<char>& l_field, char sign);
	std::pair<int, int> promptCell() const;
	static std::vector<int> getFreeIndexes(const std::vector<char>&);
	static char	getEnemySign(char mySign);

	void	makePlayer1Move();
	void	makePlayer2Move();

	void	renderField() const;
	void	printGameResult() const;

	std::vector<char>	_field;
	int		width;
	int		height;
	enum	e_playerType{
		Human,
		AI
	} Player1Type, Player2Type;
	enum	e_gameType{ // HU - игрок, AI - компьютер
		HU_AI,
		HU_HU,
		AI_AI
	}	gameType;
	bool	firstMove;
};

#endif //TICTACTOE_GAME_H