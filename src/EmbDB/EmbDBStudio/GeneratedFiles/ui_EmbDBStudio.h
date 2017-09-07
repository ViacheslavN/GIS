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
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EmbDBStudio
{
public:
    QAction *actionOpen_database;
    QAction *actionNew_database;
    QWidget *centralWidget;
    QSplitter *splitter;
    QTreeWidget *treeWidget;
    QTabWidget *tabWidget;
    QWidget *tabShema;
    QWidget *tabData;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *EmbDBStudio)
    {
        if (EmbDBStudio->objectName().isEmpty())
            EmbDBStudio->setObjectName(QStringLiteral("EmbDBStudio"));
        EmbDBStudio->resize(990, 637);
        actionOpen_database = new QAction(EmbDBStudio);
        actionOpen_database->setObjectName(QStringLiteral("actionOpen_database"));
        actionNew_database = new QAction(EmbDBStudio);
        actionNew_database->setObjectName(QStringLiteral("actionNew_database"));
        centralWidget = new QWidget(EmbDBStudio);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setGeometry(QRect(20, 0, 951, 561));
        splitter->setOrientation(Qt::Horizontal);
        treeWidget = new QTreeWidget(splitter);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(treeWidget->sizePolicy().hasHeightForWidth());
        treeWidget->setSizePolicy(sizePolicy);
        splitter->addWidget(treeWidget);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy1);
        tabShema = new QWidget();
        tabShema->setObjectName(QStringLiteral("tabShema"));
        tabWidget->addTab(tabShema, QString());
        tabData = new QWidget();
        tabData->setObjectName(QStringLiteral("tabData"));
        tabWidget->addTab(tabData, QString());
        splitter->addWidget(tabWidget);
        EmbDBStudio->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(EmbDBStudio);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 990, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        EmbDBStudio->setMenuBar(menuBar);
        mainToolBar = new QToolBar(EmbDBStudio);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        EmbDBStudio->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(EmbDBStudio);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        EmbDBStudio->setStatusBar(statusBar);
        toolBar = new QToolBar(EmbDBStudio);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        EmbDBStudio->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen_database);
        menuFile->addAction(actionNew_database);

        retranslateUi(EmbDBStudio);
        QObject::connect(actionOpen_database, SIGNAL(triggered()), EmbDBStudio, SLOT(OpenDatabase()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(EmbDBStudio);
    } // setupUi

    void retranslateUi(QMainWindow *EmbDBStudio)
    {
        EmbDBStudio->setWindowTitle(QApplication::translate("EmbDBStudio", "EmbDBStudio", Q_NULLPTR));
        actionOpen_database->setText(QApplication::translate("EmbDBStudio", "Open database", Q_NULLPTR));
        actionNew_database->setText(QApplication::translate("EmbDBStudio", "New database", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabShema), QApplication::translate("EmbDBStudio", "Shema", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabData), QApplication::translate("EmbDBStudio", "Data", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("EmbDBStudio", "File", Q_NULLPTR));
        toolBar->setWindowTitle(QApplication::translate("EmbDBStudio", "toolBar", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EmbDBStudio: public Ui_EmbDBStudio {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EMBDBSTUDIO_H
