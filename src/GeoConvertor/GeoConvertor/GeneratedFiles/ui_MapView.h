/********************************************************************************
** Form generated from reading UI file 'MapView.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAPVIEW_H
#define UI_MAPVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MapView
{
public:

    void setupUi(QWidget *MapView)
    {
        if (MapView->objectName().isEmpty())
            MapView->setObjectName(QStringLiteral("MapView"));
        MapView->resize(400, 300);

        retranslateUi(MapView);

        QMetaObject::connectSlotsByName(MapView);
    } // setupUi

    void retranslateUi(QWidget *MapView)
    {
        MapView->setWindowTitle(QApplication::translate("MapView", "MapView", 0));
    } // retranslateUi

};

namespace Ui {
    class MapView: public Ui_MapView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAPVIEW_H
