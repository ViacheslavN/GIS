#ifndef GEOCONVERTOR_H
#define GEOCONVERTOR_H

#include <QtWidgets/QMainWindow>
#include "ui_geoconvertor.h"

class CLegendView;
class CMapView;
class GeoConvertor : public QMainWindow, private Ui::GeoConvertorClass 
{
	Q_OBJECT

public:
	GeoConvertor(QWidget *parent = 0);
	~GeoConvertor();

private:
	CLegendView *m_pLegendView;
	CMapView *m_pMapView;
private:
	 
};

#endif // GEOCONVERTOR_H
