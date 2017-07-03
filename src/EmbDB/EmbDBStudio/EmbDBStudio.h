#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_EmbDBStudio.h"

class EmbDBStudio : public QMainWindow
{
	Q_OBJECT

public:
	EmbDBStudio(QWidget *parent = Q_NULLPTR);

private:
	Ui::EmbDBStudioClass ui;
};
