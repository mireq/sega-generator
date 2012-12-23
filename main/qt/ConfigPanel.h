#ifndef CONFIGPANEL_H_FJ4MC32B
#define CONFIGPANEL_H_FJ4MC32B


#include <QWidget>
#include <QIcon>


class ConfigPanel: public QWidget
{
Q_OBJECT
public:
	ConfigPanel(QWidget *parent = 0);
	~ConfigPanel();
	virtual QString panelName() const = 0;
	virtual QIcon icon() const = 0;
	virtual void saveChanges() = 0;
}; /* -----  end of class ConfigPanel  ----- */

#endif /* end of include guard: CONFIGPANEL_H_FJ4MC32B */

