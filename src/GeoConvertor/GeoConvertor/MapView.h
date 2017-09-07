#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QtWidgets/QWidget>
#include "ui_MapView.h"

class CMapView : public QFrame
{
	Q_OBJECT

public:
	CMapView(QWidget *parent = 0);
	~CMapView();
	 void paintEvent ( QPaintEvent* evnt );
private:
	//Ui::MapView ui;
};

#endif // MAPVIEW_H
