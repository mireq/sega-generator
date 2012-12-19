#include <QApplication>
#include <QTimer>
#include <sys/time.h>
#include "QtGeneratorWindow.h"

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

#define HSIZE (320 + 2 * hborder)
#define VSIZE ((vdp_vislines ? vdp_vislines : 224) + 2 * vborder)


static char ui_region_lock = 0;          /* lock region at startup -Trilkk */

static const char *ui_initload = NULL;  /* filename to load on init */
static int ui_arcade_mode = 0;          /* play ROM at start */
static uint8 ui_plotfield = 0;  /* flag indicating plotting this field */
static int ui_running = 0;      /* running a game */
static int ui_was_paused = 0;   /* used for frame skip/delay */
static uint8 ui_frameskip = 0;  /* 0 for dynamic */
static uint8 ui_actualskip = 0; /* the last skip we did (1..) */
static int ui_locksurface;      /* lock SDL surface? */
static unsigned int ui_hborder = HBORDER_DEFAULT; /* horizontal border */
static unsigned int ui_vborder = VBORDER_DEFAULT; /* vertical border */
static unsigned int hborder = HBORDER_DEFAULT; /* actual value */
static unsigned int vborder = VBORDER_DEFAULT; /* actual value */
static int ui_musicfile = -1;   /* fd of output file for GYM/GNM logging */
static t_avi *ui_avi = NULL;    /* Current AVI writer if applicable */
static uint8 *ui_avivideo;      /* video buffer */
static uint8 *ui_aviaudio;      /* audio buffer */
static int sound_active;

Emulator::Emulator(QtGeneratorWindow *parent):
	QThread(parent),
	stopEmul(false),
	win(parent),
	image(0)
{
}

Emulator::~Emulator()
{
}

void Emulator::stop()
{
	stopEmul = true;
}

void Emulator::loadImage(const char *image)
{
	this->image = image;
}

void Emulator::run()
{
	gen_loadmemrom(initcart, initcart_len);
	while (!stopEmul) {
		if (image) {
			char *error = gen_loadimage(image);
			if (error) {
				fprintf(stderr, "%s\n", error);
				gen_loadmemrom(initcart, initcart_len);
			}
			image = 0;
		}
		win->uiNewFrame();
		event_doframe();
	}
}


QtGeneratorWindow::QtGeneratorWindow(QWidget *parent):
	QMainWindow(parent),
	emulator(new Emulator(this)),
	xv(new QtXvWidget(this)),
	hborder(HBORDER_DEFAULT),
	vborder(VBORDER_DEFAULT)
{
	xv->setAdaptor(xv->adaptors().first());
	xv->setMinimumSize(HMAXSIZE, VMAXSIZE);
	if (!xv->setPixelFormat(QVideoFrame::Format_YUYV)) {
		if (!xv->setPixelFormat(QVideoFrame::Format_UYVY)) {
			fprintf(stderr, "No supported xv device found");
		}
	}
	if (xv->pixelFormat() != QVideoFrame::Format_Invalid) {
		frame = QVideoFrame(HMAXSIZE * VMAXSIZE * 2, QSize(HMAXSIZE, VMAXSIZE), HMAXSIZE * 2, xv->pixelFormat());
		frame.map(QAbstractVideoBuffer::ReadWrite);
	}
	setCentralWidget(xv);
}

QtGeneratorWindow::~QtGeneratorWindow()
{
	emulator->stop();
	emulator->wait();
}

int QtGeneratorWindow::uiInit(int argc, char *argv[])
{
	fprintf(stderr, "Generator is (c) James Ponder 1997-2003, all rights reserved. v" VERSION "\n\n");
	int ch;
	while ((ch = getopt(argc, argv, "?adcr:w:")) != -1) {
		switch (ch) {
			case 'a':
				ui_arcade_mode = 1;
				break;
			case 'd':
				gen_debugmode = 1;
				break;
			case 'w':
				chdir(optarg);
				break;
			case 'r':
				if (!strcasecmp(optarg, "europe")) {
					ui_region_lock = 'E';
				}
				else if (!strcasecmp(optarg, "japan")) {
					ui_region_lock = 'J';
				}
				else if (!strcasecmp(optarg, "usa")) {
					ui_region_lock = 'U';
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
		ui_initload = argv[0];
		argc--;
		argv++;
		emulator->loadImage(ui_initload);
	}

	if (argc > 0) {
		uiUsage();
		return 1;
	}

	QTimer::singleShot(0, emulator, SLOT(start()));

	return 0;
}

int frm = 0;
void QtGeneratorWindow::uiEndField()
{
	frm++;
	if (frm == 1000) {
		emulator->stop();
		QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
	}
	static int counter = 0, frames = 0, waitstates;
	static struct timeval tv0;
	struct timeval tv;
	long dt;
	int max_wait;

	gettimeofday(&tv, NULL);

	if (ui_plotfield) {
		QMetaObject::invokeMethod(this, "presentFrame", Qt::BlockingQueuedConnection);
	}

	if (ui_frameskip == 0) {
		/* dynamic frame skipping */
		counter++;
		if (sound_feedback >= 0) {
			ui_actualskip = counter;
			counter = 0;
		}
	}
	else {
		ui_actualskip = ui_frameskip;
	}

	if (ui_was_paused) {
		ui_was_paused = 0;
		frames = 0;
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
	   ui_plotfield alone so we do fields in pairs, this stablises the
	   display, reduces blurring */
	}
	else {
		ui_plotfield = 0;
		if (ui_frameskip == 0) {
			if (sound_feedback != -1) {
				ui_plotfield = 1;
			}
		}
		else {
			if (cpu68k_frames % ui_frameskip == 0) {
				ui_plotfield = 1;
			}
		}
	}

	if (!ui_plotfield) {
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
	if (ui_musicfile != -1) {
		write(ui_musicfile, data, length);
	}
}

void QtGeneratorWindow::uiLine(int line)
{
	static uint8 gfx[320];
	unsigned int width = (vdp_reg[12] & 1) ? 320 : 256;
	unsigned int offset = hborder + ((vdp_reg[12] & 1) ? 0 : 32);

	if (!ui_plotfield) {
		return;
	}

	if (line < -(int)vborder || line >= (int)(vdp_vislines + vborder)) {
		return;
	}

	uint8 *location = frame.bits() + (line + vborder) * HMAXSIZE * 2;
	uint8 bg = vdp_reg[7] & 63;
	uiplot_checkpalcache(0);

	if (line < 0 || line >= (int)vdp_vislines) {
		uint32 bg;
		uint32 *p = reinterpret_cast<uint32 *>(location);
		uint32 *q = p + HSIZE / 2;
		while (p < q) {
			*p++ = bg;
		}
		return;
	}

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

void QtGeneratorWindow::uiUsage()
{
	fprintf(stderr, "generator [options] <rom>\n\n");
	fprintf(stderr, "  -a                  arcade mode: start in fullscreen\n");
	fprintf(stderr, "  -d                  debug mode\n");
	fprintf(stderr, "  -r <region>         set region to europe, japan or usa\n");
	fprintf(stderr, "  -w <work dir>       set work directory\n");
	fprintf(stderr, "  ROM types supported: .rom or .smd interleaved (autodetected)\n");
}

void QtGeneratorWindow::presentFrame()
{
	xv->present(frame);
}

