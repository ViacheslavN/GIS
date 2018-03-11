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
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_legend
{
public:
    QTreeWidget *treeWidget;

    void setupUi(QWidget *legend)
    {
        if (legend->objectName().isEmpty())
            legend->setObjectName(QStringLiteral("legend"));
        legend->resize(400, 300);
        treeWidget = new QTreeWidget(legend);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setGeometry(QRect(0, 0, 401, 301));

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
