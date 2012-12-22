#ifndef QTGENERATORWINDOW_H_92ZLRNOU
#define QTGENERATORWINDOW_H_92ZLRNOU

#include <QMainWindow>
#include <QThread>
#include <QVideoFrame>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "QtXvWidget.h"
extern "C"
{
#include "generator.h"
}

class QtGeneratorWindow;

class Emulator: public QThread
{
Q_OBJECT
public:
	Emulator(QtGeneratorWindow *parent = 0);
	~Emulator();
	void loadImage(const QString &file);
	void setArcade(bool arcade);

protected:
	void run();

private slots:
	void renderFrame();
	void loadCurrentImage();

private:
	QtGeneratorWindow *m_win;
	bool m_arcade;
	QString m_image;
}; /* -----  end of class Emulator  ----- */


class QtGeneratorWindow: public QMainWindow
{
Q_OBJECT
public:
	QtGeneratorWindow(QWidget *parent = 0);
	~QtGeneratorWindow();
	void createMenu();
	int uiInit(int argc, char *argv[]);
	void uiEndField();
	void uiNewFrame();
	void uiMusiclog(uint8 *data, unsigned int length);
	void uiLine(int line);
	void uiSingleLine(int line);
	void uiFrame();
	void uiSimplePlot();
	void uiUsage();

private slots:
	void openROM();

private:
	QVideoFrame m_frame;
	Emulator *m_emulator;
	QtXvWidget *m_xv;
	bool m_plotfield;
	int m_frameskip;
}; /* -----  end of class QtGeneratorWindow  ----- */

#endif /* end of include guard: QTGENERATORWINDOW_H_92ZLRNOU */

