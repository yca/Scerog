#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHash>
#include <QList>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

struct VideoStats {
	VideoStats()
	{
		frameCount = 0;
		totalDuration = 0;
		IFrameCount = BFrameCount = PFrameCount = 0;
		width = 0;
		height = 0;
	}

	int frameCount;
	qint64 totalDuration;
	int IFrameCount;
	int BFrameCount;
	int PFrameCount;
	int width;
	int height;
};

struct VideoFrame;
class PlayerInterface;
class MultiProgressBar;
class AnalyzerInterface;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void newVideoFrame(PlayerInterface *pl, VideoFrame *frame);
	void playbackFinished(PlayerInterface *pl);
signals:
	void playbackFinishedSignal(PlayerInterface *pl);
protected:
	void closeEvent(QCloseEvent *ev);
private slots:
	void progressTimeout();
	void playerPlaybackFinished(PlayerInterface *pl);
	void on_actionAdd_Videos_triggered();

	void on_actionAdjust_Training_triggered();

	void on_actionAnalyze_Videos_triggered();

	void on_actionShow_Video_Stats_triggered();

	void on_actionCreate_Dictionary_triggered();

private:
	Ui::MainWindow *ui;

	enum OperationMode {
		MODE_ANALYZE,
		MODE_CREATE_DICT,
	};

	QList<PlayerInterface *> players;
	QList<AnalyzerInterface *> analyzers;
	QHash<QString, VideoStats *> videoStats;
	QHash<PlayerInterface *, QStringList> playlists;
	MultiProgressBar *progressWidget;
	OperationMode mode;

	QStringList videoList;
	QStringList trainList;
	QStringList evalList;
};

#endif // MAINWINDOW_H
