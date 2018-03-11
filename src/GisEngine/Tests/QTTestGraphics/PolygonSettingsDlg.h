#pragma once

#include <QDialog>
#include "ui_PolygonSettingsDlg.h"

class PolygonSettingsDlg : public QDialog
{
	Q_OBJECT

public:
	PolygonSettingsDlg(QWidget *parent = Q_NULLPTR);
	~PolygonSettingsDlg();

private:
	Ui::PolygonSettingsDlg ui;
};
