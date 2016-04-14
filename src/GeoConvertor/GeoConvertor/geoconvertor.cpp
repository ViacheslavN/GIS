#include "geoconvertor.h"
#include "MapView.h"
#include "legend.h"
#include <QtWidgets/QTextEdit>
GeoConvertor::GeoConvertor(QWidget *parent)
	: QMainWindow(parent)
{
	setupUi(this);

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	m_pLegendView = new CLegendView();
	m_pMapView = new CMapView();

	//QTextEdit *textEdit1 = new QTextEdit;
	//QTextEdit *textEdit2 = new QTextEdit;

 

	 QTabWidget* tabWidget = new QTabWidget;
	 tabWidget->addTab(m_pLegendView, tr("Legend"));

	//m_pMapView->setSizePolicy(sizePolicy);
	//m_pLegendView->setSizePolicy(sizePolicy);
	this->splitter->setSizePolicy(sizePolicy);
	tabWidget->setSizePolicy(sizePolicy);
	this->splitter->addWidget(tabWidget);
	this->splitter->addWidget(m_pMapView);
}

GeoConvertor::~GeoConvertor()
{

}
