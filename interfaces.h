#ifndef INTERFACES_H
#define INTERFACES_H

#include <QUrl>

struct VideoFrame {
	enum PixFmt {
		PIX_UYVY,
		PIX_YVYU,
		PIX_YUYV,
		PIX_VYUY,
		PIX_RGB444,
		PIX_RGB888,
		PIX_RGB555,
		PIX_RGB565,
		PIX_RGB32,
		PIX_GRAY,
		PIX_UNKNOWN,
	};
	const uchar *data;
	size_t size;
	int llen;			/* line length of input */
	int lpitch;			/* pitch of one line */
	int width;
	int height;
	PixFmt pixFmt;
	int streamNo;
	int frameType;
	int duration;
};

class PlayerInterface;

typedef void (*newRtpFrame)(PlayerInterface *pl, int, long long, void *priv);
typedef void (*newVideoFrame)(PlayerInterface *pl, struct VideoFrame *, void *priv);
typedef void (*playbackFinished)(PlayerInterface *pl, void *);

class PlayerInterface
{
public:
	enum RunStatus {
		STATUS_PLAYING,
		STATUS_STOPPED,
		STATUS_PAUSED
	};
	enum VideoRatio {
		RATIO_ORIGINAL,
		RATIO_SCALED,
		RATIO_SCALED_TO_HEIGHT,
		RATIO_SCALED_TO_WIDTH
	};
	enum PlaybackFlags {
		FLAG_NONE,
		FLAG_ONLY_DEMUX = 0x1,
		FLAG_ONLY_KEYFRAME = 0x02,
	};

	virtual PlayerInterface * create(QObject *parent = NULL) = 0;
	virtual ~ PlayerInterface() {}
	virtual int analyze(QUrl source) = 0;
	virtual int play(QUrl source) = 0;
	virtual QUrl getPlaybackUrl() = 0;
	virtual int pausePlayback() = 0;
	virtual int stopPlayback() = 0;
	virtual int resumePlayback() = 0;
	virtual int setRtpFrameCallback(newRtpFrame, void *priv) = 0;
	virtual int setNewFrameCallback(newVideoFrame, void *priv) = 0;
	virtual int setFinishedCallback(playbackFinished, void *priv) = 0;
	virtual RunStatus getRunningStatus() = 0;
	virtual void setParentWidget(QWidget *w) = 0;
	virtual void setVideoRatio(VideoRatio r) = 0;
	virtual void setSeiDecode(bool on) = 0;
	virtual qint64 getTotalVideoDuration() = 0;
	virtual qint64 getPlaybackPosition() = 0;
	virtual void setFlags(uint f) = 0;

	virtual int getProgressTotal() = 0;
	virtual int getProgressCurrent() = 0;
};

class AnalyzerInterface
{
public:
	virtual AnalyzerInterface * create(QObject *parent = NULL) = 0;
	virtual ~ AnalyzerInterface() {}
	virtual int analyze(VideoFrame *frame) = 0;
	virtual int cluster(QString filename) = 0;
	virtual int useCluster(QString filename) = 0;
	virtual int train() = 0;
	virtual int useSvm(QString filename) = 0;
	virtual void setTrainingClass(int tc) = 0;
	virtual int saveTrainingData() = 0;
	virtual int setParameter(QString, QVariant) = 0;
};

Q_DECLARE_INTERFACE(PlayerInterface, "com.bilkon.qt.plugins.player")
Q_DECLARE_INTERFACE(AnalyzerInterface, "com.bilkon.qt.plugins.analyzer")

#endif // INTERFACES_H
