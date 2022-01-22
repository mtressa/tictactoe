//
// Created by ALIBEK on 17-Jan-22.
//

// You may need to build the project (run Qt uic code generator) to get "ui_IndexPushButton.h" resolved

#include <QStyle>
#include <QResizeEvent>
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

void IndexPushButton::resizeEvent(QResizeEvent *event) {
	int button_margin = ui->label->style()->pixelMetric(QStyle::PM_ButtonMargin);
	QFont f = font();
	f.setPixelSize(event->size().height() - button_margin * 2);
//	f.setPointSize(event->size().height() - button_margin * 2);
	ui->label->setFont(f);
}

void IndexPushButton::markCell(char sign) {
	ui->label->setText(static_cast<QChar>(sign));
}
