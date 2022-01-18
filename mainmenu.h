//
// Created by ALIBEK on 16-Jan-22.
//

#ifndef TICTACTOE_MAINMENU_H
#define TICTACTOE_MAINMENU_H

#include <QWidget>
#include "Game.h"

class	Game;
struct	GameData;

QT_BEGIN_NAMESPACE
namespace Ui { class MainMenu; }
QT_END_NAMESPACE

class MainMenu : public QWidget {
Q_OBJECT

public:
	explicit MainMenu(QWidget *parent = nullptr, Game * gameptr = nullptr);
	~MainMenu() override;

	GameData	getGameData();

	signals:
	void	onStartButtonClicked();

private:
    Ui::MainMenu *ui;
	Game	*game;
};


#endif //TICTACTOE_MAINMENU_H
