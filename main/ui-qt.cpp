#include <QApplication>
#include <QMainWindow>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "qt/QtGeneratorWindow.h"

static QApplication *app = 0;
static QtGeneratorWindow *win = 0;
static int argument_count;

extern "C"
{

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
	win->uiLine(line);
}

void ui_endfield()
{
	win->uiEndField();
}

void ui_final(void)
{
	if (app != 0) {
		delete win;
		delete app;
	}
}

int ui_loop(void)
{
	return app->exec();
}

int ui_init(int argc, char *argv[])
{
	argument_count = argc;
	app = new QApplication(argument_count, argv);
	QApplication::setOrganizationName("Squish");
	QApplication::setOrganizationDomain("squish.net");
	QApplication::setApplicationName("Generator");
	win = new QtGeneratorWindow();
	win->show();
	return win->uiInit(argc, argv);
}

void ui_musiclog(uint8 *data, unsigned int length)
{
	win->uiMusiclog(data, length);
}

}
