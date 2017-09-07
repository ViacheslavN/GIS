#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_EmbDBStudio.h"

class EmbDBStudio : public QMainWindow, public Ui::EmbDBStudio
{
	Q_OBJECT

public:
	EmbDBStudio(QWidget *parent = Q_NULLPTR);
public slots:
	void OpenDatabase();
private:
	//Ui::EmbDBStudioClass ui;
};
