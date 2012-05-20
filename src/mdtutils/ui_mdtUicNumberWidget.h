/********************************************************************************
** Form generated from reading UI file 'mdtUicNumberWidget.ui'
**
** Created: Sun May 20 14:42:21 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MDTUICNUMBERWIDGET_H
#define UI_MDTUICNUMBERWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mdtUicNumberWidget
{
public:
    QGridLayout *gridLayout_2;
    QLabel *lbUicNumber;
    QLineEdit *leUicNumber;
    QPushButton *pbCheckUicNumber;
    QPushButton *pbDetails;
    QWidget *wDetails;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *lbUsageLabel;
    QLabel *lbUsage;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lbCountryLabel;
    QLabel *lbCountry;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QLabel *lbTypeLabel;
    QLabel *lbType;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_4;
    QLabel *lbSpeedAndHeatLabel;
    QLabel *lbSpeedAndHeat;
    QSpacerItem *horizontalSpacer_4;

    void setupUi(QWidget *mdtUicNumberWidget)
    {
        if (mdtUicNumberWidget->objectName().isEmpty())
            mdtUicNumberWidget->setObjectName(QString::fromUtf8("mdtUicNumberWidget"));
        mdtUicNumberWidget->resize(330, 170);
        gridLayout_2 = new QGridLayout(mdtUicNumberWidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setSizeConstraint(QLayout::SetFixedSize);
        lbUicNumber = new QLabel(mdtUicNumberWidget);
        lbUicNumber->setObjectName(QString::fromUtf8("lbUicNumber"));

        gridLayout_2->addWidget(lbUicNumber, 0, 0, 1, 1);

        leUicNumber = new QLineEdit(mdtUicNumberWidget);
        leUicNumber->setObjectName(QString::fromUtf8("leUicNumber"));
        leUicNumber->setMinimumSize(QSize(200, 0));
        leUicNumber->setMaxLength(20);

        gridLayout_2->addWidget(leUicNumber, 0, 1, 1, 1);

        pbCheckUicNumber = new QPushButton(mdtUicNumberWidget);
        pbCheckUicNumber->setObjectName(QString::fromUtf8("pbCheckUicNumber"));

        gridLayout_2->addWidget(pbCheckUicNumber, 0, 2, 1, 1);

        pbDetails = new QPushButton(mdtUicNumberWidget);
        pbDetails->setObjectName(QString::fromUtf8("pbDetails"));
        pbDetails->setCheckable(true);

        gridLayout_2->addWidget(pbDetails, 1, 0, 1, 1);

        wDetails = new QWidget(mdtUicNumberWidget);
        wDetails->setObjectName(QString::fromUtf8("wDetails"));
        wDetails->setEnabled(true);
        verticalLayout = new QVBoxLayout(wDetails);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        lbUsageLabel = new QLabel(wDetails);
        lbUsageLabel->setObjectName(QString::fromUtf8("lbUsageLabel"));

        horizontalLayout_3->addWidget(lbUsageLabel);

        lbUsage = new QLabel(wDetails);
        lbUsage->setObjectName(QString::fromUtf8("lbUsage"));

        horizontalLayout_3->addWidget(lbUsage);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        lbCountryLabel = new QLabel(wDetails);
        lbCountryLabel->setObjectName(QString::fromUtf8("lbCountryLabel"));

        horizontalLayout_2->addWidget(lbCountryLabel);

        lbCountry = new QLabel(wDetails);
        lbCountry->setObjectName(QString::fromUtf8("lbCountry"));

        horizontalLayout_2->addWidget(lbCountry);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lbTypeLabel = new QLabel(wDetails);
        lbTypeLabel->setObjectName(QString::fromUtf8("lbTypeLabel"));

        horizontalLayout->addWidget(lbTypeLabel);

        lbType = new QLabel(wDetails);
        lbType->setObjectName(QString::fromUtf8("lbType"));

        horizontalLayout->addWidget(lbType);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        lbSpeedAndHeatLabel = new QLabel(wDetails);
        lbSpeedAndHeatLabel->setObjectName(QString::fromUtf8("lbSpeedAndHeatLabel"));

        horizontalLayout_4->addWidget(lbSpeedAndHeatLabel);

        lbSpeedAndHeat = new QLabel(wDetails);
        lbSpeedAndHeat->setObjectName(QString::fromUtf8("lbSpeedAndHeat"));

        horizontalLayout_4->addWidget(lbSpeedAndHeat);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_4);


        gridLayout_2->addWidget(wDetails, 2, 0, 1, 3);

#ifndef QT_NO_SHORTCUT
        lbUicNumber->setBuddy(leUicNumber);
#endif // QT_NO_SHORTCUT

        retranslateUi(mdtUicNumberWidget);

        QMetaObject::connectSlotsByName(mdtUicNumberWidget);
    } // setupUi

    void retranslateUi(QWidget *mdtUicNumberWidget)
    {
        mdtUicNumberWidget->setWindowTitle(QApplication::translate("mdtUicNumberWidget", "Form", 0, QApplication::UnicodeUTF8));
        lbUicNumber->setText(QApplication::translate("mdtUicNumberWidget", "UIC number", 0, QApplication::UnicodeUTF8));
        pbCheckUicNumber->setText(QApplication::translate("mdtUicNumberWidget", "Ok", 0, QApplication::UnicodeUTF8));
        pbDetails->setText(QApplication::translate("mdtUicNumberWidget", "Details", 0, QApplication::UnicodeUTF8));
        lbUsageLabel->setText(QApplication::translate("mdtUicNumberWidget", "Usage:", 0, QApplication::UnicodeUTF8));
        lbUsage->setText(QString());
        lbCountryLabel->setText(QApplication::translate("mdtUicNumberWidget", "Country:", 0, QApplication::UnicodeUTF8));
        lbCountry->setText(QString());
        lbTypeLabel->setText(QApplication::translate("mdtUicNumberWidget", "Type:", 0, QApplication::UnicodeUTF8));
        lbType->setText(QString());
        lbSpeedAndHeatLabel->setText(QApplication::translate("mdtUicNumberWidget", "Speed/heat:", 0, QApplication::UnicodeUTF8));
        lbSpeedAndHeat->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class mdtUicNumberWidget: public Ui_mdtUicNumberWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MDTUICNUMBERWIDGET_H
