#pragma once

#include <QDialog>
#include "ui_LIneSettingsDlg.h"

class LIneSettingsDlg : public QDialog
{
	Q_OBJECT

public:
	LIneSettingsDlg(QWidget *parent = Q_NULLPTR);
	~LIneSettingsDlg();

private:
	Ui::LIneSettingsDlg ui;
};
