
#include "MapView.h"
 #include <QPainter>
CMapView::CMapView(QWidget *parent)
	: QFrame(parent)
{
	//ui.setupUi(this);
}

CMapView::~CMapView()
{

}

 void CMapView::paintEvent ( QPaintEvent* evnt )
 {

	 QSize sz  = this->size();
	 int dd = 0;
	 dd++;
	 Q_UNUSED(evnt);

	 QPainter painter;
	 painter.begin( this );
	 painter.fillRect(this->rect(), Qt::white);
	 painter.drawLine(0, 0,  100, 100);

	 painter.end();
 }