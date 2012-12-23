#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QCloseEvent>
#include "ConfigPanel.h"
#include "ConfigDialog.h"

ConfigDialog::ConfigDialog(QWidget *parent):
	QDialog(parent)
{
	setWindowTitle(tr("Configure"));

	m_currentItem = NULL;
	m_previousItem = NULL;

	m_panelsList = new QListWidget();
	m_panelsList->setFixedWidth(200);
	m_panelsList->setIconSize(QSize(48, 48));
	m_panelsList->setUniformItemSizes(true);
	m_panelsList->setWordWrap(true);
	m_panelsList->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(m_panelsList, SIGNAL(itemSelectionChanged()), SLOT(listSelectionChanged()));

	m_panel = new QStackedWidget();

	QVBoxLayout *layout = new QVBoxLayout();
	QHBoxLayout *centerLayout = new QHBoxLayout();
	QVBoxLayout *panelLayout = new QVBoxLayout();

	layout->addLayout(centerLayout, 1);
	centerLayout->addWidget(m_panelsList);
	centerLayout->addLayout(panelLayout);
	panelLayout->addWidget(m_panel, 1);

	m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
	layout->addWidget(m_buttons);
	setLayout(layout);

	connect(m_buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(onOkClicked()));
}

ConfigDialog::~ConfigDialog()
{
}

void ConfigDialog::addPanel(ConfigPanel *panel)
{
	QListWidgetItem *newItem = new QListWidgetItem();
	newItem->setData(Qt::DisplayRole, panel->panelName());
	newItem->setData(Qt::DecorationRole, panel->icon());
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	m_panelsList->addItem(newItem);
	m_panel->addWidget(panel);
	if (m_panelsList->model()->rowCount(QModelIndex()) == 1)
	{
		m_panelsList->setCurrentRow(0);
	}
}

void ConfigDialog::closeEvent(QCloseEvent *e)
{
	if (currentPanel()) {
		currentPanel()->saveChanges();
	}
}

void ConfigDialog::showEvent(QShowEvent *e)
{
	Q_UNUSED(e);
	m_currentItem = NULL;
	m_previousItem = NULL;
	m_panelsList->setCurrentRow(0);
	listSelectionChanged();
}

ConfigPanel *ConfigDialog::currentPanel() const
{
	return static_cast<ConfigPanel *>(m_panel->currentWidget());
}

void ConfigDialog::listSelectionChanged()
{
	QListWidgetItem *old = m_currentItem;
	m_currentItem = m_panelsList->currentItem();

	if (m_currentItem == NULL) {
		return;
	}

	bool changePanel = true;
	if (old != NULL) {
		int prevRow = m_panelsList->row(old);
		ConfigPanel *panel = static_cast<ConfigPanel *>(m_panel->widget(prevRow));
		panel->saveChanges();
	}

	int currRow = m_panelsList->row(m_currentItem);
	ConfigPanel *panel = static_cast<ConfigPanel *>(m_panel->widget(currRow));
	m_panel->setCurrentIndex(currRow);
	m_previousItem = m_currentItem;
}

void ConfigDialog::onOkClicked()
{
	if (currentPanel()) {
		currentPanel()->saveChanges();
	}
	accept();
}
