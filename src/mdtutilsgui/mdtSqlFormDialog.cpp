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
#include "mdtSqlFormDialog.h"
#include "mdtSqlDialog.h"
#include "mdtAbstractSqlWidget.h"
#include "mdtSqlFormWidget.h"

mdtSqlFormDialog::mdtSqlFormDialog(QObject *parent)
 : mdtSqlFormOld(parent)
{
  pvSqlDialog = new mdtSqlDialog;
  pvSqlDialog->setSqlWidget(mainSqlWidget());
}

mdtSqlFormDialog::~mdtSqlFormDialog()
{
  delete pvSqlDialog;
}

mdtSqlDialog *mdtSqlFormDialog::sqlDialog()
{
  Q_ASSERT(pvSqlDialog != 0);

  return pvSqlDialog;
}

int mdtSqlFormDialog::exec()
{
  return pvSqlDialog->exec();
}

void mdtSqlFormDialog::addChildWidget(mdtAbstractSqlWidget *widget)
{
  Q_ASSERT(widget != 0);

  pvSqlDialog->addChildWidget(widget, widget->userFriendlyTableName());
}

