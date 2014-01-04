/********************************************************************************
** Form generated from reading UI file 'mdtTtTestItemEditor.ui'
**
** Created: Fri Jan 3 22:00:13 2014
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MDTTTTESTITEMEDITOR_H
#define UI_MDTTTTESTITEMEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mdtTtTestItemEditor
{
public:
    QFormLayout *formLayout;
    QLabel *lbSequenceNumber;
    QLineEdit *fld_SequenceNumber;
    QLabel *lbExpectedValue;
    QLineEdit *fld_ExpectedValue;

    void setupUi(QWidget *mdtTtTestItemEditor)
    {
        if (mdtTtTestItemEditor->objectName().isEmpty())
            mdtTtTestItemEditor->setObjectName(QString::fromUtf8("mdtTtTestItemEditor"));
        mdtTtTestItemEditor->resize(342, 62);
        formLayout = new QFormLayout(mdtTtTestItemEditor);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        lbSequenceNumber = new QLabel(mdtTtTestItemEditor);
        lbSequenceNumber->setObjectName(QString::fromUtf8("lbSequenceNumber"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lbSequenceNumber);

        fld_SequenceNumber = new QLineEdit(mdtTtTestItemEditor);
        fld_SequenceNumber->setObjectName(QString::fromUtf8("fld_SequenceNumber"));

        formLayout->setWidget(0, QFormLayout::FieldRole, fld_SequenceNumber);

        lbExpectedValue = new QLabel(mdtTtTestItemEditor);
        lbExpectedValue->setObjectName(QString::fromUtf8("lbExpectedValue"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lbExpectedValue);

        fld_ExpectedValue = new QLineEdit(mdtTtTestItemEditor);
        fld_ExpectedValue->setObjectName(QString::fromUtf8("fld_ExpectedValue"));

        formLayout->setWidget(1, QFormLayout::FieldRole, fld_ExpectedValue);


        retranslateUi(mdtTtTestItemEditor);

        QMetaObject::connectSlotsByName(mdtTtTestItemEditor);
    } // setupUi

    void retranslateUi(QWidget *mdtTtTestItemEditor)
    {
        mdtTtTestItemEditor->setWindowTitle(QApplication::translate("mdtTtTestItemEditor", "Form", 0, QApplication::UnicodeUTF8));
        lbSequenceNumber->setText(QApplication::translate("mdtTtTestItemEditor", "Seq. #:", 0, QApplication::UnicodeUTF8));
        lbExpectedValue->setText(QApplication::translate("mdtTtTestItemEditor", "Expected value:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mdtTtTestItemEditor: public Ui_mdtTtTestItemEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MDTTTTESTITEMEDITOR_H
