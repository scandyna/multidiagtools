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
#include "mdtTtLogicalTestCableDutWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include <QString>
#include <QVariant>

mdtTtLogicalTestCableDutWidget::mdtTtLogicalTestCableDutWidget(QWidget* parent)
 : QGroupBox(parent)
{
  setupUi(this);
  connect(pbSelectDut, SIGNAL(clicked()), this, SLOT(selectDut()));
  connect(pbSelectCn, SIGNAL(clicked()), this, SLOT(selectCn()));
  lbSchemaPosition->clear();
  lbAlias->clear();
  lbCn->clear();
}

void mdtTtLogicalTestCableDutWidget::setTestCableConnector(const QVariant& connectorId, const QString& name)
{
  pvCnId = connectorId;
  pvCnType = Connector;
  lbTestCableCnLabel->setText(tr("Test cable connector:"));
  lbTestCableCn->setText(name);
  lbCnLabel->setText(tr("Connector:"));
}

void mdtTtLogicalTestCableDutWidget::setTestCableConnection(const QVariant& connectionId, const QString& name)
{
  pvCnId = connectionId;
  pvCnType = Connection;
  lbTestCableCnLabel->setText(tr("Test cable connection:"));
  lbTestCableCn->setText(name);
  lbCnLabel->setText(tr("Connection:"));
}

void mdtTtLogicalTestCableDutWidget::selectDut()
{

}

void mdtTtLogicalTestCableDutWidget::selectCn()
{

}
