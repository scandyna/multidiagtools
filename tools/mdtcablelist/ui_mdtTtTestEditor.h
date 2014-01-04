/********************************************************************************
** Form generated from reading UI file 'mdtTtTestEditor.ui'
**
** Created: Fri Jan 3 22:00:13 2014
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MDTTTTESTEDITOR_H
#define UI_MDTTTTESTEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mdtTtTestEditor
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *lbDesignationEN;
    QLineEdit *fld_DesignationEN;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbGenerateNodeUnitSetup;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *mdtTtTestEditor)
    {
        if (mdtTtTestEditor->objectName().isEmpty())
            mdtTtTestEditor->setObjectName(QString::fromUtf8("mdtTtTestEditor"));
        mdtTtTestEditor->resize(518, 101);
        verticalLayout = new QVBoxLayout(mdtTtTestEditor);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        lbDesignationEN = new QLabel(mdtTtTestEditor);
        lbDesignationEN->setObjectName(QString::fromUtf8("lbDesignationEN"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lbDesignationEN);

        fld_DesignationEN = new QLineEdit(mdtTtTestEditor);
        fld_DesignationEN->setObjectName(QString::fromUtf8("fld_DesignationEN"));

        formLayout->setWidget(0, QFormLayout::FieldRole, fld_DesignationEN);


        verticalLayout->addLayout(formLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pbGenerateNodeUnitSetup = new QPushButton(mdtTtTestEditor);
        pbGenerateNodeUnitSetup->setObjectName(QString::fromUtf8("pbGenerateNodeUnitSetup"));

        horizontalLayout->addWidget(pbGenerateNodeUnitSetup);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(mdtTtTestEditor);

        QMetaObject::connectSlotsByName(mdtTtTestEditor);
    } // setupUi

    void retranslateUi(QWidget *mdtTtTestEditor)
    {
        mdtTtTestEditor->setWindowTitle(QApplication::translate("mdtTtTestEditor", "Form", 0, QApplication::UnicodeUTF8));
        lbDesignationEN->setText(QApplication::translate("mdtTtTestEditor", "Designation (ENG)", 0, QApplication::UnicodeUTF8));
        pbGenerateNodeUnitSetup->setText(QApplication::translate("mdtTtTestEditor", "Generate test node unit setup", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mdtTtTestEditor: public Ui_mdtTtTestEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MDTTTTESTEDITOR_H
