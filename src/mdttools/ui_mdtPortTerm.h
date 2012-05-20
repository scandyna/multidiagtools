/********************************************************************************
** Form generated from reading UI file 'mdtPortTerm.ui'
**
** Created: Sun May 20 14:42:46 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MDTPORTTERM_H
#define UI_MDTPORTTERM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mdtPortTerm
{
public:
    QAction *action_Quit;
    QAction *action_Setup;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *lbTerm;
    QPushButton *pbClearTerm;
    QSpacerItem *horizontalSpacer_2;
    QTextEdit *teTerm;
    QLabel *lbCmd;
    QPlainTextEdit *teCmd;
    QHBoxLayout *bottomHLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pbSendCmdAbort;
    QPushButton *pbSendCmd;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menu_Port;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *mdtPortTerm)
    {
        if (mdtPortTerm->objectName().isEmpty())
            mdtPortTerm->setObjectName(QString::fromUtf8("mdtPortTerm"));
        mdtPortTerm->resize(624, 466);
        action_Quit = new QAction(mdtPortTerm);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        action_Setup = new QAction(mdtPortTerm);
        action_Setup->setObjectName(QString::fromUtf8("action_Setup"));
        centralwidget = new QWidget(mdtPortTerm);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lbTerm = new QLabel(centralwidget);
        lbTerm->setObjectName(QString::fromUtf8("lbTerm"));

        horizontalLayout->addWidget(lbTerm);

        pbClearTerm = new QPushButton(centralwidget);
        pbClearTerm->setObjectName(QString::fromUtf8("pbClearTerm"));

        horizontalLayout->addWidget(pbClearTerm);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);

        teTerm = new QTextEdit(centralwidget);
        teTerm->setObjectName(QString::fromUtf8("teTerm"));
        teTerm->setFocusPolicy(Qt::ClickFocus);
        teTerm->setReadOnly(true);

        verticalLayout->addWidget(teTerm);

        lbCmd = new QLabel(centralwidget);
        lbCmd->setObjectName(QString::fromUtf8("lbCmd"));

        verticalLayout->addWidget(lbCmd);

        teCmd = new QPlainTextEdit(centralwidget);
        teCmd->setObjectName(QString::fromUtf8("teCmd"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(teCmd->sizePolicy().hasHeightForWidth());
        teCmd->setSizePolicy(sizePolicy);
        teCmd->setMinimumSize(QSize(0, 80));
        teCmd->setMaximumSize(QSize(16777215, 80));

        verticalLayout->addWidget(teCmd);

        bottomHLayout = new QHBoxLayout();
        bottomHLayout->setObjectName(QString::fromUtf8("bottomHLayout"));
        horizontalSpacer = new QSpacerItem(398, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        bottomHLayout->addItem(horizontalSpacer);

        pbSendCmdAbort = new QPushButton(centralwidget);
        pbSendCmdAbort->setObjectName(QString::fromUtf8("pbSendCmdAbort"));

        bottomHLayout->addWidget(pbSendCmdAbort);

        pbSendCmd = new QPushButton(centralwidget);
        pbSendCmd->setObjectName(QString::fromUtf8("pbSendCmd"));

        bottomHLayout->addWidget(pbSendCmd);


        verticalLayout->addLayout(bottomHLayout);

        mdtPortTerm->setCentralWidget(centralwidget);
        teTerm->raise();
        teCmd->raise();
        pbSendCmd->raise();
        lbCmd->raise();
        lbTerm->raise();
        pbClearTerm->raise();
        pbSendCmdAbort->raise();
        menubar = new QMenuBar(mdtPortTerm);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 624, 20));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_Port = new QMenu(menubar);
        menu_Port->setObjectName(QString::fromUtf8("menu_Port"));
        mdtPortTerm->setMenuBar(menubar);
        statusbar = new QStatusBar(mdtPortTerm);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        mdtPortTerm->setStatusBar(statusbar);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menu_Port->menuAction());
        menu_File->addAction(action_Quit);
        menu_Port->addAction(action_Setup);

        retranslateUi(mdtPortTerm);
        QObject::connect(action_Quit, SIGNAL(activated()), mdtPortTerm, SLOT(close()));

        QMetaObject::connectSlotsByName(mdtPortTerm);
    } // setupUi

    void retranslateUi(QMainWindow *mdtPortTerm)
    {
        mdtPortTerm->setWindowTitle(QApplication::translate("mdtPortTerm", "MDT Port Terminal", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("mdtPortTerm", "&Quit", 0, QApplication::UnicodeUTF8));
        action_Setup->setText(QApplication::translate("mdtPortTerm", "&Setup", 0, QApplication::UnicodeUTF8));
        lbTerm->setText(QApplication::translate("mdtPortTerm", "RX data", 0, QApplication::UnicodeUTF8));
        pbClearTerm->setText(QApplication::translate("mdtPortTerm", "&Clear", 0, QApplication::UnicodeUTF8));
        lbCmd->setText(QApplication::translate("mdtPortTerm", "Command", 0, QApplication::UnicodeUTF8));
        pbSendCmdAbort->setText(QApplication::translate("mdtPortTerm", "Abort", 0, QApplication::UnicodeUTF8));
        pbSendCmd->setText(QApplication::translate("mdtPortTerm", "Send", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("mdtPortTerm", "&File", 0, QApplication::UnicodeUTF8));
        menu_Port->setTitle(QApplication::translate("mdtPortTerm", "&Port", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mdtPortTerm: public Ui_mdtPortTerm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MDTPORTTERM_H
