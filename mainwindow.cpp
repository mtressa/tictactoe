#include <QVBoxLayout>
#include <QDialog>
#include <QMessageBox>
#include <thread>
#include "ui_mainwindow.h"
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent, Game *gameptr) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), game(gameptr)
{
    ui->setupUi(this);

	switcher = new QStackedWidget();
	mainMenu = new MainMenu(nullptr);
	gameArea = new GameArea(nullptr, game);
	switcher->addWidget(mainMenu);
	switcher->addWidget(gameArea);
	this->setCentralWidget(switcher);

	ui->actionMain_Menu->setEnabled(false);
	ui->actionRestart->setEnabled(false);

	QObject::connect(ui->actionMain_Menu, &QAction::triggered, this, &MainWindow::goToMainMenu);
	QObject::connect(ui->actionRestart, &QAction::triggered, [&](){
		gameArea->cellClicked(-1);
		gameArea->restart();
		game->restart();
	});
	QObject::connect(ui->actionExit, &QAction::triggered, [&](){
		QApplication::exit(0);
	});

	QObject::connect(mainMenu, &MainMenu::onStartButtonClicked, this, &MainWindow::start);

}

MainWindow::~MainWindow()
{
    delete ui;
	delete switcher;
	delete mainMenu;
	delete gameArea;
}

GameData MainWindow::getGameData() {
	return (mainMenu->getGameData());
}

void MainWindow::start() {
	game->setGameData(getGameData());
	goToGameArea();
	emit	onGameStarted();
}

GameArea *MainWindow::getGameArea() {
	return (gameArea);
}

void MainWindow::markCell(int index, char sign) {
	gameArea->markCell(index, sign);
}

void MainWindow::goToMainMenu() {
	if (switcher->currentIndex() == 1)
	{
		gameArea->cellClicked(-1);
		gameArea->stop();
		game->stop();
		switcher->setCurrentIndex(0);
		ui->actionMain_Menu->setEnabled(false);
		ui->actionRestart->setEnabled(false);
	}
}

void MainWindow::goToGameArea() {
	if (switcher->currentIndex() == 0)
	{
		gameArea->start();
		switcher->setCurrentIndex(1);
		ui->actionMain_Menu->setEnabled(true);
		ui->actionRestart->setEnabled(true);
	}
}