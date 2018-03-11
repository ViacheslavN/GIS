/********************************************************************************
** Form generated from reading UI file 'LIneSettingsDlg.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LINESETTINGSDLG_H
#define UI_LINESETTINGSDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_LIneSettingsDlg
{
public:
    QLabel *label;
    QComboBox *CapsStyleBox;
    QLabel *label_2;
    QLabel *label_3;
    QComboBox *comboBoxJoinStyle;
    QComboBox *comboBox_2;
    QLabel *label_4;
    QCheckBox *checkBoxDrawPoints;
    QLabel *label_5;
    QPushButton *colorLine;
    QLabel *label_6;
    QSpinBox *spinBoxLineWeght;

    void setupUi(QDialog *LIneSettingsDlg)
    {
        if (LIneSettingsDlg->objectName().isEmpty())
            LIneSettingsDlg->setObjectName(QStringLiteral("LIneSettingsDlg"));
        LIneSettingsDlg->resize(601, 395);
        label = new QLabel(LIneSettingsDlg);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 40, 141, 31));
        CapsStyleBox = new QComboBox(LIneSettingsDlg);
        QIcon icon;
        icon.addFile(QStringLiteral("images/cap_butt.png"), QSize(), QIcon::Normal, QIcon::On);
        CapsStyleBox->addItem(icon, QString());
        QIcon icon1;
        icon1.addFile(QStringLiteral("images/cap_butt.png"), QSize(), QIcon::Normal, QIcon::Off);
        CapsStyleBox->addItem(icon1, QString());
        CapsStyleBox->addItem(QString());
        CapsStyleBox->setObjectName(QStringLiteral("CapsStyleBox"));
        CapsStyleBox->setGeometry(QRect(110, 50, 151, 31));
        label_2 = new QLabel(LIneSettingsDlg);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 90, 91, 41));
        label_3 = new QLabel(LIneSettingsDlg);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 150, 81, 31));
        comboBoxJoinStyle = new QComboBox(LIneSettingsDlg);
        comboBoxJoinStyle->setObjectName(QStringLiteral("comboBoxJoinStyle"));
        comboBoxJoinStyle->setGeometry(QRect(110, 100, 151, 22));
        comboBox_2 = new QComboBox(LIneSettingsDlg);
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));
        comboBox_2->setGeometry(QRect(110, 150, 151, 22));
        label_4 = new QLabel(LIneSettingsDlg);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(330, 50, 111, 21));
        checkBoxDrawPoints = new QCheckBox(LIneSettingsDlg);
        checkBoxDrawPoints->setObjectName(QStringLiteral("checkBoxDrawPoints"));
        checkBoxDrawPoints->setGeometry(QRect(440, 50, 70, 17));
        label_5 = new QLabel(LIneSettingsDlg);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(330, 90, 47, 13));
        colorLine = new QPushButton(LIneSettingsDlg);
        colorLine->setObjectName(QStringLiteral("colorLine"));
        colorLine->setGeometry(QRect(430, 80, 75, 23));
        label_6 = new QLabel(LIneSettingsDlg);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(330, 160, 71, 16));
        spinBoxLineWeght = new QSpinBox(LIneSettingsDlg);
        spinBoxLineWeght->setObjectName(QStringLiteral("spinBoxLineWeght"));
        spinBoxLineWeght->setGeometry(QRect(430, 160, 81, 22));

        retranslateUi(LIneSettingsDlg);

        QMetaObject::connectSlotsByName(LIneSettingsDlg);
    } // setupUi

    void retranslateUi(QDialog *LIneSettingsDlg)
    {
        LIneSettingsDlg->setWindowTitle(QApplication::translate("LIneSettingsDlg", "LIneSettingsDlg", Q_NULLPTR));
        label->setText(QApplication::translate("LIneSettingsDlg", "Caps style", Q_NULLPTR));
        CapsStyleBox->setItemText(0, QApplication::translate("LIneSettingsDlg", "butt", Q_NULLPTR));
        CapsStyleBox->setItemText(1, QApplication::translate("LIneSettingsDlg", "round", Q_NULLPTR));
        CapsStyleBox->setItemText(2, QApplication::translate("LIneSettingsDlg", "square", Q_NULLPTR));

        label_2->setText(QApplication::translate("LIneSettingsDlg", "Join style", Q_NULLPTR));
        label_3->setText(QApplication::translate("LIneSettingsDlg", "Inner Join Style", Q_NULLPTR));
        comboBoxJoinStyle->clear();
        comboBoxJoinStyle->insertItems(0, QStringList()
         << QApplication::translate("LIneSettingsDlg", "Round", Q_NULLPTR)
         << QApplication::translate("LIneSettingsDlg", "Bevel", Q_NULLPTR)
         << QApplication::translate("LIneSettingsDlg", "Mitter", Q_NULLPTR)
        );
        label_4->setText(QApplication::translate("LIneSettingsDlg", "Draw points", Q_NULLPTR));
        checkBoxDrawPoints->setText(QApplication::translate("LIneSettingsDlg", "CheckBox", Q_NULLPTR));
        label_5->setText(QApplication::translate("LIneSettingsDlg", "Color", Q_NULLPTR));
        colorLine->setText(QApplication::translate("LIneSettingsDlg", "Color line", Q_NULLPTR));
        label_6->setText(QApplication::translate("LIneSettingsDlg", "Line weight", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class LIneSettingsDlg: public Ui_LIneSettingsDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LINESETTINGSDLG_H
