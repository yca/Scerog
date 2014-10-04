#include "multiprogressbar.h"
#include "ui_multiprogressbar.h"

MultiProgressBar::MultiProgressBar(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MultiProgressBar)
{
	ui->setupUi(this);
}

MultiProgressBar::~MultiProgressBar()
{
	delete ui;
}
