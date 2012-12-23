#ifndef KEYSEQUENCEEDIT_H_NYHXXKRM
#define KEYSEQUENCEEDIT_H_NYHXXKRM

#include <QKeySequence>
#include <QWidget>

class KeySequenceEdit: public QWidget
{
Q_OBJECT
public:
	KeySequenceEdit(QWidget *parent = 0);
	~KeySequenceEdit();
	QKeySequence keySequence() const;

public slots:
	void setKeySequence(const QKeySequence &sequence);
	void clearKeySequence();

signals:
	void keySequenceChanged(const QKeySequence &sequence);

protected:
	bool event(QEvent *event);
	void keyPressEvent(QKeyEvent *event);

private:
	void processKeyPress(QKeyEvent *event);
	int translateModifiers(Qt::KeyboardModifiers modifiers);

private:
	QKeySequence m_keySequence;
}; /* -----  end of class KeySequenceEdit  ----- */

#endif /* end of include guard: KEYSEQUENCEEDIT_H_NYHXXKRM */

