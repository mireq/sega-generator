#include <QFormLayout>
#include <QRadioButton>
#include <QSettings>
#include <QSpinBox>
#include "Icon.h"
#include "Panels.h"

PerformancePanel::PerformancePanel(QWidget *parent):
	ConfigPanel(parent)
{
	QSettings settings;
	QFormLayout *layout = new QFormLayout(this);
	QSpinBox *frameskip = new QSpinBox(this);
	frameskip->setObjectName("frameskip");
	frameskip->setMinimum(0);
	frameskip->setMaximum(10);
	frameskip->setSpecialValueText(tr("Automatic"));
	frameskip->setValue(settings.value("frameskip", 0).toInt());
	layout->addRow(tr("Frameskip"), frameskip);
	QRadioButton *lineRenderer = new QRadioButton(tr("Line"), this);
	lineRenderer->setObjectName("line");
	QRadioButton *cellRenderer = new QRadioButton(tr("Cell"), this);
	cellRenderer->setObjectName("cell");
	layout->addRow(tr("Rendering method"), lineRenderer);
	layout->addRow("", cellRenderer);
	int renderer = settings.value("renderer", 0).toInt();
	if (renderer) {
		cellRenderer->setChecked(true);
	}
	else {
		lineRenderer->setChecked(true);
	}
	setLayout(layout);
}

PerformancePanel::~PerformancePanel()
{
}

QString PerformancePanel::panelName() const
{
	return tr("Performance");
}

QIcon PerformancePanel::icon() const
{
	return Icon("cpu");
}

void PerformancePanel::saveChanges()
{
	QSettings settings;
	settings.setValue("frameskip", findChild<QSpinBox *>("frameskip")->property("value"));
	if (findChild<QRadioButton *>("cell")->property("checked").toBool()) {
		settings.setValue("renderer", 1);
	}
	else {
		settings.setValue("renderer", 0);
	}
}

