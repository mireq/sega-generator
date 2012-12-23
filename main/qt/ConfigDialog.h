#ifndef CONFIGDIALOG_H_4QHSXJUU
#define CONFIGDIALOG_H_4QHSXJUU

#include <QDialog>
class ConfigPanel;
class QDialogButtonBox;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class ConfigDialog: public QDialog
{
Q_OBJECT
public:
	ConfigDialog(QWidget *parent = 0);
	~ConfigDialog();
	void addPanel(ConfigPanel *panel);

protected:
	void closeEvent(QCloseEvent *e);

private:
	ConfigPanel *currentPanel() const;

private slots:
	void listSelectionChanged();
	void onOkClicked();

private:
	QListWidget *m_panelsList;
	QStackedWidget *m_panel;
	QDialogButtonBox *m_buttons;

	QListWidgetItem *m_currentItem;
	QListWidgetItem *m_previousItem;
}; /* -----  end of class ConfigDialog  ----- */

#endif /* end of include guard: CONFIGDIALOG_H_4QHSXJUU */

