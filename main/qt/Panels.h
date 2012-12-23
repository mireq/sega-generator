#ifndef PANELS_H_LWXGEDQL
#define PANELS_H_LWXGEDQL

#include "ConfigPanel.h"
class QtXvWidget;


class PerformancePanel: public ConfigPanel
{
Q_OBJECT
public:
	PerformancePanel(QWidget *parent = 0);
	~PerformancePanel();
	QString panelName() const;
	QIcon icon() const;
	void saveChanges();
}; /* -----  end of class PerformancePanel  ----- */


class DisplayPanel: public ConfigPanel
{
Q_OBJECT
public:
	DisplayPanel(QtXvWidget *xv, QWidget *parent = 0);
	~DisplayPanel();
	QString panelName() const;
	QIcon icon() const;
	void saveChanges();

private slots:
	void adaptorChanged(int adaptor);

private:
	QtXvWidget *m_xv;
}; /* -----  end of class DisplayPanel  ----- */


class InputPanel: public ConfigPanel
{
Q_OBJECT
public:
	InputPanel(QWidget *parent = 0);
	~InputPanel();
	QString panelName() const;
	QIcon icon() const;
	void saveChanges();
}; /* -----  end of class InputPanel  ----- */


#endif /* end of include guard: PANELS_H_LWXGEDQL */

