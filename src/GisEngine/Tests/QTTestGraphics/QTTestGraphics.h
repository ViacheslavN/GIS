#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QTTestGraphics.h"
#include "View.h"

class QTTestGraphics : public QMainWindow
{
	Q_OBJECT

public:
	QTTestGraphics(QWidget *parent = Q_NULLPTR);
private slots:
	void AddPart(bool addPart);
	void Clear();
	void DrawLine(bool bLine);
	void AggOpenGL(bool bAgg);
private:
	Ui::QTTestGraphicsClass ui;
	View *m_pView;
};
