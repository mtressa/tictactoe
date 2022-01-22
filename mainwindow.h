#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "Game.h"
#include "mainmenu.h"
#include "gamearea.h"

class	Game;
struct	GameData;
class	MainMenu;
class	GameArea;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, Game *gameptr = nullptr);
    ~MainWindow();

	GameData	getGameData();
	void		start();
	GameArea	*getGameArea();

	void	markCell(int index, char sign);

	signals:
	void	onGameStarted();

private:
    Ui::MainWindow	*ui;
	Game			*game;
	QStackedWidget	*switcher;
	MainMenu		*mainMenu;
	GameArea		*gameArea;

	void	goToMainMenu();
	void	goToGameArea();
};

#endif // MAINWINDOW_H
