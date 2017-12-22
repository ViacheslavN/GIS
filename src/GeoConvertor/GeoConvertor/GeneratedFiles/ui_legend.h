/********************************************************************************
** Form generated from reading UI file 'legend.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LEGEND_H
#define UI_LEGEND_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_legend
{
public:

    void setupUi(QWidget *legend)
    {
        if (legend->objectName().isEmpty())
            legend->setObjectName(QStringLiteral("legend"));
        legend->resize(510, 641);

        retranslateUi(legend);

        QMetaObject::connectSlotsByName(legend);
    } // setupUi

    void retranslateUi(QWidget *legend)
    {
        legend->setWindowTitle(QApplication::translate("legend", "legend", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class legend: public Ui_legend {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LEGEND_H
