#include "multiprogressbar.h"
#include "ui_multiprogressbar.h"

MultiProgressBar::MultiProgressBar(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MultiProgressBar)
{
	ui->setupUi(this);

	bar1 = ui->progressBar_1;
	bar2 = ui->progressBar_2;
}

MultiProgressBar::~MultiProgressBar()
{
	delete ui;
}
