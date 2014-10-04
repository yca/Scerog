#include "debug.h"
#include "mainwindow.h"
#include "interfaces.h"
#include "ui_mainwindow.h"
#include "multiprogressbar.h"
#include "videoselectiondialog.h"

#include <QDir>
#include <QTimer>
#include <QSettings>
#include <QFileDialog>
#include <QProgressBar>
#include <QPluginLoader>
#include <QPlainTextEdit>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	/* reload plugins */
	QDir d("/home/caglar/myfs/source-codes/bilkon/build_x86/usr/local/lib/");
	QFileInfoList list = d.entryInfoList(QStringList() << "*.so", QDir::Files | QDir::NoDotAndDotDot);
	foreach (QFileInfo info, list) {
		qDebug() << info.absoluteFilePath();
		QPluginLoader loader(info.absoluteFilePath(), this);
		QObject *plugin = loader.instance();
		if (!plugin) {
			mDebug("error loading plugin '%s:%s'", qPrintable(info.fileName()), qPrintable(loader.errorString()));
			continue;
		}
		//plugins << plugin;
		PlayerInterface *player = qobject_cast<PlayerInterface *>(plugin);
		if (player) {
			players << player;
		}
		AnalyzerInterface *a = qobject_cast<AnalyzerInterface *>(plugin);
		if (a) {
			analyzers << a;
#if 0
			if (analyzer->useCluster("cluster.bin")) {
				ui->actionCreate_dictionary->setEnabled(true);
				ui->actionTrain_Existing->setEnabled(false);
				ui->actionLoad_SVM->setEnabled(false);
				ui->actionSave_Training_Data->setEnabled(false);
				qDebug() << "error loading cluster file";
			} else {
				ui->actionTrain_Existing->setEnabled(true);
				ui->actionLoad_SVM->setEnabled(true);
				ui->actionSave_Training_Data->setEnabled(true);
			}
#endif
		}
		mDebug("loaded plugin");
	}
	mDebug("%d plugins loaded, %d players present", players.size() + analyzers.size(), players.size());
	if (players.size())
		players.first()->setParentWidget(ui->frameVideo);

	QSettings sets;
	videoList = sets.value("videoList").toStringList();
	trainList = sets.value("trainList").toStringList();
	evalList = sets.value("evalList").toStringList();
	int size = sets.beginReadArray("VideoStats");
	for (int i = 0; i < size; ++i) {
		sets.setArrayIndex(i);
		VideoStats *s = new VideoStats;
		s->frameCount = sets.value("frameCount").toInt();
		s->IFrameCount = sets.value("IFrameCount").toInt();
		s->PFrameCount = sets.value("PFrameCount").toInt();
		s->BFrameCount = sets.value("BFrameCount").toInt();
		s->totalDuration = sets.value("totalDuration").toLongLong();
		s->width = sets.value("width").toInt();
		s->height = sets.value("height").toInt();
		videoStats.insert(sets.value("fileName").toString(), s);
	}
	sets.endArray();
	videoList.removeAll("");
	trainList.removeAll("");
	evalList.removeAll("");

	connect(this, SIGNAL(playbackFinishedSignal(PlayerInterface*)), SLOT(playerPlaybackFinished(PlayerInterface*)));

	progressWidget = new MultiProgressBar();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::newVideoFrame(PlayerInterface *pl, VideoFrame *frame)
{
	if (mode == MODE_ANALYZE) {
		QUrl url = pl->getPlaybackUrl();
		QString file = url.toLocalFile();
		if (!videoStats.contains(file)) {
			videoStats.insert(file, new VideoStats);
		}
		VideoStats *stats = videoStats[file];
		stats->frameCount++;
		if (frame->frameType == 0)
			stats->IFrameCount++;
		else if (frame->frameType == 1)
			stats->PFrameCount++;
		else if (frame->frameType > 0)
			stats->BFrameCount++;
		if (frame->width > 0)
			stats->width = frame->width;
		if (frame->height > 0)
			stats->height = frame->height;
		stats->totalDuration += frame->duration;
	} else if (mode == MODE_CREATE_DICT) {
		if (frame->frameType == 0) {
			AnalyzerInterface *a = analyzers.first();
			a->analyze(frame);
		}
	}

	QTimer::singleShot(0, this, SLOT(progressTimeout()));
}

void MainWindow::playbackFinished(PlayerInterface *pl)
{
	emit playbackFinishedSignal(pl);
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
	QSettings sets;
	sets.setValue("videoList", videoList);
	sets.setValue("trainList", trainList);
	sets.setValue("evalList", evalList);

	QHashIterator<QString, VideoStats *> i(videoStats);
	sets.beginWriteArray("VideoStats");
	int cnt = 0;
	while (i.hasNext()) {
		i.next();
		sets.setArrayIndex(cnt++);
		VideoStats *s = i.value();
		sets.setValue("fileName", i.key());
		sets.setValue("frameCount", s->frameCount);
		sets.setValue("IFrameCount", s->IFrameCount);
		sets.setValue("PFrameCount", s->PFrameCount);
		sets.setValue("BFrameCount", s->BFrameCount);
		sets.setValue("totalDuration", s->totalDuration);
		sets.setValue("width", s->width);
		sets.setValue("height", s->height);
	}
	sets.endArray();
	QMainWindow::closeEvent(ev);
}

void MainWindow::progressTimeout()
{
	PlayerInterface *pl = players.first();
	progressWidget->bar2->setMaximum(pl->getTotalVideoDuration() / 1000000);
	progressWidget->bar2->setValue(pl->getPlaybackPosition() / 1000000);
	progressWidget->bar2->update();
}

