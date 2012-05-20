/********************************************************************************
** Form generated from reading UI file 'mdtSerialPortConfigWidget.ui'
**
** Created: Sun May 20 14:41:59 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MDTSERIALPORTCONFIGWIDGET_H
#define UI_MDTSERIALPORTCONFIGWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mdtSerialPortConfigWidget
{
public:
    QGridLayout *gridLayout;
    QLabel *lbBaudRate;
    QComboBox *cbBaudRate;
    QGroupBox *gbDataBitCount;
    QVBoxLayout *verticalLayout;
    QRadioButton *rbDataBit5;
    QRadioButton *rbDataBit6;
    QRadioButton *rbDataBit7;
    QRadioButton *rbDataBit8;
    QGroupBox *gbStopBitCount;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *rbStopBit1;
    QRadioButton *rbStopBit2;
    QGroupBox *gbParity;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *rbParityNone;
    QRadioButton *rbParityEven;
    QRadioButton *rbParityOdd;
    QGroupBox *gbFlowCtl;
    QVBoxLayout *verticalLayout_4;
    QRadioButton *rbFlowCtlNone;
    QRadioButton *rbFlowCtlXonXoff;
    QHBoxLayout *horizontalLayout;
    QLabel *lbXonChar;
    QComboBox *cbXonChar;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lbXoffChar;
    QComboBox *cbXoffChar;
    QRadioButton *rbFlowCtlRtsCts;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *mdtSerialPortConfigWidget)
    {
        if (mdtSerialPortConfigWidget->objectName().isEmpty())
            mdtSerialPortConfigWidget->setObjectName(QString::fromUtf8("mdtSerialPortConfigWidget"));
        mdtSerialPortConfigWidget->resize(397, 198);
        gridLayout = new QGridLayout(mdtSerialPortConfigWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lbBaudRate = new QLabel(mdtSerialPortConfigWidget);
        lbBaudRate->setObjectName(QString::fromUtf8("lbBaudRate"));

        gridLayout->addWidget(lbBaudRate, 0, 0, 1, 1);

        cbBaudRate = new QComboBox(mdtSerialPortConfigWidget);
        cbBaudRate->setObjectName(QString::fromUtf8("cbBaudRate"));

        gridLayout->addWidget(cbBaudRate, 0, 1, 1, 2);

        gbDataBitCount = new QGroupBox(mdtSerialPortConfigWidget);
        gbDataBitCount->setObjectName(QString::fromUtf8("gbDataBitCount"));
        verticalLayout = new QVBoxLayout(gbDataBitCount);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        rbDataBit5 = new QRadioButton(gbDataBitCount);
        rbDataBit5->setObjectName(QString::fromUtf8("rbDataBit5"));

        verticalLayout->addWidget(rbDataBit5);

        rbDataBit6 = new QRadioButton(gbDataBitCount);
        rbDataBit6->setObjectName(QString::fromUtf8("rbDataBit6"));

        verticalLayout->addWidget(rbDataBit6);

        rbDataBit7 = new QRadioButton(gbDataBitCount);
        rbDataBit7->setObjectName(QString::fromUtf8("rbDataBit7"));

        verticalLayout->addWidget(rbDataBit7);

        rbDataBit8 = new QRadioButton(gbDataBitCount);
        rbDataBit8->setObjectName(QString::fromUtf8("rbDataBit8"));

        verticalLayout->addWidget(rbDataBit8);


        gridLayout->addWidget(gbDataBitCount, 1, 0, 2, 1);

        gbStopBitCount = new QGroupBox(mdtSerialPortConfigWidget);
        gbStopBitCount->setObjectName(QString::fromUtf8("gbStopBitCount"));
        verticalLayout_3 = new QVBoxLayout(gbStopBitCount);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        rbStopBit1 = new QRadioButton(gbStopBitCount);
        rbStopBit1->setObjectName(QString::fromUtf8("rbStopBit1"));

        verticalLayout_3->addWidget(rbStopBit1);

        rbStopBit2 = new QRadioButton(gbStopBitCount);
        rbStopBit2->setObjectName(QString::fromUtf8("rbStopBit2"));

        verticalLayout_3->addWidget(rbStopBit2);


        gridLayout->addWidget(gbStopBitCount, 1, 1, 1, 1);

        gbParity = new QGroupBox(mdtSerialPortConfigWidget);
        gbParity->setObjectName(QString::fromUtf8("gbParity"));
        verticalLayout_2 = new QVBoxLayout(gbParity);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        rbParityNone = new QRadioButton(gbParity);
        rbParityNone->setObjectName(QString::fromUtf8("rbParityNone"));

        verticalLayout_2->addWidget(rbParityNone);

        rbParityEven = new QRadioButton(gbParity);
        rbParityEven->setObjectName(QString::fromUtf8("rbParityEven"));

        verticalLayout_2->addWidget(rbParityEven);

        rbParityOdd = new QRadioButton(gbParity);
        rbParityOdd->setObjectName(QString::fromUtf8("rbParityOdd"));

        verticalLayout_2->addWidget(rbParityOdd);


        gridLayout->addWidget(gbParity, 1, 2, 1, 1);

        gbFlowCtl = new QGroupBox(mdtSerialPortConfigWidget);
        gbFlowCtl->setObjectName(QString::fromUtf8("gbFlowCtl"));
        verticalLayout_4 = new QVBoxLayout(gbFlowCtl);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        rbFlowCtlNone = new QRadioButton(gbFlowCtl);
        rbFlowCtlNone->setObjectName(QString::fromUtf8("rbFlowCtlNone"));

        verticalLayout_4->addWidget(rbFlowCtlNone);

        rbFlowCtlXonXoff = new QRadioButton(gbFlowCtl);
        rbFlowCtlXonXoff->setObjectName(QString::fromUtf8("rbFlowCtlXonXoff"));

        verticalLayout_4->addWidget(rbFlowCtlXonXoff);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lbXonChar = new QLabel(gbFlowCtl);
        lbXonChar->setObjectName(QString::fromUtf8("lbXonChar"));

        horizontalLayout->addWidget(lbXonChar);

        cbXonChar = new QComboBox(gbFlowCtl);
        cbXonChar->setObjectName(QString::fromUtf8("cbXonChar"));
        cbXonChar->setEditable(true);

        horizontalLayout->addWidget(cbXonChar);


        verticalLayout_4->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        lbXoffChar = new QLabel(gbFlowCtl);
        lbXoffChar->setObjectName(QString::fromUtf8("lbXoffChar"));

        horizontalLayout_2->addWidget(lbXoffChar);

        cbXoffChar = new QComboBox(gbFlowCtl);
        cbXoffChar->setObjectName(QString::fromUtf8("cbXoffChar"));
        cbXoffChar->setEditable(true);

        horizontalLayout_2->addWidget(cbXoffChar);


        verticalLayout_4->addLayout(horizontalLayout_2);

        rbFlowCtlRtsCts = new QRadioButton(gbFlowCtl);
        rbFlowCtlRtsCts->setObjectName(QString::fromUtf8("rbFlowCtlRtsCts"));

        verticalLayout_4->addWidget(rbFlowCtlRtsCts);


        gridLayout->addWidget(gbFlowCtl, 1, 3, 2, 1);

        verticalSpacer = new QSpacerItem(20, 43, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 2, 1, 1, 1);

#ifndef QT_NO_SHORTCUT
        lbBaudRate->setBuddy(cbBaudRate);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(cbBaudRate, rbDataBit5);
        QWidget::setTabOrder(rbDataBit5, rbDataBit6);
        QWidget::setTabOrder(rbDataBit6, rbDataBit7);
        QWidget::setTabOrder(rbDataBit7, rbDataBit8);
        QWidget::setTabOrder(rbDataBit8, rbStopBit1);
        QWidget::setTabOrder(rbStopBit1, rbStopBit2);
        QWidget::setTabOrder(rbStopBit2, rbParityNone);
        QWidget::setTabOrder(rbParityNone, rbParityEven);
        QWidget::setTabOrder(rbParityEven, rbParityOdd);
        QWidget::setTabOrder(rbParityOdd, rbFlowCtlNone);
        QWidget::setTabOrder(rbFlowCtlNone, rbFlowCtlXonXoff);
        QWidget::setTabOrder(rbFlowCtlXonXoff, rbFlowCtlRtsCts);

        retranslateUi(mdtSerialPortConfigWidget);

        QMetaObject::connectSlotsByName(mdtSerialPortConfigWidget);
    } // setupUi

    void retranslateUi(QWidget *mdtSerialPortConfigWidget)
    {
        mdtSerialPortConfigWidget->setWindowTitle(QApplication::translate("mdtSerialPortConfigWidget", "Form", 0, QApplication::UnicodeUTF8));
        lbBaudRate->setText(QApplication::translate("mdtSerialPortConfigWidget", "Baud rate", 0, QApplication::UnicodeUTF8));
        gbDataBitCount->setTitle(QApplication::translate("mdtSerialPortConfigWidget", "Data bits", 0, QApplication::UnicodeUTF8));
        rbDataBit5->setText(QApplication::translate("mdtSerialPortConfigWidget", "5", 0, QApplication::UnicodeUTF8));
        rbDataBit6->setText(QApplication::translate("mdtSerialPortConfigWidget", "6", 0, QApplication::UnicodeUTF8));
        rbDataBit7->setText(QApplication::translate("mdtSerialPortConfigWidget", "7", 0, QApplication::UnicodeUTF8));
        rbDataBit8->setText(QApplication::translate("mdtSerialPortConfigWidget", "8", 0, QApplication::UnicodeUTF8));
        gbStopBitCount->setTitle(QApplication::translate("mdtSerialPortConfigWidget", "Stop bits", 0, QApplication::UnicodeUTF8));
        rbStopBit1->setText(QApplication::translate("mdtSerialPortConfigWidget", "1", 0, QApplication::UnicodeUTF8));
        rbStopBit2->setText(QApplication::translate("mdtSerialPortConfigWidget", "2", 0, QApplication::UnicodeUTF8));
        gbParity->setTitle(QApplication::translate("mdtSerialPortConfigWidget", "Parity", 0, QApplication::UnicodeUTF8));
        rbParityNone->setText(QApplication::translate("mdtSerialPortConfigWidget", "None", 0, QApplication::UnicodeUTF8));
        rbParityEven->setText(QApplication::translate("mdtSerialPortConfigWidget", "Even", 0, QApplication::UnicodeUTF8));
        rbParityOdd->setText(QApplication::translate("mdtSerialPortConfigWidget", "Odd", 0, QApplication::UnicodeUTF8));
        gbFlowCtl->setTitle(QApplication::translate("mdtSerialPortConfigWidget", "Flow control", 0, QApplication::UnicodeUTF8));
        rbFlowCtlNone->setText(QApplication::translate("mdtSerialPortConfigWidget", "None", 0, QApplication::UnicodeUTF8));
        rbFlowCtlXonXoff->setText(QApplication::translate("mdtSerialPortConfigWidget", "Xon/Xoff", 0, QApplication::UnicodeUTF8));
        lbXonChar->setText(QApplication::translate("mdtSerialPortConfigWidget", "Xon char", 0, QApplication::UnicodeUTF8));
        lbXoffChar->setText(QApplication::translate("mdtSerialPortConfigWidget", "Xoff char", 0, QApplication::UnicodeUTF8));
        rbFlowCtlRtsCts->setText(QApplication::translate("mdtSerialPortConfigWidget", "RTS/CTS", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mdtSerialPortConfigWidget: public Ui_mdtSerialPortConfigWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MDTSERIALPORTCONFIGWIDGET_H
