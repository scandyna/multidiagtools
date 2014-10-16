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
#include "mdtClLink.h"
#include "mdtError.h"
#include <QString>
#include <QVariant>
#include <QMessageBox>

mdtTtLogicalTestCableDutWidget::mdtTtLogicalTestCableDutWidget(QWidget* parent, QSqlDatabase db)
 : QGroupBox(parent),
   pvDatabase(db)
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
  pvTestCableCnId = connectorId;
  pvCnType = Connector;
  lbTestCableCnLabel->setText(tr("Test cable connector:"));
  lbTestCableCn->setText(name);
  lbCnLabel->setText(tr("Connector:"));
}

void mdtTtLogicalTestCableDutWidget::setTestCableConnection(const QVariant& connectionId, const QString& name)
{
  pvTestCableCnId = connectionId;
  pvCnType = Connection;
  lbTestCableCnLabel->setText(tr("Test cable connection:"));
  lbTestCableCn->setText(name);
  lbCnLabel->setText(tr("Connection:"));
}

void mdtTtLogicalTestCableDutWidget::selectDut()
{
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  QString sql;

  // Build SQL query
  sql = "SELECT * FROM Unit_view";
  // Setup and show dialog
  selectionDialog.setMessage(tr("Select DUT unit:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Store selected DUT unit ID
  s = selectionDialog.selection("Unit_Id_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  pvUnitId = s.data(0, "Unit_Id_PK");
}

void mdtTtLogicalTestCableDutWidget::selectCn()
{
  // Check that DUT unit was selected
  if(pvUnitId.isNull()){
    return;
  }
  // Check that test cable CN was set
  if(pvTestCableCnId.isNull()){
    return;
  }
  // Call selection method regarding CN type
  switch(pvCnType){
    case Connector:
      selectDutConnector();
      break;
    case Connection:
      selectDutConnection();
      break;
  }
}

void mdtTtLogicalTestCableDutWidget::selectDutConnector()
{
  mdtClLink lnk(0, pvDatabase);
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  mdtClConnectableCriteria c;
  QString sql;
  bool ok;

  // Get SQL query
  sql = lnk.sqlForConnectableUnitConnectorsSelection(pvTestCableCnId, pvUnitId, c, ok);
  if(!ok){
    displayError(lnk.lastError());
    return;
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Select DUT connector:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Store selected DUT connector ID
  s = selectionDialog.selection("???");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  pvCnId = s.data(0, "???");
}

void mdtTtLogicalTestCableDutWidget::selectDutConnection()
{

}

void mdtTtLogicalTestCableDutWidget::displayError(const mdtError& error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