void MainWindow::playerPlaybackFinished(PlayerInterface *pl)
{
	pl->stopPlayback();
	if (playlists[pl].size()) {
		pl->play(QUrl::fromLocalFile(playlists[pl].takeFirst()));
		progressWidget->bar1->setValue(progressWidget->bar1->maximum() - playlists[pl].size());
		progressWidget->bar1->update();
	} else {
		if (mode == MODE_CREATE_DICT) {
			AnalyzerInterface *a = analyzers.first();
			a->cluster("cluster.bin");
		}
		progressWidget->hide();
	}
}

void MainWindow::on_actionAdd_Videos_triggered()
{
	QStringList filenames = QFileDialog::
			getOpenFileNames(this, trUtf8("Video File Selection"), "", "Videos (*.mpg *.avi *.mkv *.mp4 *.mov)");
	videoList.append(filenames);
}

void MainWindow::on_actionAdjust_Training_triggered()
{
	VideoSelectionDialog dlg(videoList, trainList ,evalList, this);
	dlg.exec();
	trainList = dlg.training;
	evalList = dlg.eval;
}

static void frameCallback(PlayerInterface *pl, struct VideoFrame *frame, void *priv)
{
	MainWindow *w = (MainWindow *)priv;
	w->newVideoFrame(pl, frame);
}

static void finished(PlayerInterface *pl, void *priv)
{
	MainWindow *w = (MainWindow *)priv;
	w->playbackFinished(pl);
}

void MainWindow::on_actionAnalyze_Videos_triggered()
{
	progressWidget->bar1->setMaximum(videoList.size());
	progressWidget->bar1->setValue(0);
	progressWidget->show();
	videoStats.clear();
	PlayerInterface *pl = players.first();
	pl->setFlags(PlayerInterface::FLAG_ONLY_DEMUX);
	playlists[pl] = videoList;
	pl->setNewFrameCallback(frameCallback, this);
	pl->setFinishedCallback(finished, this);
	pl->play(QUrl::fromLocalFile(playlists[pl].takeFirst()));
	mode = MODE_ANALYZE;
}

void MainWindow::on_actionShow_Video_Stats_triggered()
{
	QStringList lines;
	lines << QString("%1 total videos").arg(videoList.size());
	lines << QString("%1 training videos").arg(trainList.size());
	lines << QString("%1 evaluation videos").arg(evalList.size());
	int seconds = 0;
	int frames = 0;
	int iFrames = 0;
	int pFrames = 0;
	int maxPixCnt = 0, minPixCnt = INT_MAX;
	QSize maxSize, minSize;
	foreach (QString video, trainList) {
		if (!videoStats.contains(video))
			continue;
		VideoStats *s = videoStats[video];
		seconds += s->totalDuration / 60 / 1000000;
		frames += s->frameCount;
		iFrames += s->IFrameCount;
		pFrames += s->PFrameCount + s->BFrameCount;
		int pcnt = s->width * s->height;
		if (pcnt > maxPixCnt) {
			maxSize = QSize(s->width, s->height);
			maxPixCnt = pcnt;
		}
		if (pcnt < minPixCnt) {
			minSize = QSize(s->width, s->height);
			minPixCnt = pcnt;
		}
	}
	lines << QString("%1 training frames").arg(frames);
	lines << QString("%1 training keyframes").arg(iFrames);
	lines << QString("%1 seconds training duration").arg(seconds);
	lines << QString("max training video size %1x%2").arg(maxSize.width()).arg(maxSize.height());
	lines << QString("min training video size %1x%2").arg(minSize.width()).arg(minSize.height());
	/* evaluation */
	seconds = 0;
	frames = 0;
	iFrames = 0;
	pFrames = 0;
	maxPixCnt = 0, minPixCnt = INT_MAX;
	foreach (QString video, evalList) {
		if (!videoStats.contains(video))
			continue;
		VideoStats *s = videoStats[video];
		seconds += s->totalDuration / 60 / 1000000;
		frames += s->frameCount;
		iFrames += s->IFrameCount;
		pFrames += s->PFrameCount + s->BFrameCount;
		int pcnt = s->width * s->height;
		if (pcnt > maxPixCnt) {
			maxSize = QSize(s->width, s->height);
			maxPixCnt = pcnt;
		}
		if (pcnt < minPixCnt) {
			minSize = QSize(s->width, s->height);
			minPixCnt = pcnt;
		}
	}
	lines << QString("%1 evaluation frames").arg(frames);
	lines << QString("%1 evaluation keyframes").arg(iFrames);
	lines << QString("%1 seconds evaluation duration").arg(seconds);
	lines << QString("max evaluation video size %1x%2").arg(maxSize.width()).arg(maxSize.height());
	lines << QString("min evaluation video size %1x%2").arg(minSize.width()).arg(minSize.height());

	QPlainTextEdit edit;
	edit.setPlainText(lines.join("\n"));
	edit.show();
	while (edit.isVisible())
		QApplication::processEvents();

}

void MainWindow::on_actionCreate_Dictionary_triggered()
{
	progressWidget->bar1->setMaximum(trainList.size());
	progressWidget->bar1->setValue(0);
	progressWidget->show();
	PlayerInterface *pl = players.first();
	pl->setFlags(PlayerInterface::FLAG_ONLY_KEYFRAME);
	playlists[pl] = trainList;
	pl->setNewFrameCallback(frameCallback, this);
	pl->setFinishedCallback(finished, this);
	pl->play(QUrl::fromLocalFile(playlists[pl].takeFirst()));
	mode = MODE_CREATE_DICT;
	AnalyzerInterface *a = analyzers.first();
	a->setParameter("gridX", 60);
	a->setParameter("dictSize", 1024);
}
