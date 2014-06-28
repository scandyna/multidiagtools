/********************************************************************************
** Form generated from reading UI file 'mdtTtTestModelItemDialog.ui'
**
** Created: Fri Jun 27 03:44:53 2014
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MDTTTTESTMODELITEMDIALOG_H
#define UI_MDTTTTESTMODELITEMDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mdtTtTestModelItemDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tbTestModelItem;
    QFormLayout *formLayout;
    QLabel *lbSequenceNumber;
    QSpinBox *sbSequenceNumber;
    QLabel *lbExpectedValue;
    QDoubleSpinBox *sbExpectedValue;
    QLabel *lbLimitValueMin;
    QDoubleSpinBox *sbLimitValueMin;
    QLabel *lbLimitValueMax;
    QDoubleSpinBox *sbLimitValueMax;
    QLabel *lbFailValueMin;
    QDoubleSpinBox *sbFailValueMin;
    QLabel *lbFailValueMax;
    QDoubleSpinBox *sbFailValueMax;
    QWidget *tbTestLink;
    QWidget *tbTestNodeUnitSetup;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *mdtTtTestModelItemDialog)
    {
        if (mdtTtTestModelItemDialog->objectName().isEmpty())
            mdtTtTestModelItemDialog->setObjectName(QString::fromUtf8("mdtTtTestModelItemDialog"));
        mdtTtTestModelItemDialog->resize(739, 319);
        verticalLayout = new QVBoxLayout(mdtTtTestModelItemDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(mdtTtTestModelItemDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tbTestModelItem = new QWidget();
        tbTestModelItem->setObjectName(QString::fromUtf8("tbTestModelItem"));
        formLayout = new QFormLayout(tbTestModelItem);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        lbSequenceNumber = new QLabel(tbTestModelItem);
        lbSequenceNumber->setObjectName(QString::fromUtf8("lbSequenceNumber"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lbSequenceNumber);

        sbSequenceNumber = new QSpinBox(tbTestModelItem);
        sbSequenceNumber->setObjectName(QString::fromUtf8("sbSequenceNumber"));
        sbSequenceNumber->setMaximum(200);

        formLayout->setWidget(0, QFormLayout::FieldRole, sbSequenceNumber);

        lbExpectedValue = new QLabel(tbTestModelItem);
        lbExpectedValue->setObjectName(QString::fromUtf8("lbExpectedValue"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lbExpectedValue);

        sbExpectedValue = new QDoubleSpinBox(tbTestModelItem);
        sbExpectedValue->setObjectName(QString::fromUtf8("sbExpectedValue"));

        formLayout->setWidget(1, QFormLayout::FieldRole, sbExpectedValue);

        lbLimitValueMin = new QLabel(tbTestModelItem);
        lbLimitValueMin->setObjectName(QString::fromUtf8("lbLimitValueMin"));

        formLayout->setWidget(2, QFormLayout::LabelRole, lbLimitValueMin);

        sbLimitValueMin = new QDoubleSpinBox(tbTestModelItem);
        sbLimitValueMin->setObjectName(QString::fromUtf8("sbLimitValueMin"));

        formLayout->setWidget(2, QFormLayout::FieldRole, sbLimitValueMin);

        lbLimitValueMax = new QLabel(tbTestModelItem);
        lbLimitValueMax->setObjectName(QString::fromUtf8("lbLimitValueMax"));

        formLayout->setWidget(3, QFormLayout::LabelRole, lbLimitValueMax);

        sbLimitValueMax = new QDoubleSpinBox(tbTestModelItem);
        sbLimitValueMax->setObjectName(QString::fromUtf8("sbLimitValueMax"));

        formLayout->setWidget(3, QFormLayout::FieldRole, sbLimitValueMax);

        lbFailValueMin = new QLabel(tbTestModelItem);
        lbFailValueMin->setObjectName(QString::fromUtf8("lbFailValueMin"));

        formLayout->setWidget(4, QFormLayout::LabelRole, lbFailValueMin);

        sbFailValueMin = new QDoubleSpinBox(tbTestModelItem);
        sbFailValueMin->setObjectName(QString::fromUtf8("sbFailValueMin"));

        formLayout->setWidget(4, QFormLayout::FieldRole, sbFailValueMin);

        lbFailValueMax = new QLabel(tbTestModelItem);
        lbFailValueMax->setObjectName(QString::fromUtf8("lbFailValueMax"));

        formLayout->setWidget(5, QFormLayout::LabelRole, lbFailValueMax);

        sbFailValueMax = new QDoubleSpinBox(tbTestModelItem);
        sbFailValueMax->setObjectName(QString::fromUtf8("sbFailValueMax"));

        formLayout->setWidget(5, QFormLayout::FieldRole, sbFailValueMax);

        tabWidget->addTab(tbTestModelItem, QString());
        tbTestLink = new QWidget();
        tbTestLink->setObjectName(QString::fromUtf8("tbTestLink"));
        tabWidget->addTab(tbTestLink, QString());
        tbTestNodeUnitSetup = new QWidget();
        tbTestNodeUnitSetup->setObjectName(QString::fromUtf8("tbTestNodeUnitSetup"));
        tabWidget->addTab(tbTestNodeUnitSetup, QString());

        verticalLayout->addWidget(tabWidget);

        buttonBox = new QDialogButtonBox(mdtTtTestModelItemDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(mdtTtTestModelItemDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), mdtTtTestModelItemDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), mdtTtTestModelItemDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(mdtTtTestModelItemDialog);
    } // setupUi

    void retranslateUi(QDialog *mdtTtTestModelItemDialog)
    {
        mdtTtTestModelItemDialog->setWindowTitle(QApplication::translate("mdtTtTestModelItemDialog", "Test model item edition", 0, QApplication::UnicodeUTF8));
        lbSequenceNumber->setText(QApplication::translate("mdtTtTestModelItemDialog", "Sequence #:", 0, QApplication::UnicodeUTF8));
        lbExpectedValue->setText(QApplication::translate("mdtTtTestModelItemDialog", "Expected value:", 0, QApplication::UnicodeUTF8));
        lbLimitValueMin->setText(QApplication::translate("mdtTtTestModelItemDialog", "Limit value min:", 0, QApplication::UnicodeUTF8));
        lbLimitValueMax->setText(QApplication::translate("mdtTtTestModelItemDialog", "Limit value max:", 0, QApplication::UnicodeUTF8));
        lbFailValueMin->setText(QApplication::translate("mdtTtTestModelItemDialog", "Fail value min:", 0, QApplication::UnicodeUTF8));
        lbFailValueMax->setText(QApplication::translate("mdtTtTestModelItemDialog", "Fail value max:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tbTestModelItem), QApplication::translate("mdtTtTestModelItemDialog", "Main data", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tbTestLink), QApplication::translate("mdtTtTestModelItemDialog", "Test links", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tbTestNodeUnitSetup), QApplication::translate("mdtTtTestModelItemDialog", "Test node unit setup", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mdtTtTestModelItemDialog: public Ui_mdtTtTestModelItemDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MDTTTTESTMODELITEMDIALOG_H
