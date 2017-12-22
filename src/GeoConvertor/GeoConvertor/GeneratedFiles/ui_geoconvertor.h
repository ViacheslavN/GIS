/********************************************************************************
** Form generated from reading UI file 'geoconvertor.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
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
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>

QT_BEGIN_NAMESPACE

class Ui_GeoConvertorClass
{
public:
    QAction *actionNew;
    QAction *actionOpen_project;
    QAction *actionSave_project;
    QAction *actionSave_project_as;
    QSplitter *splitter;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *GeoConvertorClass)
    {
        if (GeoConvertorClass->objectName().isEmpty())
            GeoConvertorClass->setObjectName(QStringLiteral("GeoConvertorClass"));
        GeoConvertorClass->resize(953, 541);
        actionNew = new QAction(GeoConvertorClass);
        actionNew->setObjectName(QStringLiteral("actionNew"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/GeoConvertor/images/new.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew->setIcon(icon);
        actionOpen_project = new QAction(GeoConvertorClass);
        actionOpen_project->setObjectName(QStringLiteral("actionOpen_project"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/GeoConvertor/images/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen_project->setIcon(icon1);
        actionSave_project = new QAction(GeoConvertorClass);
        actionSave_project->setObjectName(QStringLiteral("actionSave_project"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/GeoConvertor/images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_project->setIcon(icon2);
        actionSave_project_as = new QAction(GeoConvertorClass);
        actionSave_project_as->setObjectName(QStringLiteral("actionSave_project_as"));
        actionSave_project_as->setIcon(icon2);
        splitter = new QSplitter(GeoConvertorClass);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        GeoConvertorClass->setCentralWidget(splitter);
        menuBar = new QMenuBar(GeoConvertorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 953, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        GeoConvertorClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(GeoConvertorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        GeoConvertorClass->setStatusBar(statusBar);
        toolBar = new QToolBar(GeoConvertorClass);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        GeoConvertorClass->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addAction(actionOpen_project);
        menuFile->addAction(actionSave_project);
        menuFile->addAction(actionSave_project_as);
        menuFile->addSeparator();
        toolBar->addAction(actionNew);
        toolBar->addAction(actionOpen_project);
        toolBar->addAction(actionSave_project);
        toolBar->addAction(actionSave_project_as);
        toolBar->addSeparator();

        retranslateUi(GeoConvertorClass);

        QMetaObject::connectSlotsByName(GeoConvertorClass);
    } // setupUi

    void retranslateUi(QMainWindow *GeoConvertorClass)
    {
        GeoConvertorClass->setWindowTitle(QApplication::translate("GeoConvertorClass", "GeoConvertor", Q_NULLPTR));
        actionNew->setText(QApplication::translate("GeoConvertorClass", "New", Q_NULLPTR));
        actionOpen_project->setText(QApplication::translate("GeoConvertorClass", "Open project", Q_NULLPTR));
        actionSave_project->setText(QApplication::translate("GeoConvertorClass", "Save project", Q_NULLPTR));
        actionSave_project_as->setText(QApplication::translate("GeoConvertorClass", "Save project as", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("GeoConvertorClass", "File", Q_NULLPTR));
        menuHelp->setTitle(QApplication::translate("GeoConvertorClass", "Help", Q_NULLPTR));
        toolBar->setWindowTitle(QApplication::translate("GeoConvertorClass", "toolBar", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GeoConvertorClass: public Ui_GeoConvertorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GEOCONVERTOR_H
