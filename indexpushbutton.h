//
// Created by ALIBEK on 17-Jan-22.
//

#ifndef TICTACTOE_INDEXPUSHBUTTON_H
#define TICTACTOE_INDEXPUSHBUTTON_H

#include <QPushButton>


QT_BEGIN_NAMESPACE
namespace Ui { class IndexPushButton; }
QT_END_NAMESPACE

class IndexPushButton : public QPushButton {
Q_OBJECT

public:
	explicit IndexPushButton(int newindex, QWidget *parent = nullptr);

	~IndexPushButton() override;

	int		getIndex();
	void	markCell(char sign);

protected:
	void resizeEvent(QResizeEvent *event);

private:
	Ui::IndexPushButton	*ui;
	int					index;
};


#endif //TICTACTOE_INDEXPUSHBUTTON_H
