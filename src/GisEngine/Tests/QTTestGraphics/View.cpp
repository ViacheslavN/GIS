#include "stdafx.h"
#include <QPainter>
#include "View.h"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>

View::View(QWidget *parent)
	: QWidget(parent), m_pCurGraphics(nullptr), m_bAgg(true), m_bLine(false), m_bAddParts(false)
{
	ui.setupUi(this);
	setMouseTracking(true);


	m_brush.setColor(GisEngine::Display::Color::Red);
	m_brush.setBgColor(GisEngine::Display::Color::Red);
	m_pen.setColor(GisEngine::Display::Color(134,45,88,255));
	m_pen.setWidth(1);
}

View::~View()
{
}


void View::paintEvent(QPaintEvent* pEvent)
{

	if (m_bAddParts)
	{
		DrawAddsElement();
		return;
	}


	if (m_pCurGraphics == nullptr)
	{
		init();
	}
	if (m_bLine)
		DrawLine();
	else
		DrawPolygon();

	QImage image;
	Q_UNUSED(pEvent);


	GisEngine::Display::CBitmap& bitmap = m_pCurGraphics->GetSurface();

	QPixmap pixmap = QPixmap::fromImage(
		QImage(
		(unsigned char *)bitmap.bits(),
			bitmap.width(),
			bitmap.height(),
			QImage::Format_RGB32
		)
	);

	QPainter painter(this);
	painter.drawPixmap(0, 0, pixmap);
}


void View::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (!m_bAddParts)
		return;
		
	ClosePart();
	update();
}


void  View::ClosePart()
{
	if (m_vecCurrAddsPoints.empty())
		return;


	if (m_bLine)
	{
		if (m_vecCurrAddsPoints.size() < 2)
			return;
	}
	else
	{
		if (m_vecCurrAddsPoints.size() < 3)
			return;


		if (m_vecCurrAddsPoints[0] != m_vecCurrAddsPoints[m_vecCurrAddsPoints.size() - 1])
			m_vecCurrAddsPoints.push_back(m_vecCurrAddsPoints[0]);

	}

	for (size_t i = 0; i < m_vecCurrAddsPoints.size(); i++)
	{
		m_vecPoints.push_back(m_vecCurrAddsPoints[i]);
	}

	m_vecParts.push_back(m_vecCurrAddsPoints.size());
	m_vecCurrAddsPoints.clear();
}

void View::mousePressEvent(QMouseEvent *event)
{
	if (!m_bAddParts)
		return;

	if (event->button() == Qt::RightButton)
	{
		ClosePart();
	}
	else
	{

		QPoint pt = event->pos();
		m_vecCurrAddsPoints.push_back(GisEngine::Display::GPoint(pt.x(), pt.y()));
	}

	update();


}
void View::mouseMoveEvent(QMouseEvent *event)
{
	if (!m_bAddParts)
		return;

	m_lastMovePoint = event->pos();
	update();
}
void View::mouseReleaseEvent(QMouseEvent *event)
{

}
void View::resizeEvent(QResizeEvent *event)
{
	/*if (m_pCurGraphics != nullptr && (m_pCurGraphics->GetWidth() != width() || m_pCurGraphics->GetHeight() != height()))
		m_pCurGraphics = nullptr;
	*/
	QWidget::resizeEvent(event);
}


void View::init()
{
	m_pOpenGLGraphics.release();
	m_pAggGraphics.release();

	m_pOpenGLGraphics = new GisEngine::Display::CGraphicsOpenGLWin(0, width(), height(), true);
	m_pAggGraphics =  new GisEngine::Display::CGraphicsAgg(width(), height(), true);
	if (m_bAgg)
		m_pCurGraphics = m_pAggGraphics.get();
	else
		m_pCurGraphics = m_pOpenGLGraphics.get();
}

void View::DrawLine()
{
	m_pCurGraphics->Erase(GisEngine::Display::Color::White);
}
void View::DrawPolygon()
{

	m_pCurGraphics->StartDrawing();
	m_pCurGraphics->Erase(GisEngine::Display::Color::Black);

	if (!m_vecPoints.empty()) 
	{

		m_vecPoints.clear();
		m_vecParts.clear();





		m_vecPoints.push_back(GisEngine::Display::GPoint(1, 6));
		m_vecPoints.push_back(GisEngine::Display::GPoint(1, 1));
		m_vecPoints.push_back(GisEngine::Display::GPoint(4, 3));
		m_vecPoints.push_back(GisEngine::Display::GPoint(7, 1));
		m_vecPoints.push_back(GisEngine::Display::GPoint(7, 6));
		m_vecPoints.push_back(GisEngine::Display::GPoint(1, 6));
		m_vecParts.push_back(m_vecPoints.size());

		/*m_vecPoints.push_back(GisEngine::Display::GPoint(150, 150));
		m_vecPoints.push_back(GisEngine::Display::GPoint(150, 125));
		m_vecPoints.push_back(GisEngine::Display::GPoint(175, 125));
		m_vecPoints.push_back(GisEngine::Display::GPoint(175, 150));
		m_vecPoints.push_back(GisEngine::Display::GPoint(150, 150));
		m_vecParts.push_back(m_vecPoints.size() - m_vecParts[0]);*/

		


		m_pCurGraphics->DrawPolyPolygon(nullptr, &m_brush, &m_vecPoints[0], &m_vecParts[0], m_vecParts.size());

		int nOffcet = 0;
		for (size_t i = 0; i < m_vecParts.size(); ++i)
		{

			m_pCurGraphics->DrawLine(&m_pen, &m_vecPoints[0]+ nOffcet, m_vecParts[i]);
			nOffcet += m_vecParts[i];
		}




	}



	m_pCurGraphics->EndDrawing();
}


