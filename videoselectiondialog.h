#ifndef VIDEOSELECTIONDIALOG_H
#define VIDEOSELECTIONDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class VideoSelectionDialog;
}

class VideoSelectionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit VideoSelectionDialog(const QStringList videos, const QStringList training, const QStringList eval, QWidget *parent = 0);
	~VideoSelectionDialog();
	void addVideos();
	QStringList videos;
	QStringList training;
	QStringList eval;
private slots:
	void on_pushMark_1_clicked();

	void on_pushMark_2_clicked();

private:
	Ui::VideoSelectionDialog *ui;
};

#endif // VIDEOSELECTIONDIALOG_H
