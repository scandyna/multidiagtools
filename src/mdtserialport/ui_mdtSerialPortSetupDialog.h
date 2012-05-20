/********************************************************************************
** Form generated from reading UI file 'mdtSerialPortSetupDialog.ui'
**
** Created: Sun May 20 14:41:58 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MDTSERIALPORTSETUPDIALOG_H
#define UI_MDTSERIALPORTSETUPDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mdtSerialPortSetupDialog
{
public:
    QGridLayout *gridLayout;
    QPushButton *pbRescan;
    QComboBox *cbPort;
    QTabWidget *twOptions;
    QWidget *tab;
    QLabel *lbState;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *mdtSerialPortSetupDialog)
    {
        if (mdtSerialPortSetupDialog->objectName().isEmpty())
            mdtSerialPortSetupDialog->setObjectName(QString::fromUtf8("mdtSerialPortSetupDialog"));
        mdtSerialPortSetupDialog->resize(542, 248);
        gridLayout = new QGridLayout(mdtSerialPortSetupDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pbRescan = new QPushButton(mdtSerialPortSetupDialog);
        pbRescan->setObjectName(QString::fromUtf8("pbRescan"));

        gridLayout->addWidget(pbRescan, 0, 0, 1, 1);

        cbPort = new QComboBox(mdtSerialPortSetupDialog);
        cbPort->setObjectName(QString::fromUtf8("cbPort"));
        cbPort->setMinimumSize(QSize(150, 0));

        gridLayout->addWidget(cbPort, 0, 1, 1, 1);

        twOptions = new QTabWidget(mdtSerialPortSetupDialog);
        twOptions->setObjectName(QString::fromUtf8("twOptions"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        twOptions->addTab(tab, QString());

        gridLayout->addWidget(twOptions, 1, 0, 1, 3);

        lbState = new QLabel(mdtSerialPortSetupDialog);
        lbState->setObjectName(QString::fromUtf8("lbState"));

        gridLayout->addWidget(lbState, 2, 0, 1, 2);

        buttonBox = new QDialogButtonBox(mdtSerialPortSetupDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(false);

        gridLayout->addWidget(buttonBox, 2, 2, 1, 1);


        retranslateUi(mdtSerialPortSetupDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), mdtSerialPortSetupDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), mdtSerialPortSetupDialog, SLOT(reject()));

        twOptions->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(mdtSerialPortSetupDialog);
    } // setupUi

    void retranslateUi(QDialog *mdtSerialPortSetupDialog)
    {
        mdtSerialPortSetupDialog->setWindowTitle(QApplication::translate("mdtSerialPortSetupDialog", "Serial port setup", 0, QApplication::UnicodeUTF8));
        pbRescan->setText(QApplication::translate("mdtSerialPortSetupDialog", "Rescan", 0, QApplication::UnicodeUTF8));
        twOptions->setTabText(twOptions->indexOf(tab), QApplication::translate("mdtSerialPortSetupDialog", "Tab 1", 0, QApplication::UnicodeUTF8));
        lbState->setText(QApplication::translate("mdtSerialPortSetupDialog", "State", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mdtSerialPortSetupDialog: public Ui_mdtSerialPortSetupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MDTSERIALPORTSETUPDIALOG_H