void View::Clear()
{
	m_vecCurrAddsPoints.clear();
	m_vecPoints.clear();
	m_vecParts.clear();
	update();
}
void View::AddParts(bool bAddParts)
{
	m_bAddParts = bAddParts;
	if (!m_bAddParts)
	{
		ClosePart();
		update();
	}
}


void View::Line(bool bLine)
{
	Clear();
	m_bLine = bLine;
	update();
}

void View::DrawAddsElement()
{
	if (m_bLine)
		DrawAddsLine();
	else
		DrawAddsPolygon();
}	

void  View::DrawAddsLine()
{
	QPainter painter(this);
	int nOffset = 0;


	painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap,	Qt::RoundJoin));

	for (size_t p = 0; p < m_vecParts.size(); ++p)
	{
		int nPart = m_vecParts[p];
		if(nPart < 2)
			continue;

		for (size_t i = 1; i < nPart; ++i)
		{
			auto gPoint0 = m_vecPoints[nOffset + i - 1];
			auto gPoint1 = m_vecPoints[nOffset + i];
			 
			painter.drawLine(QPoint(gPoint0.x, gPoint0.y), QPoint(gPoint1.x, gPoint1.y));
		}
		nOffset += nPart;
	}

	painter.setPen(QPen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	for (size_t i = 0; i < m_vecPoints.size(); ++i)
	{
		auto gPoint = m_vecPoints[i];
		painter.drawPoint(QPoint(gPoint.x, gPoint.y));
	}

	painter.setPen(QPen(Qt::red, 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
	if (!m_vecCurrAddsPoints.empty())
	{
		for (size_t i = 1; i < m_vecCurrAddsPoints.size(); ++i)
		{
			auto gPoint0 = m_vecCurrAddsPoints[i - 1];
			auto gPoint1 = m_vecCurrAddsPoints[i];

			painter.drawLine(QPoint(gPoint0.x, gPoint0.y), QPoint(gPoint1.x, gPoint1.y));
		}

		painter.setPen(QPen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		for (size_t i = 0; i < m_vecCurrAddsPoints.size(); ++i)
		{
			auto gPoint = m_vecCurrAddsPoints[i];
			painter.drawPoint(QPoint(gPoint.x, gPoint.y));
		}

		painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		auto gPoint0 = m_vecCurrAddsPoints[m_vecCurrAddsPoints.size() - 1];

		painter.drawLine(QPoint(gPoint0.x, gPoint0.y), m_lastMovePoint);
	}
	



}
void  View::DrawAddsPolygon()
{
	QPainter painter(this);
	int nOffset = 0;


	painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	for (size_t p = 0; p < m_vecParts.size(); ++p)
	{
		int nPart = m_vecParts[p];
		if (nPart < 2)
			continue;

		for (size_t i = 1; i < nPart; ++i)
		{
			auto gPoint0 = m_vecPoints[nOffset + i - 1];
			auto gPoint1 = m_vecPoints[nOffset + i];

			painter.drawLine(QPoint(gPoint0.x, gPoint0.y), QPoint(gPoint1.x, gPoint1.y));
		}
		nOffset += nPart;
	}

	painter.setPen(QPen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	for (size_t i = 0; i < m_vecPoints.size(); ++i)
	{
		auto gPoint = m_vecPoints[i];
		painter.drawPoint(QPoint(gPoint.x, gPoint.y));
	}

	painter.setPen(QPen(Qt::red, 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
	if (!m_vecCurrAddsPoints.empty())
	{
		for (size_t i = 1; i < m_vecCurrAddsPoints.size(); ++i)
		{
			auto gPoint0 = m_vecCurrAddsPoints[i - 1];
			auto gPoint1 = m_vecCurrAddsPoints[i];

			painter.drawLine(QPoint(gPoint0.x, gPoint0.y), QPoint(gPoint1.x, gPoint1.y));
		}

		painter.setPen(QPen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		for (size_t i = 0; i < m_vecCurrAddsPoints.size(); ++i)
		{
			auto gPoint = m_vecCurrAddsPoints[i];
			painter.drawPoint(QPoint(gPoint.x, gPoint.y));
		}

		painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		auto gPoint0 = m_vecCurrAddsPoints[0];
		auto gPoint1 = m_vecCurrAddsPoints[m_vecCurrAddsPoints.size() - 1];
		painter.drawLine(QPoint(gPoint0.x, gPoint0.y), m_lastMovePoint);
		painter.drawLine(QPoint(gPoint1.x, gPoint1.y), m_lastMovePoint);
	}

}


void View::AggOpenGL(bool bAgg)
{
	m_bAgg = !m_bAgg;

	if (m_bAgg)
		m_pCurGraphics = m_pAggGraphics.get();
	else
		m_pCurGraphics = m_pOpenGLGraphics.get();
	update();
}