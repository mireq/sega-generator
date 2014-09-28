#ifndef QMLGENERATORWINDOW_H_LXBQFOQY
#define QMLGENERATORWINDOW_H_LXBQFOQY

#include <QQuickView>
#include <QThread>
extern "C"
{
#include "generator.h"
}

#define HBORDER_MAX 32
#define HBORDER_DEFAULT 8

#define VBORDER_MAX 32
#define VBORDER_DEFAULT 8

#define HMAXSIZE (320 + 2 * HBORDER_MAX)
#define VMAXSIZE (240 + 2 * VBORDER_MAX)

#define HSIZE (320 + 2 * HBORDER_DEFAULT)
#define VSIZE ((vdp_vislines ? vdp_vislines : 224) + 2 * VBORDER_DEFAULT)


class QmlGeneratorWindow;


class Emulator: public QThread
{
Q_OBJECT
public:
	Emulator(QmlGeneratorWindow *parent = 0);
	~Emulator();
	void setArcade(bool arcade);
	void loadImage(const QString &file);
	void loadState(const QString &file);
	void saveState(const QString &file);
	void stop();

	bool arcade() const;

protected:
	void run();

private slots:
	void renderFrame();
	void loadCurrentImage();
	void loadCurrentState();
	void saveCurrentState();

private:
	QmlGeneratorWindow *m_win;
	bool m_arcade;
	bool m_running;
	QString m_image;
	QString m_stateFile;
}; /* -----  end of class Emulator  ----- */


class QmlGeneratorWindow: public QQuickView
{
Q_OBJECT
public:
	struct ControllerKeys {
		int aKey;
		int bKey;
		int cKey;
		int leftKey;
		int rightKey;
		int upKey;
		int downKey;
		int startKey;
	};
	QmlGeneratorWindow(QWindow *parent = 0);
	~QmlGeneratorWindow();

	void uiLine(int line);
	void uiEndField();
	void uiNewFrame();
	int uiInit(int argc, char *argv[]);
	void uiUsage();
	void uiMusiclog(uint8 *data, unsigned int length);
	const uint8 *gfx() const;

protected:
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

public slots:
	void startGame(const QString &filename);
	void stopGame();

signals:
	void frameFinished();

private:
	void setupKeyboard();
	void setKey(int key, int value);

private:
	Emulator *m_emulator;
	uint8 *m_gfx;
	bool m_plotfield;
	int m_frameskip;
	QString m_filename;
	ControllerKeys m_pad[2];

}; /* -----  end of class QmlGeneratorWindow  ----- */

#endif /* end of include guard: QMLGENERATORWINDOW_H_LXBQFOQY */
