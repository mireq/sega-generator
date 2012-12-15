#include <QTimer>
#include "QtGeneratorWindow.h"

extern "C"
{
#include "event.h"
#include "generator.h"
#include "initcart.h"
#include "vdp.h"
}


#define HBORDER_MAX 32
#define HBORDER_DEFAULT 8

#define VBORDER_MAX 32
#define VBORDER_DEFAULT 8

#define HMAXSIZE (320 + 2 * HBORDER_MAX)
#define VMAXSIZE (240 + 2 * VBORDER_MAX)

#define HSIZE (320 + 2 * hborder)
#define VSIZE ((vdp_vislines ? vdp_vislines : 224) + 2 * vborder)


Emulator::Emulator(QObject *parent):
	QThread(parent),
	stopEmul(false)
{
}

Emulator::~Emulator()
{
}

void Emulator::stop()
{
	stopEmul = true;
}

void Emulator::run()
{
	gen_loadmemrom(initcart, initcart_len);
	while (!stopEmul) {
		event_doframe();
	}
}


QtGeneratorWindow::QtGeneratorWindow(QWidget *parent):
	QMainWindow(parent),
	emulator(new Emulator(this)),
	hborder(HBORDER_DEFAULT),
	vborder(VBORDER_DEFAULT)
{
	QTimer::singleShot(0, emulator, SLOT(start()));
}

QtGeneratorWindow::~QtGeneratorWindow()
{
	emulator->stop();
	emulator->wait();
}

void QtGeneratorWindow::uiLine(int line)
{
	unsigned int width = (vdp_reg[12] & 1) ? 320 : 256;
	unsigned int offset = hborder + ((vdp_reg[12] & 1) ? 0 : 32);
}

