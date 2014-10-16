/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtTtLogicalTestCableTsWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include <QString>
#include <QVariant>

mdtTtLogicalTestCableTsWidget::mdtTtLogicalTestCableTsWidget(QWidget* parent, QSqlDatabase db)
 : QGroupBox(parent),
   pvDatabase(db)
{
  setupUi(this);

  lbSystem->clear();
  lbNode->clear();
  lbCn->clear();
}

void mdtTtLogicalTestCableTsWidget::setTestCableConnector(const QVariant& connectorId, const QString& name)
{
  pvCnId = connectorId;
  pvCnType = Connector;
  lbTestCableCnLabel->setText(tr("Test cable connector:"));
  lbTestCableCn->setText(name);
  lbCnLabel->setText(tr("Connector:"));
}

void mdtTtLogicalTestCableTsWidget::setTestCableConnection(const QVariant& connectionId, const QString& name)
{
  pvCnId = connectionId;
  pvCnType = Connection;
  lbTestCableCnLabel->setText(tr("Test cable connection:"));
  lbTestCableCn->setText(name);
  lbCnLabel->setText(tr("Connection:"));
}
