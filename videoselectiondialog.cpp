#include "videoselectiondialog.h"
#include "ui_videoselectiondialog.h"

VideoSelectionDialog::VideoSelectionDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::VideoSelectionDialog)
{
	ui->setupUi(this);
}

VideoSelectionDialog::~VideoSelectionDialog()
{
	delete ui;
}
