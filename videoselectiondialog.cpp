#include "videoselectiondialog.h"
#include "ui_videoselectiondialog.h"

VideoSelectionDialog::VideoSelectionDialog(const QStringList videos, const QStringList training, const QStringList eval, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::VideoSelectionDialog)
{
	ui->setupUi(this);

	this->videos = videos;
	this->training = training;
	this->eval = eval;
	this->videos.removeAll("");
	this->training.removeAll("");
	this->eval.removeAll("");

	addVideos();

	ui->listWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
}

VideoSelectionDialog::~VideoSelectionDialog()
{
	delete ui;
}

void VideoSelectionDialog::addVideos()
{
	ui->listWidget->clear();
	ui->listWidget->addItems(videos);

	for (int i = 0; i < ui->listWidget->count(); i++) {
		if (training.contains(ui->listWidget->item(i)->text()))
			ui->listWidget->item(i)->setBackgroundColor(Qt::green);
		else if (eval.contains(ui->listWidget->item(i)->text()))
			ui->listWidget->item(i)->setBackgroundColor(Qt::blue);
		else
			ui->listWidget->item(i)->setBackgroundColor(Qt::transparent);
	}
	ui->label_1->setText(trUtf8("Training: %1").arg(training.size()));
	ui->label_2->setText(trUtf8("Evaluation: %1").arg(eval.size()));
}

void VideoSelectionDialog::on_pushMark_1_clicked()
{
	QList<QListWidgetItem *> items = ui->listWidget->selectedItems();
	for (int i = 0; i < items.size(); i++) {
		training.append(items[i]->text());
		if (eval.contains(items[i]->text()))
			eval.removeAll(items[i]->text());
	}
	addVideos();
}

void VideoSelectionDialog::on_pushMark_2_clicked()
{
	QList<QListWidgetItem *> items = ui->listWidget->selectedItems();
	for (int i = 0; i < items.size(); i++) {
		eval.append(items[i]->text());
		if (training.contains(items[i]->text()))
			training.removeAll(items[i]->text());
	}
	addVideos();
}
