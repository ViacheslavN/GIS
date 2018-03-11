#include "QTTestGraphics.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTTestGraphics w;
	w.show();
	return a.exec();
}
