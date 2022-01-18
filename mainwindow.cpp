#include <QVBoxLayout>
#include <thread>
#include "ui_mainwindow.h"
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent, Game *gameptr) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), game(gameptr)
{
    ui->setupUi(this);
	switcher = new QStackedWidget();
	mainMenu = new MainMenu(nullptr, game);
	gameArea = new GameArea(nullptr, game);
	switcher->addWidget(mainMenu);
	switcher->addWidget(gameArea);
	this->setCentralWidget(switcher);
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
	std::cout<<"Test\n"<<std::endl;
	gameArea->start();
	std::cout<<"gameArea.start()"<<std::endl;
	std::cout<<"switcher index "<<switcher->currentIndex()<<std::endl;
	switcher->setCurrentIndex(1);
	std::cout<<"switcher"<<std::endl;

	emit	onGameStarted();
}

GameArea *MainWindow::getGameArea() {
	return (gameArea);
}
