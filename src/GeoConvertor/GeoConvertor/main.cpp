#include "geoconvertor.h"

#include "Application.h"
int main(int argc, char *argv[])
{
	CApplication a(argc, argv);
	GeoConvertor w;
	w.show();
	return a.exec();
}
