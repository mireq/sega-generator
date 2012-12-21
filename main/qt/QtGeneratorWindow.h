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
	void loadImage(const char *image);

protected:
	void run();

private slots:
	void renderFrame();

private:
	QtGeneratorWindow *win;
	const char *image;
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
	void presentFrame();

private:
	Emulator *emulator;
	QtXvWidget *xv;
	QVideoFrame frame;
	int hborder;
	int vborder;
}; /* -----  end of class QtGeneratorWindow  ----- */

#endif /* end of include guard: QTGENERATORWINDOW_H_92ZLRNOU */

