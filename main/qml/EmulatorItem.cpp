#include "EmulatorItem.h"
#include <QSGSimpleTextureNode>
#include <QImage>
#include <QQuickWindow>
#include <cstring>

extern "C" {
#include "uiplot.h"
#include "vdp.h"
}

#define GFX_WIDTH (320 + 16)
#define GFX_HEIGHT (240 + 16)

EmulatorItem::EmulatorItem(QQuickItem *parent):
	QQuickItem(parent),
	m_texture(0),
	m_gfx(0),
	m_screen(0)
{
	setFlags(QQuickItem::ItemAcceptsInputMethod | QQuickItem::ItemIsFocusScope | QQuickItem::ItemHasContents);
}

EmulatorItem::~EmulatorItem()
{
	delete m_texture;
	delete[] m_gfx;
	delete[] m_screen;
}

QSGNode *EmulatorItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData)
{
	QSGSimpleTextureNode *n = static_cast<QSGSimpleTextureNode *>(oldNode);
	if (!n) {
		connect(window(), SIGNAL(frameFinished()), this, SLOT(onFrameFinished()), Qt::QueuedConnection);
		n = new QSGSimpleTextureNode();
		QImage img(GFX_WIDTH, GFX_HEIGHT, QImage::Format_RGBA8888);
		m_gfx = new GLubyte[GFX_WIDTH * GFX_HEIGHT];
		m_screen = new GLuint[GFX_WIDTH * GFX_HEIGHT];
		img.fill(Qt::green);
		m_texture = window()->createTextureFromImage(img);
		m_texture->setFiltering(QSGTexture::Linear);
		n->setTexture(m_texture);
		n->setRect(boundingRect());
	}

	QmlGeneratorWindow *win = qobject_cast<QmlGeneratorWindow *>(window());
	uiplot_checkpalcache(0);
	uiplot_convertdata32((uint8*)win->gfx(), m_screen, (320 + 16) * (240 + 16));

	m_texture->bind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GFX_WIDTH, GFX_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_screen);
	n->markDirty(QSGNode::DirtyMaterial);

	return n;
}

void EmulatorItem::onFrameFinished()
{
	update();
}

