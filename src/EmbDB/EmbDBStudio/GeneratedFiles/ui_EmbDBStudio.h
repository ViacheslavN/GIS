/********************************************************************************
** Form generated from reading UI file 'EmbDBStudio.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EMBDBSTUDIO_H
#define UI_EMBDBSTUDIO_H

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

class Ui_EmbDBStudioClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *EmbDBStudioClass)
    {
        if (EmbDBStudioClass->objectName().isEmpty())
            EmbDBStudioClass->setObjectName(QStringLiteral("EmbDBStudioClass"));
        EmbDBStudioClass->resize(600, 400);
        menuBar = new QMenuBar(EmbDBStudioClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        EmbDBStudioClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(EmbDBStudioClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        EmbDBStudioClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(EmbDBStudioClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        EmbDBStudioClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(EmbDBStudioClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        EmbDBStudioClass->setStatusBar(statusBar);

        retranslateUi(EmbDBStudioClass);

        QMetaObject::connectSlotsByName(EmbDBStudioClass);
    } // setupUi

    void retranslateUi(QMainWindow *EmbDBStudioClass)
    {
        EmbDBStudioClass->setWindowTitle(QApplication::translate("EmbDBStudioClass", "EmbDBStudio", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EmbDBStudioClass: public Ui_EmbDBStudioClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EMBDBSTUDIO_H
