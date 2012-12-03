/********************************************************************************
** Form generated from reading UI file 'mdtDeviceU3606AWidget.ui'
**
** Created: Mon Nov 26 19:38:00 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MDTDEVICEU3606AWIDGET_H
#define UI_MDTDEVICEU3606AWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mdtDeviceU3606AWidget
{
public:
    QGridLayout *gridLayout;
    QComboBox *cbPort;
    QPushButton *pbScan;
    QPushButton *pbEssais;
    QLabel *lbEssais;
    QLabel *lbManufacturerLabel;
    QLabel *lbManufacturer;
    QLabel *lbModelLabel;
    QLabel *lbModel;
    QLabel *lbSerialLabel;
    QLabel *lbSerial;
    QLabel *lbIfBoardFwLabel;
    QLabel *lbIfBoardFw;
    QLabel *lbMeasureBoardFwLabel;
    QLabel *lbMeasureBoardFw;
    QLabel *lbSrcBoardFwLabel;
    QLabel *lbSrcBoardFw;

    void setupUi(QWidget *mdtDeviceU3606AWidget)
    {
        if (mdtDeviceU3606AWidget->objectName().isEmpty())
            mdtDeviceU3606AWidget->setObjectName(QString::fromUtf8("mdtDeviceU3606AWidget"));
        mdtDeviceU3606AWidget->resize(350, 219);
        gridLayout = new QGridLayout(mdtDeviceU3606AWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        cbPort = new QComboBox(mdtDeviceU3606AWidget);
        cbPort->setObjectName(QString::fromUtf8("cbPort"));

        gridLayout->addWidget(cbPort, 0, 0, 1, 2);

        pbScan = new QPushButton(mdtDeviceU3606AWidget);
        pbScan->setObjectName(QString::fromUtf8("pbScan"));

        gridLayout->addWidget(pbScan, 0, 2, 1, 1);

        pbEssais = new QPushButton(mdtDeviceU3606AWidget);
        pbEssais->setObjectName(QString::fromUtf8("pbEssais"));

        gridLayout->addWidget(pbEssais, 1, 0, 1, 1);

        lbEssais = new QLabel(mdtDeviceU3606AWidget);
        lbEssais->setObjectName(QString::fromUtf8("lbEssais"));

        gridLayout->addWidget(lbEssais, 1, 1, 1, 1);

        lbManufacturerLabel = new QLabel(mdtDeviceU3606AWidget);
        lbManufacturerLabel->setObjectName(QString::fromUtf8("lbManufacturerLabel"));

        gridLayout->addWidget(lbManufacturerLabel, 2, 0, 1, 1);

        lbManufacturer = new QLabel(mdtDeviceU3606AWidget);
        lbManufacturer->setObjectName(QString::fromUtf8("lbManufacturer"));

        gridLayout->addWidget(lbManufacturer, 2, 1, 1, 1);

        lbModelLabel = new QLabel(mdtDeviceU3606AWidget);
        lbModelLabel->setObjectName(QString::fromUtf8("lbModelLabel"));

        gridLayout->addWidget(lbModelLabel, 3, 0, 1, 1);

        lbModel = new QLabel(mdtDeviceU3606AWidget);
        lbModel->setObjectName(QString::fromUtf8("lbModel"));

        gridLayout->addWidget(lbModel, 3, 1, 1, 1);

        lbSerialLabel = new QLabel(mdtDeviceU3606AWidget);
        lbSerialLabel->setObjectName(QString::fromUtf8("lbSerialLabel"));

        gridLayout->addWidget(lbSerialLabel, 4, 0, 1, 1);

        lbSerial = new QLabel(mdtDeviceU3606AWidget);
        lbSerial->setObjectName(QString::fromUtf8("lbSerial"));

        gridLayout->addWidget(lbSerial, 4, 1, 1, 1);

        lbIfBoardFwLabel = new QLabel(mdtDeviceU3606AWidget);
        lbIfBoardFwLabel->setObjectName(QString::fromUtf8("lbIfBoardFwLabel"));

        gridLayout->addWidget(lbIfBoardFwLabel, 5, 0, 1, 1);

        lbIfBoardFw = new QLabel(mdtDeviceU3606AWidget);
        lbIfBoardFw->setObjectName(QString::fromUtf8("lbIfBoardFw"));

        gridLayout->addWidget(lbIfBoardFw, 5, 1, 1, 1);

        lbMeasureBoardFwLabel = new QLabel(mdtDeviceU3606AWidget);
        lbMeasureBoardFwLabel->setObjectName(QString::fromUtf8("lbMeasureBoardFwLabel"));

        gridLayout->addWidget(lbMeasureBoardFwLabel, 6, 0, 1, 1);

        lbMeasureBoardFw = new QLabel(mdtDeviceU3606AWidget);
        lbMeasureBoardFw->setObjectName(QString::fromUtf8("lbMeasureBoardFw"));

        gridLayout->addWidget(lbMeasureBoardFw, 6, 1, 1, 1);

        lbSrcBoardFwLabel = new QLabel(mdtDeviceU3606AWidget);
        lbSrcBoardFwLabel->setObjectName(QString::fromUtf8("lbSrcBoardFwLabel"));

        gridLayout->addWidget(lbSrcBoardFwLabel, 7, 0, 1, 1);

        lbSrcBoardFw = new QLabel(mdtDeviceU3606AWidget);
        lbSrcBoardFw->setObjectName(QString::fromUtf8("lbSrcBoardFw"));

        gridLayout->addWidget(lbSrcBoardFw, 7, 1, 1, 1);


        retranslateUi(mdtDeviceU3606AWidget);

        QMetaObject::connectSlotsByName(mdtDeviceU3606AWidget);
    } // setupUi

    void retranslateUi(QWidget *mdtDeviceU3606AWidget)
    {
        mdtDeviceU3606AWidget->setWindowTitle(QApplication::translate("mdtDeviceU3606AWidget", "Form", 0, QApplication::UnicodeUTF8));
        pbScan->setText(QApplication::translate("mdtDeviceU3606AWidget", "Scan", 0, QApplication::UnicodeUTF8));
        pbEssais->setText(QApplication::translate("mdtDeviceU3606AWidget", "Essais", 0, QApplication::UnicodeUTF8));
        lbEssais->setText(QApplication::translate("mdtDeviceU3606AWidget", "Default text", 0, QApplication::UnicodeUTF8));
        lbManufacturerLabel->setText(QApplication::translate("mdtDeviceU3606AWidget", "Manufacturer", 0, QApplication::UnicodeUTF8));
        lbManufacturer->setText(QApplication::translate("mdtDeviceU3606AWidget", "Unknow", 0, QApplication::UnicodeUTF8));
        lbModelLabel->setText(QApplication::translate("mdtDeviceU3606AWidget", "Model", 0, QApplication::UnicodeUTF8));
        lbModel->setText(QApplication::translate("mdtDeviceU3606AWidget", "Unknow", 0, QApplication::UnicodeUTF8));
        lbSerialLabel->setText(QApplication::translate("mdtDeviceU3606AWidget", "Serial number", 0, QApplication::UnicodeUTF8));
        lbSerial->setText(QApplication::translate("mdtDeviceU3606AWidget", "Unknow", 0, QApplication::UnicodeUTF8));
        lbIfBoardFwLabel->setText(QApplication::translate("mdtDeviceU3606AWidget", "Interface firmware", 0, QApplication::UnicodeUTF8));
        lbIfBoardFw->setText(QApplication::translate("mdtDeviceU3606AWidget", "Unknow", 0, QApplication::UnicodeUTF8));
        lbMeasureBoardFwLabel->setText(QApplication::translate("mdtDeviceU3606AWidget", "Measure firmware", 0, QApplication::UnicodeUTF8));
        lbMeasureBoardFw->setText(QApplication::translate("mdtDeviceU3606AWidget", "Unknow", 0, QApplication::UnicodeUTF8));
        lbSrcBoardFwLabel->setText(QApplication::translate("mdtDeviceU3606AWidget", "Source firmware", 0, QApplication::UnicodeUTF8));
        lbSrcBoardFw->setText(QApplication::translate("mdtDeviceU3606AWidget", "Unknow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mdtDeviceU3606AWidget: public Ui_mdtDeviceU3606AWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MDTDEVICEU3606AWIDGET_H
