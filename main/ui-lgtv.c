#include "generator.h"

#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>


#define FB_ADDR 0x0AC0A000UL
#define FB_SIZE 0x201000UL


int fd;
static uint16_t *vfb;
const char *initload = 0;


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


#define HBORDER_MAX 32
#define HBORDER_DEFAULT 8

#define VBORDER_MAX 32
#define VBORDER_DEFAULT 8

#define HMAXSIZE (320 + 2 * HBORDER_MAX)
#define VMAXSIZE (240 + 2 * VBORDER_MAX)

#define HSIZE (320 + 2 * HBORDER_DEFAULT)
#define VSIZE ((vdp_vislines ? vdp_vislines : 224) + 2 * VBORDER_DEFAULT)


void ui_err(const char *text, ...)
{
	va_list ap;
	vfprintf(stderr, text, ap);
	putc(10, stderr);
	exit(0);
}

void ui_line(int line)
{
	static uint8 gfx[(320 + 16) * (240 + 16)];
	//static int frame = 0;

	if (line != (int)(vdp_vislines >> 1)) {
		return;
	}
	/*
	frame++;
	if (frame % 8 != 0) {
		return;
	}
	*/

	unsigned int width = (vdp_reg[12] & 1) ? 320 : 256;
	unsigned int offset = HBORDER_DEFAULT + ((vdp_reg[12] & 1) ? 0 : 32);

	uiplot_checkpalcache(0);

	vdp_renderframe(gfx + (8 * (320 + 16)) + 8, 320 + 16);
	uint16_t *location = vfb + (1368 * 264) + 524;
	uint8 *indata;
	uint16_t *outdata;
	uint32 col;
	unsigned int ui;
	int l;
	for (l = 0; l < vdp_vislines; ++l) {
		indata = gfx + 8 + (l + 8) * (320 + 16);
		outdata = location + offset;

		for (ui = 0; ui < width; ++ui) {
			col = uiplot_palcache[indata[ui]];
			outdata[ui] = 0x8000 | ((col & 0xf8) >> 3) | ((col & 0xf800) >> 6) | ((col & 0xf80000) >> 9) | 0x0001;
		}

		location += 1368;
	}
}

void ui_endfield()
{
}

void ui_final(void)
{
	munmap(vfb, FB_SIZE);
	close(fd);
}

int ui_loop(void)
{
	fprintf(stderr, "Loop\n");
	if (initload == 0) {
		gen_loadmemrom(initcart, initcart_len);
	}
	else {
		char *error = gen_loadimage(initload);
		if (error) {
			return 3;
		}
	}
	while (1) {
		event_doframe();
	}
	return 0;
}

int ui_init(int argc, const char *argv[])
{
	fprintf(stderr, "Ini startt\n");
	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1) {
		return 1;
	}
	vfb = mmap(0, FB_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, FB_ADDR);
	if (vfb == MAP_FAILED) {
		close(fd);
		return 2;
	}
	fprintf(stderr, "Init\n");
	if (argc >= 2) {
		initload = argv[1];
	}
	uiplot_setshifts(19, 11, 3);
	return 0;
}

void ui_musiclog(uint8 *data, unsigned int length)
{
}
