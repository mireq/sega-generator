#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QVBoxLayout>
#include "KeySequenceEdit.h"

KeySequenceEdit::KeySequenceEdit(QWidget *parent):
	QWidget(parent)
{
	setAttribute(Qt::WA_InputMethodEnabled);
	setFocusPolicy(Qt::StrongFocus);

	QLineEdit *edit = new QLineEdit(this);
	edit->setObjectName("edit");
	edit->setReadOnly(true);
	edit->setFocusProxy(this);
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(edit);
}

KeySequenceEdit::~KeySequenceEdit()
{
}

QKeySequence KeySequenceEdit::keySequence() const
{
	return m_keySequence;
}

void KeySequenceEdit::setKeySequence(const QKeySequence &sequence)
{
	m_keySequence = sequence;
	findChild<QLineEdit *>("edit")->setText(sequence.toString(QKeySequence::NativeText));
}

void KeySequenceEdit::clearKeySequence()
{
	if (m_keySequence.isEmpty()) {
		return;
	}
	setKeySequence(QKeySequence());
	emit keySequenceChanged(m_keySequence);
}

bool KeySequenceEdit::event(QEvent *event)
{
	if (event->type() == QEvent::Shortcut || event->type() == QEvent::ShortcutOverride) {
		event->accept();
		return true;
	}
	return QWidget::event(event);
}

void KeySequenceEdit::keyPressEvent(QKeyEvent *event)
{
	processKeyPress(event);
	event->accept();
}

void KeySequenceEdit::processKeyPress(QKeyEvent *event)
{
	int pressedKey = event->key();
	switch (pressedKey) {
		case Qt::Key_Shift:
		case Qt::Key_Control:
		case Qt::Key_Meta:
		case Qt::Key_Alt:
		case Qt::Key_AltGr:
		case Qt::Key_Super_L:
		case Qt::Key_Super_R:
			return;
		default:
			break;
	}

	pressedKey |= translateModifiers(event->modifiers());

	m_keySequence = QKeySequence(pressedKey);
	setKeySequence(m_keySequence);
	emit keySequenceChanged(m_keySequence);
}

int KeySequenceEdit::translateModifiers(Qt::KeyboardModifiers modifiers)
{
	int ret = 0;
	if (modifiers & Qt::ShiftModifier) {
		ret |= Qt::SHIFT;
	}
	if (modifiers & Qt::MetaModifier) {
		ret |= Qt::META;
	}
	if (modifiers & Qt::ControlModifier) {
		ret |= Qt::CTRL;
	}
	if (modifiers & Qt::AltModifier) {
		ret |= Qt::ALT;
	}
	return ret;
}

