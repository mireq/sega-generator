#include "QmlGeneratorWindow.h"
#include <QDebug>
#include <QKeySequence>
#include <QTimer>
#include <QQmlContext>
#include <QQmlEngine>
#include <QSettings>
#include <QUrl>
#include <sys/time.h>


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

static int sound_active;

Emulator::Emulator(QmlGeneratorWindow *parent):
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

bool Emulator::arcade() const
{
	return m_arcade;
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
	while (true) {
		if (!m_running) {
			return;
		}
		this->renderFrame();
	}
}

void Emulator::renderFrame()
{
	m_win->uiNewFrame();
	event_doframe();
}

void Emulator::loadCurrentImage()
{
	if (m_arcade && m_image.isEmpty()) {
		gen_loadmemrom(initcart, initcart_len);
		m_running = true;
		QTimer::singleShot(0, this, SLOT(start()));
		return;
	}

	char *error = gen_loadimage(m_image.toLocal8Bit().constData());
	if (error) {
		m_image = QString();
		fprintf(stderr, "%s\n", error);
		if (m_arcade) {
			gen_loadmemrom(initcart, initcart_len);
			m_running = true;
			QTimer::singleShot(0, this, SLOT(start()));
		}
		else {
			m_running = false;
			return;
		}
	}
	else {
		m_running = true;
		QTimer::singleShot(0, this, SLOT(start()));
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

void Emulator::stop()
{
	m_running = false;
	wait();
}



QmlGeneratorWindow::QmlGeneratorWindow(QWindow *parent):
	QQuickView(parent),
	m_emulator(new Emulator(this)),
	m_plotfield(true),
	m_frameskip(1)
{
	setColor("#000000");
	m_gfx = new uint8[(320 + 16) * (240 + 16)];
	resize(800, 480);
	setResizeMode(QQuickView::SizeRootObjectToView);
	engine()->rootContext()->setContextProperty("app", this);
	setSource(QUrl("ui/main.qml"));
	setupKeyboard();
}

QmlGeneratorWindow::~QmlGeneratorWindow()
{
	m_emulator->stop();
	delete[] m_gfx;
}

void QmlGeneratorWindow::uiLine(int line)
{
	if (!m_plotfield) {
		return;
	}

	if (line != (int)(vdp_vislines >> 1)) {
		return;
	}

	unsigned int width = (vdp_reg[12] & 1) ? 320 : 256;
	unsigned int offset = HBORDER_DEFAULT + ((vdp_reg[12] & 1) ? 0 : 32);

	vdp_renderframe(m_gfx + (8 * (320 + 16)) + 8, 320 + 16);
	emit frameFinished();
}

void QmlGeneratorWindow::uiEndField()
{
	static int counter = 0, frames = 0, waitstates, ui_actualskip = 0;
	static struct timeval tv0;
	struct timeval tv;
	long dt;
	int max_wait;

	gettimeofday(&tv, NULL);

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

void QmlGeneratorWindow::uiNewFrame()
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

int QmlGeneratorWindow::uiInit(int argc, char *argv[])
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
	else if (m_emulator->arcade()) {
		m_emulator->loadImage("");
	}

	if (argc > 0) {
		uiUsage();
		return 1;
	}

	uiplot_setshifts(3, 11, 19);

	return 0;
}

void QmlGeneratorWindow::uiUsage()
{
	fprintf(stderr, "generator [options] <rom>\n\n");
	fprintf(stderr, "  -a                  arcade mode: start in fullscreen\n");
	fprintf(stderr, "  -d                  debug mode\n");
	fprintf(stderr, "  -r <region>         set region to europe, japan or usa\n");
	fprintf(stderr, "  -w <work dir>       set work directory\n");
	fprintf(stderr, "  ROM types supported: .rom or .smd interleaved (autodetected)\n");
}

void QmlGeneratorWindow::uiMusiclog(uint8 *data, unsigned int length)
{
	Q_UNUSED(data);
	Q_UNUSED(length);
}

const uint8 *QmlGeneratorWindow::gfx() const
{
	return m_gfx;
}

void QmlGeneratorWindow::keyPressEvent(QKeyEvent *event)
{
	if (!m_emulator->isRunning()) {
		return;
	}
	setKey(event->key(), 1);
}

void QmlGeneratorWindow::keyReleaseEvent(QKeyEvent *event)
{
	if (!m_emulator->isRunning()) {
		return;
	}
	setKey(event->key(), 0);
}

void QmlGeneratorWindow::startGame(const QString &filename)
{
	m_emulator->stop();
	m_emulator->loadImage(filename);
	m_emulator->start();
}

void QmlGeneratorWindow::stopGame()
{
	m_emulator->stop();
}

void QmlGeneratorWindow::setupKeyboard()
{
	QSettings settings;

	QKeySequence i1a       = settings.value("key_i1a"      , QKeySequence("Z")).value<QKeySequence>();
	QKeySequence i1b       = settings.value("key_i1b"      , QKeySequence("X")).value<QKeySequence>();
	QKeySequence i1c       = settings.value("key_i1c"      , QKeySequence("C")).value<QKeySequence>();
	QKeySequence i1left    = settings.value("key_i1left"   , QKeySequence("Left")).value<QKeySequence>();
	QKeySequence i1right   = settings.value("key_i1right"  , QKeySequence("Right")).value<QKeySequence>();
	QKeySequence i1up      = settings.value("key_i1up"     , QKeySequence("Up")).value<QKeySequence>();
	QKeySequence i1down    = settings.value("key_i1down"   , QKeySequence("Down")).value<QKeySequence>();
	QKeySequence i1start   = settings.value("key_i1start"  , QKeySequence("Return")).value<QKeySequence>();

	QKeySequence i2a       = settings.value("key_i2a"      , QKeySequence("")).value<QKeySequence>();
	QKeySequence i2b       = settings.value("key_i2b"      , QKeySequence("")).value<QKeySequence>();
	QKeySequence i2c       = settings.value("key_i2c"      , QKeySequence("")).value<QKeySequence>();
	QKeySequence i2left    = settings.value("key_i2left"   , QKeySequence("")).value<QKeySequence>();
	QKeySequence i2right   = settings.value("key_i2right"  , QKeySequence("")).value<QKeySequence>();
	QKeySequence i2up      = settings.value("key_i2up"     , QKeySequence("")).value<QKeySequence>();
	QKeySequence i2down    = settings.value("key_i2down"   , QKeySequence("")).value<QKeySequence>();
	QKeySequence i2start   = settings.value("key_i2start"  , QKeySequence("")).value<QKeySequence>();

	QKeySequence openROM   = settings.value("key_openROM"  , QKeySequence("Ctrl+O")).value<QKeySequence>();
	QKeySequence loadState = settings.value("key_loadState", QKeySequence("F2")).value<QKeySequence>();
	QKeySequence saveState = settings.value("key_saveState", QKeySequence("F3")).value<QKeySequence>();
	QKeySequence quit      = settings.value("key_quit"     , QKeySequence("Ctrl+Q")).value<QKeySequence>();
	QKeySequence configure = settings.value("key_configure", QKeySequence("Ctrl+P")).value<QKeySequence>();

	m_pad[0].aKey     = i1a.isEmpty()     ? 0 : i1a[0];
	m_pad[0].bKey     = i1b.isEmpty()     ? 0 : i1b[0];
	m_pad[0].cKey     = i1c.isEmpty()     ? 0 : i1c[0];
	m_pad[0].leftKey  = i1left.isEmpty()  ? 0 : i1left[0];
	m_pad[0].rightKey = i1right.isEmpty() ? 0 : i1right[0];
	m_pad[0].upKey    = i1up.isEmpty()    ? 0 : i1up[0];
	m_pad[0].downKey  = i1down.isEmpty()  ? 0 : i1down[0];
	m_pad[0].startKey = i1start.isEmpty() ? 0 : i1start[0];

	m_pad[1].aKey     = i2a.isEmpty()     ? 0 : i2a[0];
	m_pad[1].bKey     = i2b.isEmpty()     ? 0 : i2b[0];
	m_pad[1].cKey     = i2c.isEmpty()     ? 0 : i2c[0];
	m_pad[1].leftKey  = i2left.isEmpty()  ? 0 : i2left[0];
	m_pad[1].rightKey = i2right.isEmpty() ? 0 : i2right[0];
	m_pad[1].upKey    = i2up.isEmpty()    ? 0 : i2up[0];
	m_pad[1].downKey  = i2down.isEmpty()  ? 0 : i2down[0];
	m_pad[1].startKey = i2start.isEmpty() ? 0 : i2start[0];
}

void QmlGeneratorWindow::setKey(int key, int value)
{
	for (int p = 0; p < 2; ++p) {
		if (m_pad[p].aKey == key)     mem68k_cont[p].a = value;
		if (m_pad[p].bKey == key)     mem68k_cont[p].b = value;
		if (m_pad[p].cKey == key)     mem68k_cont[p].c = value;
		if (m_pad[p].leftKey == key)  mem68k_cont[p].left = value;
		if (m_pad[p].rightKey == key) mem68k_cont[p].right = value;
		if (m_pad[p].upKey == key)    mem68k_cont[p].up = value;
		if (m_pad[p].downKey == key)  mem68k_cont[p].down = value;
		if (m_pad[p].startKey == key) mem68k_cont[p].start = value;
	}
}

