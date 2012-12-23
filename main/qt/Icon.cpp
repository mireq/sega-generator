#include "Icon.h"
#include <QFile>


const int Icon::sm_size[7] = {16, 22, 32, 48, 64, 128, 0};


Icon::Icon(const QString &name, bool showUnknownIcon)
{
	scanFiles(name);
	if (!availableSizes().count()) {
		if (!showUnknownIcon) {
			return;
		}
		scanFiles("unknown");
	}
}

Icon::~Icon()
{
}

void Icon::scanFiles(const QString &name)
{
	QString prefix = ":icons/";
	QString filename = name + QString(".png");
	for (int i = 0; i < sizeof(sm_size) / sizeof(const int); ++i) {
		QString fullName = prefix + QString("%1x%1").arg(sm_size[i]) + QString("/") + filename;
		if (QFile::exists(fullName))
		{
			addFile(fullName, QSize(sm_size[i], sm_size[i]));
		}
	}
}

