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
#include <thread>
#include "mainwindow.h"

typedef enum	e_playerType{
	Human,
	AI
}				t_playerType;

typedef enum	e_gameType{ // HU - игрок, AI - компьютер
	HU_AI,
	HU_HU,
	AI_AI
}				t_gameType;

typedef enum	e_gameMode{
	MODE3X3,
	MODE5X5
}				t_gameMode;

struct GameData
{
	t_gameMode	gameMode;
	bool		firstMove;
	t_gameType	gameType;
};

class MainWindow;

class Game: public QObject
{
public:
	void	start();
	Game();
	~Game();

	GameData	*getGameData();
	int			getDimension();

private:
	void	promptInput();
	void	promptGameMode();
	void	promptGameType();
	void	promptFirstMove();
	bool	isGameOver(const std::vector<char>&) const;
	bool	isFieldEmpty() const;
	bool	isPlayerWin(const std::vector<char>& l_field, char sign) const;
	bool	isDraw(const std::vector<char>& l_field) const;
	char	getFieldSign(int x, int y);
	int     getInstantWinIndex(const std::vector<char>& l_field, char sign);

	/**
	 *
	 * @param l_field field to later copy
	 * @param sign 'X' or 'O'
	 * @return first: index of char in field, second: chance for win
	 */
	std::pair<int, int>	computeBestMove(const std::vector<char>& l_field, char sign);
	int promptCell() const;
	static std::vector<int> getFreeIndexes(const std::vector<char>&);
	static char	getEnemySign(char mySign);
	void        fillBestIndexMap();
	int         getBestIndexFromMap(const std::vector<char>& l_field);

	void	makePlayer1Move();
	void	makePlayer2Move();

	void	renderField() const;
	void	printGameResult() const;

	std::vector<char>	_field;
	std::vector<int>	_bestIndexMap;
	int					dimension;
	enum e_playerType	Player1Type, Player2Type;
	GameData			gameData;
	int					lastIndex;
	std::thread			gameLoop;

    //Graphical
    MainWindow	*w;
};

#endif //TICTACTOE_GAME_H