#ifndef VIDEOSELECTIONDIALOG_H
#define VIDEOSELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class VideoSelectionDialog;
}

class VideoSelectionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit VideoSelectionDialog(QWidget *parent = 0);
	~VideoSelectionDialog();

private:
	Ui::VideoSelectionDialog *ui;
};

#endif // VIDEOSELECTIONDIALOG_H
