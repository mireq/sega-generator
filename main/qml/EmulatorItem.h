#ifndef EMULATORITEM_H_J29AQOQV
#define EMULATORITEM_H_J29AQOQV

#include <QQuickItem>
#include <QSGTexture>
#include <QtGui/QOpenGLFunctions>
#include "QmlGeneratorWindow.h"


class EmulatorItem: public QQuickItem
{
Q_OBJECT
public:
	EmulatorItem(QQuickItem *parent = 0);
	~EmulatorItem();

protected:
	QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData);

private slots:
	void onFrameFinished();

private:
	QSGTexture *m_texture;
	GLuint *m_screen;

	QOpenGLFunctions *m_gl;
}; /* -----  end of class EmulatorItem  ----- */


#endif /* end of include guard: EMULATORITEM_H_J29AQOQV */
