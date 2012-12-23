#include <QComboBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QRadioButton>
#include <QScrollArea>
#include <QSettings>
#include <QSpinBox>
#include <QVideoFrame>
#include "Icon.h"
#include "KeySequenceEdit.h"
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


InputPanel::InputPanel(QWidget *parent):
	ConfigPanel(parent)
{
	QSettings settings;
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	setLayout(mainLayout);
	QWidget *w = new QWidget(this);
	QScrollArea *area = new QScrollArea(this);
	area->setWidgetResizable(true);
	mainLayout->addWidget(area);

	QVBoxLayout *layout = new QVBoxLayout(w);

	KeySequenceEdit *i1a     = new KeySequenceEdit(this);
	KeySequenceEdit *i1b     = new KeySequenceEdit(this);
	KeySequenceEdit *i1c     = new KeySequenceEdit(this);
	KeySequenceEdit *i1left  = new KeySequenceEdit(this);
	KeySequenceEdit *i1right = new KeySequenceEdit(this);
	KeySequenceEdit *i1up    = new KeySequenceEdit(this);
	KeySequenceEdit *i1down  = new KeySequenceEdit(this);
	KeySequenceEdit *i1start = new KeySequenceEdit(this);

	i1a->setKeySequence(settings.value("key_i1a", QKeySequence("Z")).value<QKeySequence>());
	i1b->setKeySequence(settings.value("key_i1b", QKeySequence("X")).value<QKeySequence>());
	i1c->setKeySequence(settings.value("key_i1c", QKeySequence("C")).value<QKeySequence>());
	i1left->setKeySequence(settings.value("key_i1left", QKeySequence("Left")).value<QKeySequence>());
	i1right->setKeySequence(settings.value("key_i1right", QKeySequence("Right")).value<QKeySequence>());
	i1up->setKeySequence(settings.value("key_i1up", QKeySequence("Up")).value<QKeySequence>());
	i1down->setKeySequence(settings.value("key_i1down", QKeySequence("Down")).value<QKeySequence>());
	i1start->setKeySequence(settings.value("key_i1start", QKeySequence("Return")).value<QKeySequence>());

	i1a->setObjectName("i1a");
	i1b->setObjectName("i1b");
	i1c->setObjectName("i1c");
	i1left->setObjectName("i1left");
	i1right->setObjectName("i1right");
	i1up->setObjectName("i1up");
	i1down->setObjectName("i1down");
	i1start->setObjectName("i1start");

	QGroupBox *i1 = new QGroupBox(tr("Controller 1"));
	QFormLayout *i1l = new QFormLayout(i1);
	i1->setLayout(i1l);
	i1l->addRow(tr("A"), i1a);
	i1l->addRow(tr("B"), i1b);
	i1l->addRow(tr("C"), i1c);
	i1l->addRow(tr("Left"), i1left);
	i1l->addRow(tr("Right"), i1right);
	i1l->addRow(tr("Up"), i1up);
	i1l->addRow(tr("Down"), i1down);
	i1l->addRow(tr("Start"), i1start);
	layout->addWidget(i1);

	KeySequenceEdit *i2a     = new KeySequenceEdit(this);
	KeySequenceEdit *i2b     = new KeySequenceEdit(this);
	KeySequenceEdit *i2c     = new KeySequenceEdit(this);
	KeySequenceEdit *i2left  = new KeySequenceEdit(this);
	KeySequenceEdit *i2right = new KeySequenceEdit(this);
	KeySequenceEdit *i2up    = new KeySequenceEdit(this);
	KeySequenceEdit *i2down  = new KeySequenceEdit(this);
	KeySequenceEdit *i2start = new KeySequenceEdit(this);

	i2a->setKeySequence(settings.value("key_i2a", QKeySequence()).value<QKeySequence>());
	i2b->setKeySequence(settings.value("key_i2b", QKeySequence()).value<QKeySequence>());
	i2c->setKeySequence(settings.value("key_i2c", QKeySequence()).value<QKeySequence>());
	i2left->setKeySequence(settings.value("key_i2left", QKeySequence()).value<QKeySequence>());
	i2right->setKeySequence(settings.value("key_i2right", QKeySequence()).value<QKeySequence>());
	i2up->setKeySequence(settings.value("key_i2up", QKeySequence()).value<QKeySequence>());
	i2down->setKeySequence(settings.value("key_i2down", QKeySequence()).value<QKeySequence>());
	i2start->setKeySequence(settings.value("key_i2start", QKeySequence()).value<QKeySequence>());

	i2a->setObjectName("i2a");
	i2b->setObjectName("i2b");
	i2c->setObjectName("i2c");
	i2left->setObjectName("i2left");
	i2right->setObjectName("i2right");
	i2up->setObjectName("i2up");
	i2down->setObjectName("i2down");
	i2start->setObjectName("i2start");

	QGroupBox *i2 = new QGroupBox(tr("Controller 2"));
	QFormLayout *i2l = new QFormLayout(i2);
	i2->setLayout(i2l);
	i2l->addRow(tr("A"), i2a);
	i2l->addRow(tr("B"), i2b);
	i2l->addRow(tr("C"), i2c);
	i2l->addRow(tr("Left"), i2left);
	i2l->addRow(tr("Right"), i2right);
	i2l->addRow(tr("Up"), i2up);
	i2l->addRow(tr("Down"), i2down);
	i2l->addRow(tr("Start"), i2start);
	layout->addWidget(i2);

	KeySequenceEdit *openROM   = new KeySequenceEdit(this);
	KeySequenceEdit *loadState = new KeySequenceEdit(this);
	KeySequenceEdit *saveState = new KeySequenceEdit(this);
	KeySequenceEdit *quit      = new KeySequenceEdit(this);
	KeySequenceEdit *configure = new KeySequenceEdit(this);

	openROM->setKeySequence(settings.value("key_openROM", QKeySequence("Ctrl+O")).value<QKeySequence>());
	loadState->setKeySequence(settings.value("key_loadState", QKeySequence("F2")).value<QKeySequence>());
	saveState->setKeySequence(settings.value("key_saveState", QKeySequence("F3")).value<QKeySequence>());
	quit->setKeySequence(settings.value("key_quit", QKeySequence("Ctrl+Q")).value<QKeySequence>());
	configure->setKeySequence(settings.value("key_configure", QKeySequence("Ctrl+P")).value<QKeySequence>());

	openROM->setObjectName("openROM");
	loadState->setObjectName("loadState");
	saveState->setObjectName("saveState");
	quit->setObjectName("quit");
	configure->setObjectName("configure");

	QGroupBox *actions = new QGroupBox(tr("Actions"));
	QFormLayout *al = new QFormLayout(actions);
	actions->setLayout(al);
	al->addRow(tr("&Open ROM"), openROM);
	al->addRow(tr("&Load state"), loadState);
	al->addRow(tr("&Save state"), saveState);
	al->addRow(tr("&Quit"), quit);
	al->addRow(tr("&Configure"), configure);
	layout->addWidget(actions);

	w->setLayout(layout);
	area->setWidget(w);
}

