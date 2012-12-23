#include <QApplication>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QTimer>
#include <sys/time.h>
#include "ConfigDialog.h"
#include "QtGeneratorWindow.h"
#include "Icon.h"
#include "Panels.h"

extern "C"
{
#include "ui.h"
#include "uiplot.h"
#include "vdp.h"
#include "gensound.h"
#include "cpu68k.h"
#include "mem68k.h"
#include "cpuz80.h"
#include "event.h"
#include "state.h"
#include "initcart.h"
#include "patch.h"
#include "dib.h"
#include "avi.h"
}


#define HBORDER_MAX 32
#define HBORDER_DEFAULT 8

#define VBORDER_MAX 32
#define VBORDER_DEFAULT 8

#define HMAXSIZE (320 + 2 * HBORDER_MAX)
#define VMAXSIZE (240 + 2 * VBORDER_MAX)

#define HSIZE (320 + 2 * HBORDER_DEFAULT)
#define VSIZE ((vdp_vislines ? vdp_vislines : 224) + 2 * VBORDER_DEFAULT)


static int sound_active;

Emulator::Emulator(QtGeneratorWindow *parent):
	QThread(parent),
	m_win(parent),
	m_arcade(false),
	m_running(false)
{
}

Emulator::~Emulator()
{
}

void Emulator::setArcade(bool arcade)
{
	m_arcade = arcade;
}

void Emulator::loadState(const QString &file)
{
	m_stateFile = file;
	QMetaObject::invokeMethod(this, "loadCurrentState", Qt::QueuedConnection);
}

void Emulator::saveState(const QString &file)
{
	m_stateFile = file;
	QMetaObject::invokeMethod(this, "saveCurrentState", Qt::QueuedConnection);
}

void Emulator::loadImage(const QString &file)
{
	m_image = file;
	QMetaObject::invokeMethod(this, "loadCurrentImage", Qt::QueuedConnection);
}

void Emulator::run()
{
	if (!m_running) {
		return;
	}
	QMetaObject::invokeMethod(this, "renderFrame", Qt::QueuedConnection);
	exec();
}

void Emulator::renderFrame()
{
	m_win->uiNewFrame();
	event_doframe();
	if (isRunning()) {
		QMetaObject::invokeMethod(this, "renderFrame", Qt::QueuedConnection);
	}
}

void Emulator::loadCurrentImage()
{
	char *error = gen_loadimage(m_image.toLocal8Bit().constData());
	if (error) {
		m_image = QString();
		fprintf(stderr, "%s\n", error);
		if (m_arcade) {
			gen_loadmemrom(initcart, initcart_len);
			m_running = true;
		}
		else {
			m_running = false;
			return;
		}
	}
	else {
		m_running = true;
	}
}

void Emulator::loadCurrentState()
{
	if (state_loadfile(m_stateFile.toLocal8Bit().constData()) != 0) {
		qDebug() << "An error occured whilst trying to load state from " << m_stateFile;
	}
}

void Emulator::saveCurrentState()
{
	if (state_savefile(m_stateFile.toLocal8Bit().constData()) != 0) {
		qDebug() << "An error occured whilst trying to save state to " << m_stateFile;
	}
}


QtGeneratorWindow::QtGeneratorWindow(QWidget *parent):
	QMainWindow(parent),
	m_emulator(new Emulator(this)),
	m_xv(new QtXvWidget(this)),
	m_plotfield(true),
	m_frameskip(0),
	m_cellRenderer(0)
{
	m_xv->setAdaptor(m_xv->adaptors().first());
	if (!m_xv->setPixelFormat(QVideoFrame::Format_YUYV)) {
		if (!m_xv->setPixelFormat(QVideoFrame::Format_UYVY)) {
			fprintf(stderr, "No supported xv device found");
		}
	}
	if (m_xv->pixelFormat() != QVideoFrame::Format_Invalid) {
		m_frame = QVideoFrame(HMAXSIZE * VMAXSIZE * m_xv->formatInfo().bitsPerPixel, QSize(HMAXSIZE, VMAXSIZE), HMAXSIZE * 2, m_xv->pixelFormat());
		m_frame.map(QAbstractVideoBuffer::ReadWrite);
	}
	setCentralWidget(m_xv);
	createMenu();
}

QtGeneratorWindow::~QtGeneratorWindow()
{
	m_emulator->quit();
	m_emulator->wait();
}

