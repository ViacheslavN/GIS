/********************************************************************************
** Form generated from reading UI file 'QTTestGraphics.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTTESTGRAPHICS_H
#define UI_QTTESTGRAPHICS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QTTestGraphicsClass
{
public:
    QAction *AGG_OpenGL;
    QAction *actionRedraw;
    QAction *actionLine_Polygon;
    QAction *actionLine;
    QAction *actionPolygon;
    QAction *actionDraw;
    QAction *actionClear;
    QAction *actionAdd_Part;
    QAction *actionSave;
    QAction *actionOPen;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menufile;
    QMenu *menuFile;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *QTTestGraphicsClass)
    {
        if (QTTestGraphicsClass->objectName().isEmpty())
            QTTestGraphicsClass->setObjectName(QStringLiteral("QTTestGraphicsClass"));
        QTTestGraphicsClass->resize(968, 728);
        AGG_OpenGL = new QAction(QTTestGraphicsClass);
        AGG_OpenGL->setObjectName(QStringLiteral("AGG_OpenGL"));
        AGG_OpenGL->setCheckable(true);
        QIcon icon;
        icon.addFile(QStringLiteral(":/QTTestGraphics/images/agg.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QStringLiteral(":/QTTestGraphics/images/opengl.png"), QSize(), QIcon::Normal, QIcon::On);
        icon.addFile(QStringLiteral(":/QTTestGraphics/images/agg.png"), QSize(), QIcon::Disabled, QIcon::Off);
        icon.addFile(QStringLiteral(":/QTTestGraphics/images/opengl.png"), QSize(), QIcon::Disabled, QIcon::On);
        icon.addFile(QStringLiteral(":/QTTestGraphics/images/opengl.png"), QSize(), QIcon::Active, QIcon::Off);
        icon.addFile(QStringLiteral(":/QTTestGraphics/images/agg.png"), QSize(), QIcon::Active, QIcon::On);
        icon.addFile(QStringLiteral(":/QTTestGraphics/images/agg.png"), QSize(), QIcon::Selected, QIcon::Off);
        icon.addFile(QStringLiteral(":/QTTestGraphics/images/opengl.png"), QSize(), QIcon::Selected, QIcon::On);
        AGG_OpenGL->setIcon(icon);
        actionRedraw = new QAction(QTTestGraphicsClass);
        actionRedraw->setObjectName(QStringLiteral("actionRedraw"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/QTTestGraphics/images/redraw.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRedraw->setIcon(icon1);
        actionLine_Polygon = new QAction(QTTestGraphicsClass);
        actionLine_Polygon->setObjectName(QStringLiteral("actionLine_Polygon"));
        actionLine_Polygon->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/QTTestGraphics/images/polygon.png"), QSize(), QIcon::Selected, QIcon::Off);
        icon2.addFile(QStringLiteral(":/QTTestGraphics/images/line.png"), QSize(), QIcon::Selected, QIcon::On);
        actionLine_Polygon->setIcon(icon2);
        actionLine = new QAction(QTTestGraphicsClass);
        actionLine->setObjectName(QStringLiteral("actionLine"));
        actionPolygon = new QAction(QTTestGraphicsClass);
        actionPolygon->setObjectName(QStringLiteral("actionPolygon"));
        actionDraw = new QAction(QTTestGraphicsClass);
        actionDraw->setObjectName(QStringLiteral("actionDraw"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/QTTestGraphics/images/Draw.png"), QSize(), QIcon::Active, QIcon::On);
        actionDraw->setIcon(icon3);
        actionClear = new QAction(QTTestGraphicsClass);
        actionClear->setObjectName(QStringLiteral("actionClear"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/QTTestGraphics/images/Clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClear->setIcon(icon4);
        actionAdd_Part = new QAction(QTTestGraphicsClass);
        actionAdd_Part->setObjectName(QStringLiteral("actionAdd_Part"));
        actionAdd_Part->setCheckable(true);
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/QTTestGraphics/images/addPart.png"), QSize(), QIcon::Active, QIcon::On);
        actionAdd_Part->setIcon(icon5);
        actionSave = new QAction(QTTestGraphicsClass);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionOPen = new QAction(QTTestGraphicsClass);
        actionOPen->setObjectName(QStringLiteral("actionOPen"));
        centralWidget = new QWidget(QTTestGraphicsClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QTTestGraphicsClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QTTestGraphicsClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 968, 21));
        menufile = new QMenu(menuBar);
        menufile->setObjectName(QStringLiteral("menufile"));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        QTTestGraphicsClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(QTTestGraphicsClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QTTestGraphicsClass->setStatusBar(statusBar);
        toolBar = new QToolBar(QTTestGraphicsClass);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        QTTestGraphicsClass->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menufile->menuAction());
        menufile->addAction(actionLine);
        menufile->addAction(actionPolygon);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionOPen);
        toolBar->addAction(AGG_OpenGL);
        toolBar->addAction(actionLine_Polygon);
        toolBar->addAction(actionDraw);
        toolBar->addAction(actionClear);
        toolBar->addAction(actionAdd_Part);
        toolBar->addAction(actionRedraw);
        toolBar->addSeparator();

        retranslateUi(QTTestGraphicsClass);
        QObject::connect(actionAdd_Part, SIGNAL(triggered(bool)), QTTestGraphicsClass, SLOT(AddPart(bool)));
        QObject::connect(actionClear, SIGNAL(triggered()), QTTestGraphicsClass, SLOT(Clear()));
        QObject::connect(actionLine_Polygon, SIGNAL(toggled(bool)), QTTestGraphicsClass, SLOT(DrawLine(bool)));
        QObject::connect(AGG_OpenGL, SIGNAL(toggled(bool)), QTTestGraphicsClass, SLOT(AggOpenGL(bool)));

        QMetaObject::connectSlotsByName(QTTestGraphicsClass);
    } // setupUi

    void retranslateUi(QMainWindow *QTTestGraphicsClass)
    {
        QTTestGraphicsClass->setWindowTitle(QApplication::translate("QTTestGraphicsClass", "QTTestGraphics", Q_NULLPTR));
        AGG_OpenGL->setText(QApplication::translate("QTTestGraphicsClass", "AGG/OpenGL", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        AGG_OpenGL->setToolTip(QApplication::translate("QTTestGraphicsClass", "AGG/OpenGL", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionRedraw->setText(QApplication::translate("QTTestGraphicsClass", "Redraw", Q_NULLPTR));
        actionLine_Polygon->setText(QApplication::translate("QTTestGraphicsClass", "Line/Polygon", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionLine_Polygon->setToolTip(QApplication::translate("QTTestGraphicsClass", "Line/Polygon", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionLine->setText(QApplication::translate("QTTestGraphicsClass", "Line", Q_NULLPTR));
        actionPolygon->setText(QApplication::translate("QTTestGraphicsClass", "Polygon", Q_NULLPTR));
        actionDraw->setText(QApplication::translate("QTTestGraphicsClass", "Draw", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionDraw->setToolTip(QApplication::translate("QTTestGraphicsClass", "Draw", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionClear->setText(QApplication::translate("QTTestGraphicsClass", "Clear", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionClear->setToolTip(QApplication::translate("QTTestGraphicsClass", "Clear", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionAdd_Part->setText(QApplication::translate("QTTestGraphicsClass", "Add Part", Q_NULLPTR));
        actionSave->setText(QApplication::translate("QTTestGraphicsClass", "Save", Q_NULLPTR));
        actionOPen->setText(QApplication::translate("QTTestGraphicsClass", "Open", Q_NULLPTR));
        menufile->setTitle(QApplication::translate("QTTestGraphicsClass", "Settings", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("QTTestGraphicsClass", "File", Q_NULLPTR));
        toolBar->setWindowTitle(QApplication::translate("QTTestGraphicsClass", "toolBar", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QTTestGraphicsClass: public Ui_QTTestGraphicsClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTTESTGRAPHICS_H
