/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtClTerminalEditorDialog.h"
#include <QLineEdit>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QDialogButtonBox>

mdtClTerminalEditorDialog::mdtClTerminalEditorDialog(QWidget *parent)
 : QDialog(parent)
{
  QFormLayout *fl = new QFormLayout;
  QVBoxLayout *mainLayout = new QVBoxLayout;
  QGroupBox *gb = new QGroupBox;
  QVBoxLayout *gbLayout = new QVBoxLayout;
  QHBoxLayout *gbPbLayout = new QHBoxLayout;
  QPushButton *pb;

  // Add terminal bock fields
  lbTerminalBlock = new QLabel(tr("Terminal block"));
  leTerminalBlock = new QLineEdit;
  leTerminalBlock->setReadOnly(true);
  fl->addRow(lbTerminalBlock, leTerminalBlock);
  // Add connector/contact fields
  lbConnectorName = new QLabel(tr("Connector"));
  leConnectorName = new QLineEdit;
  fl->addRow(lbConnectorName, leConnectorName);
  lbContactName = new QLabel(tr("Contact"));
  leContactName = new QLineEdit;
  fl->addRow(lbContactName, leContactName);
  mainLayout->addLayout(fl);
  // Terminal ranges
  gb->setTitle(tr("Ranges"));
  tvRanges = new QTableView;
  gbLayout->addWidget(tvRanges);
  pbAddRange = new QPushButton(tr("Add..."));
  gbPbLayout->addWidget(pbAddRange);
  pbRemoveRange = new QPushButton(tr("Remove"));
  gbPbLayout->addWidget(pbRemoveRange);
  gbPbLayout->addStretch();
  gbLayout->addLayout(gbPbLayout);
  gb->setLayout(gbLayout);
  mainLayout->addWidget(gb);
  // Dialog buttons
  pvDialogButtons = new QDialogButtonBox(this);
  pb = pvDialogButtons->addButton(QDialogButtonBox::Ok);
  connect(pb, SIGNAL(clicked()), this, SLOT(accept()));
  pb = pvDialogButtons->addButton(QDialogButtonBox::Cancel);
  connect(pb, SIGNAL(clicked()), this, SLOT(reject()));
  mainLayout->addWidget(pvDialogButtons);

  setLayout(mainLayout);
}

mdtClTerminalEditorDialog::~mdtClTerminalEditorDialog()
{
}

void mdtClTerminalEditorDialog::setTerminalBlockName(const QString &name)
{
  Q_ASSERT(leTerminalBlock != 0);

  leTerminalBlock->setText(name);
}

void mdtClTerminalEditorDialog::setConnectorName(const QString &name)
{
  Q_ASSERT(leConnectorName != 0);

  leConnectorName->setText(name);
}

QVariant mdtClTerminalEditorDialog::connectorName() const
{
  return pvConnectorName;
}

QVariant mdtClTerminalEditorDialog::contactName() const
{
  return pvContactName;
}

void mdtClTerminalEditorDialog::accept()
{
  pvConnectorName = leConnectorName->text();
  pvContactName = leContactName->text();
  QDialog::accept();
}

void mdtClTerminalEditorDialog::reject()
{
  QDialog::reject();
}
