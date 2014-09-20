#include "EmulatorItem.h"
#include <QSGSimpleTextureNode>
#include <QImage>
#include <QQuickWindow>

extern "C" {
#include "uiplot.h"
#include "vdp.h"
}

#define GFX_WIDTH (320 - 16)
#define GFX_HEIGHT (224)

EmulatorItem::EmulatorItem(QQuickItem *parent):
	QQuickItem(parent),
	m_texture(0),
	m_gfx(0)
{
	setFlags(QQuickItem::ItemAcceptsInputMethod | QQuickItem::ItemIsFocusScope | QQuickItem::ItemHasContents);
}

EmulatorItem::~EmulatorItem()
{
	delete m_texture;
	delete[] m_gfx;
}

QSGNode *EmulatorItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData)
{
	QSGSimpleTextureNode *n = static_cast<QSGSimpleTextureNode *>(oldNode);
	if (!n) {
		connect(window(), SIGNAL(frameFinished()), SLOT(onFrameFinished()));
		n = new QSGSimpleTextureNode();
		QImage img(GFX_WIDTH, GFX_HEIGHT, QImage::Format_RGBA8888);
		m_gfx = new GLubyte[GFX_WIDTH * GFX_HEIGHT * 4];
		img.fill(Qt::green);
		m_texture = window()->createTextureFromImage(img);
		m_texture->setFiltering(QSGTexture::Linear);
		n->setTexture(m_texture);
	}
	//qDebug() << m_texture->textureId();
	n->setRect(boundingRect());

	m_texture->bind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GFX_WIDTH, GFX_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_gfx);

	return n;
}

void EmulatorItem::onFrameFinished()
{
	QmlGeneratorWindow *win = qobject_cast<QmlGeneratorWindow *>(window());
	if (!win || !m_gfx) {
		return;
	}

	const uint32 *screen = win->screen();
	const uint32 *indata;
	uint32 *outdata = reinterpret_cast<uint32 *>(m_gfx);
	unsigned int x;
	int l;
	uint32 col;

	for (l = 0; l < vdp_vislines; ++l) {
		indata = screen + 8 + (l + 8) * (320 + 16);
		for (x = 0; x < GFX_WIDTH; ++x) {
			outdata[x] = indata[x];
		}
		outdata += GFX_WIDTH;
	}

	update();
}

