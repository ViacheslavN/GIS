#ifndef GEOCONVERTOR_H
#define GEOCONVERTOR_H

#include <QtWidgets/QMainWindow>
#include "ui_geoconvertor.h"

class GeoConvertor : public QMainWindow
{
	Q_OBJECT

public:
	GeoConvertor(QWidget *parent = 0);
	~GeoConvertor();

private:
	Ui::GeoConvertorClass ui;
};

#endif // GEOCONVERTOR_H
