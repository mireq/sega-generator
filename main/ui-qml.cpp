#include <QGuiApplication>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "qml/QmlGeneratorWindow.h"
#include "qml/EmulatorItem.h"

static QGuiApplication *app = 0;
static QmlGeneratorWindow *win = 0;
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
	app = new QGuiApplication(argument_count, argv);
	QGuiApplication::setOrganizationName("Squish");
	QGuiApplication::setOrganizationDomain("squish.net");
	QGuiApplication::setApplicationName("Generator");

	qmlRegisterType<EmulatorItem>("generator", 1, 0, "EmulatorItem");

	win = new QmlGeneratorWindow();
	win->show();
	return win->uiInit(argc, argv);
}

void ui_musiclog(uint8 *data, unsigned int length)
{
	win->uiMusiclog(data, length);
}

}

