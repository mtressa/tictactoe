#ifndef TICTACTOE_GAMEAREA_H
#define TICTACTOE_GAMEAREA_H

#include <QWidget>
#include <QGridLayout>
#include "Game.h"
#include "indexpushbutton.h"

class Game;
typedef std::vector<IndexPushButton*> Buttons;

QT_BEGIN_NAMESPACE
namespace Ui { class GameArea; }
QT_END_NAMESPACE

class GameArea : public QWidget {
Q_OBJECT

public:
	explicit GameArea(QWidget *parent = nullptr, Game *gameptr = nullptr);

	~GameArea() override;

	void	start();
	void	stop();
	void	restart();
	void	markCell(int index, char sign);

	signals:
	void	cellClicked(int index);

private:
	Ui::GameArea	*ui;
	Game			*game;
	Buttons			cells;
};


#endif //TICTACTOE_GAMEAREA_H