void QtGeneratorWindow::createMenu()
{
	QMenuBar *bar = menuBar();

	QMenu *fileMenu = new QMenu("&File", this);
	QAction *openROMAction = new QAction(Icon("rom"), "&Open ROM", this);
	QAction *loadStateAction = new QAction(Icon("document-open"), "&Load state", this);
	QAction *saveStateAction = new QAction(Icon("document-save"), "&Save state", this);
	connect(openROMAction, SIGNAL(triggered()), SLOT(openROM()));
	connect(loadStateAction, SIGNAL(triggered()), SLOT(loadState()));
	connect(saveStateAction, SIGNAL(triggered()), SLOT(saveState()));
	fileMenu->addAction(openROMAction);
	fileMenu->addAction(loadStateAction);
	fileMenu->addAction(saveStateAction);
	bar->addMenu(fileMenu);

	QMenu *emulationMenu = new QMenu("&Emulation", this);
	QAction *configureAction = new QAction(Icon("configure"), "&Configure", this);
	connect(configureAction, SIGNAL(triggered()), SLOT(configure()));
	emulationMenu->addAction(configureAction);
	bar->addMenu(emulationMenu);
}

int QtGeneratorWindow::uiInit(int argc, char *argv[])
{
	fprintf(stderr, "Generator is (c) James Ponder 1997-2003, all rights reserved. v" VERSION "\n\n");
	int ch;
	while ((ch = getopt(argc, argv, "?adcr:w:")) != -1) {
		switch (ch) {
			case 'a':
				m_emulator->setArcade(true);
				break;
			case 'd':
				gen_debugmode = 1;
				break;
			case 'w': {
					int ret = chdir(optarg);
					Q_UNUSED(ret);
				}
				break;
			case 'r':
				if (!strcasecmp(optarg, "europe")) {
					vdp_overseas = 1;
					vdp_pal = 1;
				}
				else if (!strcasecmp(optarg, "japan")) {
					vdp_overseas = 0;
					vdp_pal = 0;
				}
				else if (!strcasecmp(optarg, "usa")) {
					vdp_overseas = 1;
					vdp_pal = 0;
				}
				else {
					uiUsage();
					return 1;
				}
				break;
			case '?':
				uiUsage();
				return 0;
			default:
				uiUsage();
				return 1;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 1) {
		m_emulator->loadImage(argv[0]);
		argc--;
		argv++;
	}

	if (argc > 0) {
		uiUsage();
		return 1;
	}

	QTimer::singleShot(0, m_emulator, SLOT(start()));

	return 0;
}

void QtGeneratorWindow::uiEndField()
{
	static int counter = 0, frames = 0, waitstates, ui_actualskip = 0;
	static struct timeval tv0;
	struct timeval tv;
	long dt;
	int max_wait;

	gettimeofday(&tv, NULL);

	if (m_plotfield) {
		unsigned int width = (vdp_reg[12] & 1) ? 320 : 256;
		unsigned int offset = HBORDER_DEFAULT + ((vdp_reg[12] & 1) ? 0 : 32);
		m_xv->setBorder(VBORDER_DEFAULT, HMAXSIZE - width - offset, VMAXSIZE - vdp_vislines - VBORDER_DEFAULT, offset);
		m_xv->setMinimumSize(width, vdp_vislines);
		m_xv->present(m_frame);
	}

	if (m_frameskip == 0) {
		/* dynamic frame skipping */
		counter++;
		if (sound_feedback >= 0) {
			ui_actualskip = counter;
			counter = 0;
		}
	}
	else {
		ui_actualskip = m_frameskip;
	}

	dt = (tv.tv_sec - tv0.tv_sec) * 1000000 + tv.tv_usec - tv0.tv_usec;
	tv0 = tv;
	max_wait = 1000000 / vdp_framerate;
	if (sound_active) {
		max_wait /= 2;
	}
	waitstates = dt >= max_wait ? 1 : max_wait - dt;

	if (!sound_active || sound_feedback >= 0) {
		struct timespec ts = { 0, waitstates };

		nanosleep(&ts, NULL);
	}
}

void QtGeneratorWindow::uiNewFrame()
{
	static int vmode = -1;
	static int pal = -1;
	static int skipcount = 0;
	static char frameplots[60];   /* 60 for NTSC, 50 for PAL */
	static unsigned int frameplots_i = 0;
	static int lasttype = -1;

	if (frameplots_i > vdp_framerate) {
		frameplots_i = 0;
	}

	if (((vdp_reg[12] >> 1) & 3) && vdp_oddframe) {
	/* interlace mode, and we're about to do an odd field - we always leave
	   m_plotfield alone so we do fields in pairs, this stablises the
	   display, reduces blurring */
	}
	else {
		m_plotfield = false;
		if (m_frameskip == 0) {
			if (sound_feedback != -1) {
				m_plotfield = true;
			}
		}
		else {
			if (cpu68k_frames % m_frameskip == 0) {
				m_plotfield = true;
			}
		}
	}

	if (!m_plotfield) {
		skipcount++;
		frameplots[frameplots_i++] = 0;
		return;
	}

	lasttype = vdp_oddframe;
	/* check for ROM or user changing the vertical size */
	if (vmode == (int)(vdp_reg[1] & (1 << 3)) || pal != (int)vdp_pal) {
		vdp_setupvideo();
		vmode = vdp_reg[1] & (1 << 3);
		pal = vdp_pal;
	}

	/* count the frames we've plotted in the last vdp_framerate real frames */
	int fps = 0;
	for (int i = 0; i < vdp_framerate; i++) {
		if (frameplots[i]) {
			fps++;
		}
	}
	frameplots[frameplots_i++] = 1;
}

void QtGeneratorWindow::uiMusiclog(uint8 *data, unsigned int length)
{
	Q_UNUSED(data);
	Q_UNUSED(length);
}

void QtGeneratorWindow::uiLine(int line)
{
	if (!m_plotfield) {
		return;
	}

	if (m_cellRenderer) {
		if (line == (int)(vdp_vislines >> 1)) {
			uiFrame();
		}
	}
	else {
		uiSingleLine(line);
	}
}

inline void QtGeneratorWindow::uiSingleLine(int line)
{
	static uint8 gfx[320];
	if (line < -(int)VBORDER_DEFAULT || line >= (int)(vdp_vislines + VBORDER_DEFAULT)) {
		return;
	}
	unsigned int width = (vdp_reg[12] & 1) ? 320 : 256;
	unsigned int offset = HBORDER_DEFAULT + ((vdp_reg[12] & 1) ? 0 : 32);

	uint8 *location = m_frame.bits() + (line + VBORDER_DEFAULT) * HMAXSIZE * 2;
	uint8 bg = vdp_reg[7] & 63;
	uiplot_checkpalcache(0);

	/* normal line */
	switch ((vdp_reg[12] >> 1) & 3) {
		case 0:                      /* normal */
		case 1:                      /* interlace simply doubled up */
		case 2:                      /* invalid */
			vdp_renderline(line, gfx, 0);
			break;
		case 3:                      /* interlace with double resolution */
			vdp_renderline(line, gfx, vdp_oddframe);
			break;
	}

	uiplot_convertdata_yvyu(gfx, reinterpret_cast<uint16 *>(location) + offset, width);
}

inline void QtGeneratorWindow::uiFrame()
{
	static uint8 gfx[(320 + 16) * (240 + 16)];
	unsigned int width = (vdp_reg[12] & 1) ? 320 : 256;
	unsigned int offset = HBORDER_DEFAULT + ((vdp_reg[12] & 1) ? 0 : 32);

	uiplot_checkpalcache(0);

	vdp_renderframe(gfx + (8 * (320 + 16)) + 8, 320 + 16);
	uint8 *location = m_frame.bits() + VBORDER_DEFAULT * HMAXSIZE * 2;
	for (int line = 0; line < vdp_vislines; ++line) {
		uiplot_convertdata_yvyu(gfx + 8 + (line + 8) * (320 + 16), reinterpret_cast<uint16 *>(location) + offset, width);
		location += HMAXSIZE * 2;
	}
}

void QtGeneratorWindow::uiUsage()
{
	fprintf(stderr, "generator [options] <rom>\n\n");
	fprintf(stderr, "  -a                  arcade mode: start in fullscreen\n");
	fprintf(stderr, "  -d                  debug mode\n");
	fprintf(stderr, "  -r <region>         set region to europe, japan or usa\n");
	fprintf(stderr, "  -w <work dir>       set work directory\n");
	fprintf(stderr, "  ROM types supported: .rom or .smd interleaved (autodetected)\n");
}

void QtGeneratorWindow::openROM()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open ROM"));
	if (fileName.isEmpty()) {
		return;
	}
	m_emulator->loadImage(fileName);
	QTimer::singleShot(0, m_emulator, SLOT(start()));
}

void QtGeneratorWindow::loadState()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Load state"));
	if (fileName.isEmpty()) {
		return;
	}
	m_emulator->loadState(fileName);
	QTimer::singleShot(0, m_emulator, SLOT(start()));
}

void QtGeneratorWindow::saveState()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save state"));
	if (fileName.isEmpty()) {
		return;
	}
	m_emulator->saveState(fileName);
}

void QtGeneratorWindow::configure()
{
	m_emulator->quit();
	m_emulator->wait();
	ConfigDialog dlg;
	PerformancePanel *performancePanel = new PerformancePanel(&dlg);
	dlg.addPanel(performancePanel);
	dlg.exec();
	loadSettings();
	m_emulator->start();
}

void QtGeneratorWindow::loadSettings()
{
	QSettings settings;
	m_frameskip = settings.value("frameskip", 0).toInt();
	m_cellRenderer = settings.value("renderer", 0).toInt();
}

