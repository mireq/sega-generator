#ifndef QTGENERATORWINDOW_H_92ZLRNOU
#define QTGENERATORWINDOW_H_92ZLRNOU

#include <QMainWindow>
#include <QThread>

class Emulator: public QThread
{
Q_OBJECT
public:
	Emulator(QObject *parent = 0);
	~Emulator();
	void stop();

protected:
	void run();

private:
	bool stopEmul;
}; /* -----  end of class Emulator  ----- */


class QtGeneratorWindow: public QMainWindow
{
Q_OBJECT
public:
	QtGeneratorWindow(QWidget *parent = 0);
	~QtGeneratorWindow();
	void uiLine(int line);

private:
	Emulator *emulator;
	int hborder;
	int vborder;
}; /* -----  end of class QtGeneratorWindow  ----- */

#endif /* end of include guard: QTGENERATORWINDOW_H_92ZLRNOU */

