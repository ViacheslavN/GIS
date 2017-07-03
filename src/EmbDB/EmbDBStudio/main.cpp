
#include "EmbDBStudio.h"
#include <Application.h>

int main(int argc, char *argv[])
{
	CApplication a(argc, argv);
	EmbDBStudio w;
	w.show();
	return a.exec();
}
