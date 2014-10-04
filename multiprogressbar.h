#ifndef MULTIPROGRESSBAR_H
#define MULTIPROGRESSBAR_H

#include <QWidget>

namespace Ui {
class MultiProgressBar;
}

class MultiProgressBar : public QWidget
{
	Q_OBJECT

public:
	explicit MultiProgressBar(QWidget *parent = 0);
	~MultiProgressBar();

private:
	Ui::MultiProgressBar *ui;
};

#endif // MULTIPROGRESSBAR_H
