#include <QComboBox>
#include <QFormLayout>
#include <QRadioButton>
#include <QSettings>
#include <QSpinBox>
#include <QVideoFrame>
#include "Icon.h"
#include "Panels.h"
#include "QtXvControlsWidget.h"
#include "QtXvWidget.h"

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


DisplayPanel::DisplayPanel(QtXvWidget *xv, QWidget *parent):
	ConfigPanel(parent),
	m_xv(xv)
{
	QSettings settings;
	XvPortID baseId = settings.value("adaptor", 0).value<XvPortID>();

	QVBoxLayout *layout = new QVBoxLayout(this);
	setLayout(layout);
	QComboBox *adaptors = new QComboBox(this);
	adaptors->setObjectName("adaptor");
	layout->addWidget(adaptors);
	layout->addWidget(new QtXvControlsWidget(m_xv, this));

	QtXvWidget::AdaptorList adaptorList = m_xv->adaptors();
	foreach (const QtXvWidget::AdaptorInfo &adaptorInfo, adaptorList) {
		adaptors->addItem(adaptorInfo.name, QVariant::fromValue<XvPortID>(adaptorInfo.baseId));
		if (adaptorInfo.baseId == baseId) {
			adaptors->setCurrentIndex(adaptors->count() - 1);
		}
	}

	connect(adaptors, SIGNAL(currentIndexChanged(int)), SLOT(adaptorChanged(int)));
}

DisplayPanel::~DisplayPanel()
{
}

QString DisplayPanel::panelName() const
{
	return tr("Display");
}

QIcon DisplayPanel::icon() const
{
	return Icon("video-display");
}

void DisplayPanel::saveChanges()
{
	QSettings settings;
	QComboBox *adaptor = findChild<QComboBox *>("adaptor");
	settings.setValue("adaptor", adaptor->itemData(adaptor->currentIndex()));
}

void DisplayPanel::adaptorChanged(int adaptor)
{
	XvPortID basePort = findChild<QComboBox *>("adaptor")->itemData(adaptor).value<XvPortID>();
	m_xv->present(QVideoFrame());
	m_xv->setAdaptor(basePort);
}

