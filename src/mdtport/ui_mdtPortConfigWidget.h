/********************************************************************************
** Form generated from reading UI file 'mdtPortConfigWidget.ui'
**
** Created: Sun May 20 14:41:17 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MDTPORTCONFIGWIDGET_H
#define UI_MDTPORTCONFIGWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mdtPortConfigWidget
{
public:
    QGridLayout *gridLayout_3;
    QLabel *lbFrameType;
    QComboBox *cbFrameType;
    QSpacerItem *horizontalSpacer_2;
    QLabel *lbEndOfFrameSeq;
    QComboBox *cbEndOfFrameSeq;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    QGroupBox *gbReadOptions;
    QGridLayout *gridLayout_2;
    QLabel *lbReadFrameSize;
    QSpinBox *sbReadFrameSize;
    QLabel *lbReadQueueSize;
    QSpinBox *sbReadQueueSize;
    QLabel *lbReadTimeout;
    QSpinBox *sbReadTimeout;
    QCheckBox *cbUseReadTimeoutProtocol;
    QGroupBox *gbWriteOptions;
    QGridLayout *gridLayout;
    QLabel *lbWriteFrameSize;
    QLabel *lbWriteQueueSize;
    QSpinBox *sbWriteQueueSize;
    QLabel *lbWriteTimeout;
    QSpinBox *sbWriteTimeout;
    QLabel *lbWriteMinWaitTime;
    QSpinBox *sbWriteMinWaitTime;
    QCheckBox *cbBytePerByteWrite;
    QSpinBox *sbWriteFrameSize;

    void setupUi(QWidget *mdtPortConfigWidget)
    {
        if (mdtPortConfigWidget->objectName().isEmpty())
            mdtPortConfigWidget->setObjectName(QString::fromUtf8("mdtPortConfigWidget"));
        mdtPortConfigWidget->resize(514, 225);
        gridLayout_3 = new QGridLayout(mdtPortConfigWidget);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        lbFrameType = new QLabel(mdtPortConfigWidget);
        lbFrameType->setObjectName(QString::fromUtf8("lbFrameType"));

        gridLayout_3->addWidget(lbFrameType, 0, 0, 1, 1);

        cbFrameType = new QComboBox(mdtPortConfigWidget);
        cbFrameType->setObjectName(QString::fromUtf8("cbFrameType"));

        gridLayout_3->addWidget(cbFrameType, 0, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(332, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_2, 0, 2, 1, 1);

        lbEndOfFrameSeq = new QLabel(mdtPortConfigWidget);
        lbEndOfFrameSeq->setObjectName(QString::fromUtf8("lbEndOfFrameSeq"));

        gridLayout_3->addWidget(lbEndOfFrameSeq, 1, 0, 1, 1);

        cbEndOfFrameSeq = new QComboBox(mdtPortConfigWidget);
        cbEndOfFrameSeq->setObjectName(QString::fromUtf8("cbEndOfFrameSeq"));
        cbEndOfFrameSeq->setEditable(true);

        gridLayout_3->addWidget(cbEndOfFrameSeq, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(332, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer, 1, 2, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        gbReadOptions = new QGroupBox(mdtPortConfigWidget);
        gbReadOptions->setObjectName(QString::fromUtf8("gbReadOptions"));
        gridLayout_2 = new QGridLayout(gbReadOptions);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        lbReadFrameSize = new QLabel(gbReadOptions);
        lbReadFrameSize->setObjectName(QString::fromUtf8("lbReadFrameSize"));

        gridLayout_2->addWidget(lbReadFrameSize, 0, 0, 1, 1);

        sbReadFrameSize = new QSpinBox(gbReadOptions);
        sbReadFrameSize->setObjectName(QString::fromUtf8("sbReadFrameSize"));
        sbReadFrameSize->setMaximum(5000);

        gridLayout_2->addWidget(sbReadFrameSize, 0, 1, 1, 1);

        lbReadQueueSize = new QLabel(gbReadOptions);
        lbReadQueueSize->setObjectName(QString::fromUtf8("lbReadQueueSize"));

        gridLayout_2->addWidget(lbReadQueueSize, 1, 0, 1, 1);

        sbReadQueueSize = new QSpinBox(gbReadOptions);
        sbReadQueueSize->setObjectName(QString::fromUtf8("sbReadQueueSize"));
        sbReadQueueSize->setMaximum(500);

        gridLayout_2->addWidget(sbReadQueueSize, 1, 1, 1, 1);

        lbReadTimeout = new QLabel(gbReadOptions);
        lbReadTimeout->setObjectName(QString::fromUtf8("lbReadTimeout"));

        gridLayout_2->addWidget(lbReadTimeout, 2, 0, 1, 1);

        sbReadTimeout = new QSpinBox(gbReadOptions);
        sbReadTimeout->setObjectName(QString::fromUtf8("sbReadTimeout"));
        sbReadTimeout->setMaximum(30000);

        gridLayout_2->addWidget(sbReadTimeout, 2, 1, 1, 1);

        cbUseReadTimeoutProtocol = new QCheckBox(gbReadOptions);
        cbUseReadTimeoutProtocol->setObjectName(QString::fromUtf8("cbUseReadTimeoutProtocol"));
        cbUseReadTimeoutProtocol->setTristate(false);

        gridLayout_2->addWidget(cbUseReadTimeoutProtocol, 3, 0, 1, 2);


        horizontalLayout->addWidget(gbReadOptions);

        gbWriteOptions = new QGroupBox(mdtPortConfigWidget);
        gbWriteOptions->setObjectName(QString::fromUtf8("gbWriteOptions"));
        gridLayout = new QGridLayout(gbWriteOptions);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lbWriteFrameSize = new QLabel(gbWriteOptions);
        lbWriteFrameSize->setObjectName(QString::fromUtf8("lbWriteFrameSize"));

        gridLayout->addWidget(lbWriteFrameSize, 0, 0, 1, 1);

        lbWriteQueueSize = new QLabel(gbWriteOptions);
        lbWriteQueueSize->setObjectName(QString::fromUtf8("lbWriteQueueSize"));

        gridLayout->addWidget(lbWriteQueueSize, 1, 0, 1, 1);

        sbWriteQueueSize = new QSpinBox(gbWriteOptions);
        sbWriteQueueSize->setObjectName(QString::fromUtf8("sbWriteQueueSize"));
        sbWriteQueueSize->setMaximum(500);

        gridLayout->addWidget(sbWriteQueueSize, 1, 1, 1, 1);

        lbWriteTimeout = new QLabel(gbWriteOptions);
        lbWriteTimeout->setObjectName(QString::fromUtf8("lbWriteTimeout"));

        gridLayout->addWidget(lbWriteTimeout, 2, 0, 1, 1);

        sbWriteTimeout = new QSpinBox(gbWriteOptions);
        sbWriteTimeout->setObjectName(QString::fromUtf8("sbWriteTimeout"));
        sbWriteTimeout->setMaximum(30000);

        gridLayout->addWidget(sbWriteTimeout, 2, 1, 1, 1);

        lbWriteMinWaitTime = new QLabel(gbWriteOptions);
        lbWriteMinWaitTime->setObjectName(QString::fromUtf8("lbWriteMinWaitTime"));

        gridLayout->addWidget(lbWriteMinWaitTime, 3, 0, 1, 1);

        sbWriteMinWaitTime = new QSpinBox(gbWriteOptions);
        sbWriteMinWaitTime->setObjectName(QString::fromUtf8("sbWriteMinWaitTime"));
        sbWriteMinWaitTime->setMaximum(5000);

        gridLayout->addWidget(sbWriteMinWaitTime, 3, 1, 1, 1);

        cbBytePerByteWrite = new QCheckBox(gbWriteOptions);
        cbBytePerByteWrite->setObjectName(QString::fromUtf8("cbBytePerByteWrite"));

        gridLayout->addWidget(cbBytePerByteWrite, 4, 0, 1, 2);

        sbWriteFrameSize = new QSpinBox(gbWriteOptions);
        sbWriteFrameSize->setObjectName(QString::fromUtf8("sbWriteFrameSize"));
        sbWriteFrameSize->setMaximum(5000);

        gridLayout->addWidget(sbWriteFrameSize, 0, 1, 1, 1);

        lbWriteFrameSize->raise();
        lbWriteQueueSize->raise();
        sbWriteQueueSize->raise();
        lbWriteTimeout->raise();
        sbWriteTimeout->raise();
        sbWriteMinWaitTime->raise();
        lbWriteMinWaitTime->raise();
        cbBytePerByteWrite->raise();
        sbWriteFrameSize->raise();

        horizontalLayout->addWidget(gbWriteOptions);


        gridLayout_3->addLayout(horizontalLayout, 2, 0, 1, 3);

#ifndef QT_NO_SHORTCUT
        lbFrameType->setBuddy(cbFrameType);
        lbEndOfFrameSeq->setBuddy(cbEndOfFrameSeq);
        lbReadFrameSize->setBuddy(sbReadFrameSize);
        lbReadQueueSize->setBuddy(sbReadQueueSize);
        lbReadTimeout->setBuddy(sbReadTimeout);
        lbWriteFrameSize->setBuddy(sbWriteFrameSize);
        lbWriteQueueSize->setBuddy(sbWriteQueueSize);
        lbWriteTimeout->setBuddy(sbWriteTimeout);
        lbWriteMinWaitTime->setBuddy(sbWriteMinWaitTime);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(cbFrameType, cbEndOfFrameSeq);
        QWidget::setTabOrder(cbEndOfFrameSeq, sbReadFrameSize);
        QWidget::setTabOrder(sbReadFrameSize, sbReadQueueSize);
        QWidget::setTabOrder(sbReadQueueSize, sbReadTimeout);
        QWidget::setTabOrder(sbReadTimeout, cbUseReadTimeoutProtocol);
        QWidget::setTabOrder(cbUseReadTimeoutProtocol, sbWriteFrameSize);
        QWidget::setTabOrder(sbWriteFrameSize, sbWriteQueueSize);
        QWidget::setTabOrder(sbWriteQueueSize, sbWriteTimeout);
        QWidget::setTabOrder(sbWriteTimeout, sbWriteMinWaitTime);
        QWidget::setTabOrder(sbWriteMinWaitTime, cbBytePerByteWrite);

        retranslateUi(mdtPortConfigWidget);

        QMetaObject::connectSlotsByName(mdtPortConfigWidget);
    } // setupUi

    void retranslateUi(QWidget *mdtPortConfigWidget)
    {
        mdtPortConfigWidget->setWindowTitle(QApplication::translate("mdtPortConfigWidget", "Form", 0, QApplication::UnicodeUTF8));
        lbFrameType->setText(QApplication::translate("mdtPortConfigWidget", "Frame &type", 0, QApplication::UnicodeUTF8));
        lbEndOfFrameSeq->setText(QApplication::translate("mdtPortConfigWidget", "End of frame", 0, QApplication::UnicodeUTF8));
        gbReadOptions->setTitle(QApplication::translate("mdtPortConfigWidget", "Read options", 0, QApplication::UnicodeUTF8));
        lbReadFrameSize->setText(QApplication::translate("mdtPortConfigWidget", "Frame size", 0, QApplication::UnicodeUTF8));
        lbReadQueueSize->setText(QApplication::translate("mdtPortConfigWidget", "Queue size", 0, QApplication::UnicodeUTF8));
        lbReadTimeout->setText(QApplication::translate("mdtPortConfigWidget", "Timeout [ms]", 0, QApplication::UnicodeUTF8));
        cbUseReadTimeoutProtocol->setText(QApplication::translate("mdtPortConfigWidget", "Use read timeout protocol", 0, QApplication::UnicodeUTF8));
        gbWriteOptions->setTitle(QApplication::translate("mdtPortConfigWidget", "Write options", 0, QApplication::UnicodeUTF8));
        lbWriteFrameSize->setText(QApplication::translate("mdtPortConfigWidget", "Frame size", 0, QApplication::UnicodeUTF8));
        lbWriteQueueSize->setText(QApplication::translate("mdtPortConfigWidget", "Queue size", 0, QApplication::UnicodeUTF8));
        lbWriteTimeout->setText(QApplication::translate("mdtPortConfigWidget", "Timeout [ms]", 0, QApplication::UnicodeUTF8));
        lbWriteMinWaitTime->setText(QApplication::translate("mdtPortConfigWidget", "Wait time [ms]", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        sbWriteMinWaitTime->setToolTip(QApplication::translate("mdtPortConfigWidget", "Time to wait before sending a frame, or a byte if \"Write byte per byte\" option is set.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cbBytePerByteWrite->setText(QApplication::translate("mdtPortConfigWidget", "Write byte per byte", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mdtPortConfigWidget: public Ui_mdtPortConfigWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MDTPORTCONFIGWIDGET_H
