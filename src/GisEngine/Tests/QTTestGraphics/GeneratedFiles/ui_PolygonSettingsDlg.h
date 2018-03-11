/********************************************************************************
** Form generated from reading UI file 'PolygonSettingsDlg.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POLYGONSETTINGSDLG_H
#define UI_POLYGONSETTINGSDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_PolygonSettingsDlg
{
public:
    QPushButton *ColorPolygon;
    QPushButton *pushButtonColorLine;
    QLabel *label;
    QCheckBox *checkBoxDrawPoint;
    QLabel *label_2;
    QSpinBox *spinBoxLineWIght;

    void setupUi(QDialog *PolygonSettingsDlg)
    {
        if (PolygonSettingsDlg->objectName().isEmpty())
            PolygonSettingsDlg->setObjectName(QStringLiteral("PolygonSettingsDlg"));
        PolygonSettingsDlg->resize(400, 300);
        ColorPolygon = new QPushButton(PolygonSettingsDlg);
        ColorPolygon->setObjectName(QStringLiteral("ColorPolygon"));
        ColorPolygon->setGeometry(QRect(60, 40, 91, 23));
        pushButtonColorLine = new QPushButton(PolygonSettingsDlg);
        pushButtonColorLine->setObjectName(QStringLiteral("pushButtonColorLine"));
        pushButtonColorLine->setGeometry(QRect(60, 90, 91, 23));
        label = new QLabel(PolygonSettingsDlg);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(210, 90, 71, 21));
        checkBoxDrawPoint = new QCheckBox(PolygonSettingsDlg);
        checkBoxDrawPoint->setObjectName(QStringLiteral("checkBoxDrawPoint"));
        checkBoxDrawPoint->setGeometry(QRect(280, 90, 70, 17));
        label_2 = new QLabel(PolygonSettingsDlg);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(60, 160, 71, 21));
        spinBoxLineWIght = new QSpinBox(PolygonSettingsDlg);
        spinBoxLineWIght->setObjectName(QStringLiteral("spinBoxLineWIght"));
        spinBoxLineWIght->setGeometry(QRect(210, 160, 91, 22));

        retranslateUi(PolygonSettingsDlg);

        QMetaObject::connectSlotsByName(PolygonSettingsDlg);
    } // setupUi

    void retranslateUi(QDialog *PolygonSettingsDlg)
    {
        PolygonSettingsDlg->setWindowTitle(QApplication::translate("PolygonSettingsDlg", "PolygonSettingsDlg", Q_NULLPTR));
        ColorPolygon->setText(QApplication::translate("PolygonSettingsDlg", "ColorPolygon", Q_NULLPTR));
        pushButtonColorLine->setText(QApplication::translate("PolygonSettingsDlg", "ColorLine", Q_NULLPTR));
        label->setText(QApplication::translate("PolygonSettingsDlg", "Draw Points", Q_NULLPTR));
        checkBoxDrawPoint->setText(QString());
        label_2->setText(QApplication::translate("PolygonSettingsDlg", "Line wight", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class PolygonSettingsDlg: public Ui_PolygonSettingsDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POLYGONSETTINGSDLG_H
