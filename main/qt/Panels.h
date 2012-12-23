#ifndef PANELS_H_LWXGEDQL
#define PANELS_H_LWXGEDQL

#include "ConfigPanel.h"


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


#endif /* end of include guard: PANELS_H_LWXGEDQL */

