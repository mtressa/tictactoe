//
// Created by ALIBEK on 16-Jan-22.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainMenu.h" resolved

#include "mainmenu.h"
#include "ui_MainMenu.h"


MainMenu::MainMenu(QWidget *parent) :
		QWidget(parent), ui(new Ui::MainMenu) {
	ui->setupUi(this);
	QObject::connect(ui->startButton, &QPushButton::clicked, [&](){
		emit onStartButtonClicked();
	});
}

MainMenu::~MainMenu() {
	delete ui;
}

GameData MainMenu::getGameData() {
	GameData	data;
	data.firstMove = ui->checkBox->isChecked();
	data.gameMode = ui->radioButton3x3->isChecked() ? MODE3X3 : MODE5X5;
	data.gameType = static_cast<t_gameType>(ui->comboBox->currentIndex());
	return (data);
}

