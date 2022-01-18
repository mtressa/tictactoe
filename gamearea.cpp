//
// Created by ALIBEK on 16-Jan-22.
//

// You may need to build the project (run Qt uic code generator) to get "ui_GameArea.h" resolved

#include "gamearea.h"
#include "ui_GameArea.h"


GameArea::GameArea(QWidget *parent, Game *gameptr) :
		QWidget(parent), ui(new Ui::GameArea), game(gameptr) {
	ui->setupUi(this);
}

GameArea::~GameArea() {
	delete ui;
}

void GameArea::start() {
	int dim = game->getDimension();
	switch (game->getGameData()->gameMode) {
		case MODE3X3:
			dim = 3;
			break;
		case MODE5X5:
			dim = 5;
			break;
		default:
			std::cerr << "Error: received e_GameMode is not 3x3 or 5x5. Set default to 3x3.\n";
			dim = 3;
	}
	cells.resize(dim * dim, nullptr);
	for (int i = 0; i < dim * dim; ++i)
	{
		cells[i] = new IndexPushButton(i);
		ui->gridLayout->addWidget(cells[i], i / dim, i % dim);
		int temp = cells[i]->getIndex();
		QObject::connect(cells[i], &QPushButton::clicked, [=](){
			cellClicked(temp);
		});
	}
}

