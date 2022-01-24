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

template<typename T>
using vec = std::vector<T>;
template<typename T>
using cvec = const std::vector<T>;

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

	// Procedures:
	void	readInput();
	void	promptGameMode();
	void	promptGameType();
	void	promptFirstMove();
	void	fillBestIndexMap();
	void	makePlayer1Move();
	void	makePlayer2Move();
	void	renderField() const;

	bool	isGameOver(cvec<char>&) const;
	bool	isFieldEmpty() const;
	bool	isPlayerWin(cvec<char>& l_field, char sign) const;
	bool	isDraw(cvec<char>& l_field) const;
	bool	checkPrematureDraw(cvec<char>& l_field) const;

	char	getFieldSign(int x, int y);
	cvec<int> computeSignMap(cvec<char> &l_field, char sign);
	int		getInstantWinIndex(cvec<char>& l_field, char sign);
	/**
	 *
	 * @param l_field field to later copy
	 * @param sign 'X' or 'O'
	 * @return first: index of char in field, second: chance for win
	 */
	std::pair<int, int>	computeBestMove(cvec<char>& l_field, char sign, bool isCaller = true);
	int		promptCell() const;
	static vec<int>	getFreeIndexes(cvec<char>&);
	static char		getEnemySign(char mySign);
	cvec<int> getBestIndexMap(cvec<char> &l_field, char sign);
	int getBestIndexFromMap(cvec<char> &l_field, char sign);
	std::string		printGameResult() const;

	std::vector<char>	_field;
	std::vector<int>	_bestIndexMap;
	int					dimension;
	enum e_playerType	Player1Type, Player2Type;
	GameData			gameData;
	std::thread			gameLoop;
	std::mutex			fieldMutex;

	//Graphical
	MainWindow	*w;
};

#endif //TICTACTOE_GAME_H