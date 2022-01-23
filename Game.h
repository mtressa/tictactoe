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
#include <mutex>
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
	Q_OBJECT
public:
	Game();
	~Game();
	GameData	*getGameData();

	int			getDimension();
	void		setGameData(GameData newGameData);
	void		start();
	void		restart();
	void		stop();
	void		startGameLoop();

	signals:
	void		onGameOver(const std::string &);

private:
	class	PromptCellCanceled: std::exception{
	public:
		const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
	};

	void	readInput();
	void	promptGameMode();
	void	promptGameType();
	void	promptFirstMove();
	bool	isGameOver(const std::vector<char>&) const;
	bool	isFieldEmpty() const;
	bool	isPlayerWin(const std::vector<char>& l_field, char sign) const;
	bool	isDraw(const std::vector<char>& l_field) const;
	char	getFieldSign(int x, int y);
	int		getInstantWinIndex(const std::vector<char>& l_field, char sign);
	bool	checkPrematureDraw(const std::vector<char>& l_field) const;
	/**
	 *
	 * @param l_field field to later copy
	 * @param sign 'X' or 'O'
	 * @return first: index of char in field, second: chance for win
	 */
	std::pair<int, int>	computeBestMove(const std::vector<char>& l_field, char sign, bool isCaller = true);
	int promptCell() const;
	static std::vector<int> getFreeIndexes(const std::vector<char>&);
	static char	getEnemySign(char mySign);
	void		fillBestIndexMap();
	const		std::vector<int>	getBestIndexMap(const std::vector<char> &l_field, const std::vector<int>& signMap);
	int			getBestIndexFromMap(const std::vector<char> &l_field, std::vector<int> &signMap);
	void		updateSignMaps();
	void		updateSignMaps(const std::vector<char> &l_field, std::vector<int> &xMap, std::vector<int> &oMap);

	void	makePlayer1Move();
	void	makePlayer2Move();

	void	renderField() const;
	std::string printGameResult() const;

	std::vector<char>	_field;
	std::vector<int>	_bestIndexMap;
	std::vector<int>	_xMap;
	std::vector<int>	_oMap;
	int					dimension;
	enum e_playerType	Player1Type, Player2Type;
	GameData			gameData;
	std::thread			gameLoop;
	std::mutex			fieldMutex;

	//Graphical
	MainWindow	*w;
};

#endif //TICTACTOE_GAME_H