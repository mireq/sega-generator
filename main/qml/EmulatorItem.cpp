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
	m_screen(0)
{
	setFlags(QQuickItem::ItemAcceptsInputMethod | QQuickItem::ItemIsFocusScope | QQuickItem::ItemHasContents);
}

EmulatorItem::~EmulatorItem()
{
	if (m_texture) {
		delete m_texture;
	}
	if (m_screen) {
		delete[] m_screen;
	}
}

QSGNode *EmulatorItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData)
{
	QSGSimpleTextureNode *n = static_cast<QSGSimpleTextureNode *>(oldNode);
	if (!n) {
		m_gl = window()->openglContext()->functions();
		connect(window(), SIGNAL(frameFinished()), this, SLOT(onFrameFinished()));
		n = new QSGSimpleTextureNode();
		QImage img(GFX_WIDTH, GFX_HEIGHT, QImage::Format_RGBA8888);
		m_screen = new GLuint[GFX_WIDTH * GFX_HEIGHT];
		img.fill(Qt::green);
		m_texture = window()->createTextureFromImage(img);
		m_texture->setFiltering(QSGTexture::Linear);
		n->setTexture(m_texture);
		n->setRect(boundingRect());
	}

	m_texture->bind();
	m_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GFX_WIDTH - 16, GFX_HEIGHT - 32, 0, GL_RGB, GL_UNSIGNED_BYTE, m_screen);

	return n;
}

void EmulatorItem::onFrameFinished()
{
	QmlGeneratorWindow *win = qobject_cast<QmlGeneratorWindow *>(window());
	uiplot_checkpalcache(0);
	uiplot_convertdata32((uint8*)win->gfx(), m_screen, (320 + 16) * (240 + 16));

	GLubyte *raw_screen = reinterpret_cast<GLubyte *>(m_screen);
	GLubyte *raw_src_screen = reinterpret_cast<GLubyte *>(m_screen);
	raw_src_screen += GFX_WIDTH * 8 * 4;
	for (int row = 0; row < GFX_HEIGHT - 32; ++row) {
		raw_src_screen += 32;
		for (int col = 0; col < GFX_WIDTH - 16; ++col) {
			raw_screen[0] = raw_src_screen[0];
			raw_screen[1] = raw_src_screen[1];
			raw_screen[2] = raw_src_screen[2];
			raw_screen += 3;
			raw_src_screen += 4;
		}
		raw_src_screen += 32;
	}
	update();
}

