//
// Created by ALIBEK on 17-Jan-22.
//

// You may need to build the project (run Qt uic code generator) to get "ui_IndexPushButton.h" resolved

#include "indexpushbutton.h"
#include "ui_IndexPushButton.h"


IndexPushButton::IndexPushButton(int newindex, QWidget *parent) :
		index(newindex), QPushButton(parent), ui(new Ui::IndexPushButton) {
	ui->setupUi(this);
}

IndexPushButton::~IndexPushButton() {
	delete ui;
}

int IndexPushButton::getIndex() {
	return (index);
}

