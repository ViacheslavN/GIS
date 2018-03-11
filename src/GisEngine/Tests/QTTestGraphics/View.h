#pragma once

#include <QtWidgets/QWidget>
#include "ui_View.h"
#include <QEvent>
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>

#include "../../Display/Graphics.h"
#include "../../Display/GraphTypes.h"
#include "../../Display/GraphicsAgg.h"
#include "../../Display/GraphicsOpenGLWin.h"
#include "../../Display/GraphicsWinGDI.h"
#include "../../Display/Pen.h"
#include "../../Display/Brush.h"

class View : public QWidget
{
	Q_OBJECT

public:
	View(QWidget *parent = Q_NULLPTR);
	~View();
	

	void Clear();
	void AddParts(bool bAddParts);
	void Line(bool bLine);
	void AggOpenGL(bool bAgg);
private:
	void init();
	void DrawLine();
	void DrawPolygon();
	void DrawAddsElement();

	void DrawAddsLine();
	void DrawAddsPolygon();

	void ClosePart();

protected:
	void paintEvent(QPaintEvent* evnt) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseDoubleClickEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;

private:
	Ui::View ui;
	bool m_bAgg;
	bool m_bLine;
	bool m_bAddParts;

	

	GisEngine::Display::IGraphics* m_pCurGraphics;
	GisEngine::Display::IGraphicsPtr m_pAggGraphics;
	GisEngine::Display::IGraphicsPtr m_pOpenGLGraphics;

	std::vector<GisEngine::Display::GPoint> m_vecPoints;
	std::vector<int> m_vecParts;



	std::vector<GisEngine::Display::GPoint> m_vecCurrAddsPoints;

	GisEngine::Display::CBrush m_brush;
	GisEngine::Display::CPen m_pen;

	QPoint  m_lastMovePoint;
};
