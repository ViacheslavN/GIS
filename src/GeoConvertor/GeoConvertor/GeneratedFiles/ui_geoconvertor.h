/********************************************************************************
** Form generated from reading UI file 'geoconvertor.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GEOCONVERTOR_H
#define UI_GEOCONVERTOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GeoConvertorClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GeoConvertorClass)
    {
        if (GeoConvertorClass->objectName().isEmpty())
            GeoConvertorClass->setObjectName(QStringLiteral("GeoConvertorClass"));
        GeoConvertorClass->resize(600, 400);
        menuBar = new QMenuBar(GeoConvertorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        GeoConvertorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(GeoConvertorClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        GeoConvertorClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(GeoConvertorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        GeoConvertorClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(GeoConvertorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        GeoConvertorClass->setStatusBar(statusBar);

        retranslateUi(GeoConvertorClass);

        QMetaObject::connectSlotsByName(GeoConvertorClass);
    } // setupUi

    void retranslateUi(QMainWindow *GeoConvertorClass)
    {
        GeoConvertorClass->setWindowTitle(QApplication::translate("GeoConvertorClass", "GeoConvertor", 0));
    } // retranslateUi

};

namespace Ui {
    class GeoConvertorClass: public Ui_GeoConvertorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GEOCONVERTOR_H
