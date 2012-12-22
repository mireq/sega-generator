#include "generator.h"

void ui_err(const char *text, ...)
{
	va_list ap;
	vfprintf(stderr, text, ap);
	putc(10, stderr);
	exit(0);
}

void ui_line(int line)
{
}

void ui_endfield()
{
}

void ui_final(void)
{
}

int ui_loop(void)
{
}

int ui_init(int argc, char *argv[])
{
}

void ui_musiclog(uint8 *data, unsigned int length)
{
}