InputPanel::~InputPanel()
{
}

QString InputPanel::panelName() const
{
	return tr("Input");
}

QIcon InputPanel::icon() const
{
	return Icon("input-gaming");
}

void InputPanel::saveChanges()
{
	QSettings settings;
	settings.setValue("key_i1a"      , findChild<KeySequenceEdit *>("i1a")->keySequence());
	settings.setValue("key_i1b"      , findChild<KeySequenceEdit *>("i1b")->keySequence());
	settings.setValue("key_i1c"      , findChild<KeySequenceEdit *>("i1c")->keySequence());
	settings.setValue("key_i1left"   , findChild<KeySequenceEdit *>("i1left")->keySequence());
	settings.setValue("key_i1right"  , findChild<KeySequenceEdit *>("i1right")->keySequence());
	settings.setValue("key_i1up"     , findChild<KeySequenceEdit *>("i1up")->keySequence());
	settings.setValue("key_i1down"   , findChild<KeySequenceEdit *>("i1down")->keySequence());
	settings.setValue("key_i1start"  , findChild<KeySequenceEdit *>("i1start")->keySequence());

	settings.setValue("key_i2a"      , findChild<KeySequenceEdit *>("i2a")->keySequence());
	settings.setValue("key_i2b"      , findChild<KeySequenceEdit *>("i2b")->keySequence());
	settings.setValue("key_i2c"      , findChild<KeySequenceEdit *>("i2c")->keySequence());
	settings.setValue("key_i2left"   , findChild<KeySequenceEdit *>("i2left")->keySequence());
	settings.setValue("key_i2right"  , findChild<KeySequenceEdit *>("i2right")->keySequence());
	settings.setValue("key_i2up"     , findChild<KeySequenceEdit *>("i2up")->keySequence());
	settings.setValue("key_i2down"   , findChild<KeySequenceEdit *>("i2down")->keySequence());
	settings.setValue("key_i2start"  , findChild<KeySequenceEdit *>("i2start")->keySequence());

	settings.setValue("key_openROM"  , findChild<KeySequenceEdit *>("openROM")->keySequence());
	settings.setValue("key_loadState", findChild<KeySequenceEdit *>("loadState")->keySequence());
	settings.setValue("key_saveState", findChild<KeySequenceEdit *>("saveState")->keySequence());
	settings.setValue("key_quit"     , findChild<KeySequenceEdit *>("quit")->keySequence());
	settings.setValue("key_configure", findChild<KeySequenceEdit *>("configure")->keySequence());
}

