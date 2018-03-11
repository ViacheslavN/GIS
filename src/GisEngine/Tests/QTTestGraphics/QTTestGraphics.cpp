#include "QTTestGraphics.h"

QTTestGraphics::QTTestGraphics(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_pView = new View;
	setCentralWidget(m_pView);

}
void QTTestGraphics::AddPart(bool addPart)
{
	m_pView->AddParts(addPart);
}
void QTTestGraphics::Clear()
{
	m_pView->Clear();
}

void QTTestGraphics::DrawLine(bool bLine)
{
	m_pView->Line(bLine);
}

void QTTestGraphics::AggOpenGL(bool bAgg)
{
	m_pView->AggOpenGL(bAgg);
}