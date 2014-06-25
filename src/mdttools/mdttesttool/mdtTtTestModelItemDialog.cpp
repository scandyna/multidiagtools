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
#include "mdtTtTestModelItemDialog.h"
#include "mdtSqlTableWidget.h"
#include <QVBoxLayout>
#include <QSqlTableModel>
#include <QSqlError>
#include <QMessageBox>
#include <QTableView>

mdtTtTestModelItemDialog::mdtTtTestModelItemDialog(QDialog *parent, QSqlDatabase db)
 : QDialog(parent)
{
  QVBoxLayout *l;
  QSqlTableModel *m;

  // Setup main data part
  setupUi(this);
  // Setup test link part
  l = new QVBoxLayout;
  pvLinkTableWidget = new mdtSqlTableWidget;
  m = new QSqlTableModel(this, db);
  pvLinkTableWidget->setModel(m);
  l->addWidget(pvLinkTableWidget);
  tbTestLink->setLayout(l);
  // Setup test node unit setup part
  l = new QVBoxLayout;
  pvSetupTableWidget = new mdtSqlTableWidget;
  m = new QSqlTableModel(this, db);
  pvSetupTableWidget->setModel(m);
  l->addWidget(pvSetupTableWidget);
  tbTestNodeUnitSetup->setLayout(l);
}

bool mdtTtTestModelItemDialog::setTestModelItemId(const QVariant & id)
{
  if(!setupTestLinkTable(id)){
    return false;
  }
  if(!setupTestNodeUnitSetupTable(id)){
    return false;
  }
  return true;
}

bool mdtTtTestModelItemDialog::setupTestLinkTable(const QVariant & testModelItemId)
{
  QSqlTableModel *m;
  QSqlError sqlError;

  m = pvLinkTableWidget->model();
  Q_ASSERT(m != 0);
  m->setTable("TestModelItem_TestLink_view");
  if(testModelItemId.isNull()){
    m->setFilter("TestModelItem_Id_FK IS NULL");
  }else{
    m->setFilter("TestModelItem_Id_FK = " + testModelItemId.toString());
  }
  if(!m->select()){
    sqlError = m->lastError();
    pvLastError.setError(tr("Cannot select data in table '") + m->tableName() + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelItemDialog");
    pvLastError.commit();
    displayLastError();
    return false;
  }
  // Hide technical fields
  pvLinkTableWidget->setColumnHidden("TestModelItem_Id_FK", true);
  pvLinkTableWidget->setColumnHidden("TestLink_Id_FK", true);
  pvLinkTableWidget->setColumnHidden("TestConnection_Id_FK", true);
  pvLinkTableWidget->setColumnHidden("DutConnection_Id_FK", true);
  pvLinkTableWidget->setColumnHidden("Unit_Id_FK_PK", true);
  pvLinkTableWidget->setColumnHidden("DutUnitId", true);
  pvLinkTableWidget->setColumnHidden("VehicleType_Id_FK_PK", true);
  // Rename visible fields to something human friendly
  pvLinkTableWidget->setHeaderData("IoPosition", tr("I/O\nPos."));
  pvLinkTableWidget->setHeaderData("TestNodeUnitSchemaPosition", tr("Test node\nunit\nschema pos."));
  pvLinkTableWidget->setHeaderData("TestConnectorName", tr("Test\nconnector"));
  pvLinkTableWidget->setHeaderData("TestContactName", tr("Test\ncontact"));
  pvLinkTableWidget->setHeaderData("DutUnitSchemaPosition", tr("DUT\nSchema pos."));
  pvLinkTableWidget->setHeaderData("DutUnitAlias", tr("DUT\nalias"));
  pvLinkTableWidget->setHeaderData("DutConnectorName", tr("DUT\nconnector"));
  pvLinkTableWidget->setHeaderData("DutContactName", tr("DUT\ncontact"));
  // Other att.
  pvLinkTableWidget->tableView()->resizeColumnsToContents();

  return true;
}

bool mdtTtTestModelItemDialog::setupTestNodeUnitSetupTable(const QVariant & testModelItemId)
{
  QSqlTableModel *m;
  QSqlError sqlError;

  m = pvSetupTableWidget->model();
  Q_ASSERT(m != 0);
  m->setTable("TestNodeUnitSetup_tbl");
  if(testModelItemId.isNull()){
    m->setFilter("TestModelItem_Id_FK IS NULL");
  }else{
    m->setFilter("TestModelItem_Id_FK = " + testModelItemId.toString());
  }
  if(!m->select()){
    sqlError = m->lastError();
    pvLastError.setError(tr("Cannot select data in table '") + m->tableName() + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelItemDialog");
    pvLastError.commit();
    displayLastError();
    return false;
  }

  return true;
}

void mdtTtTestModelItemDialog::displayLastError()
{
  QMessageBox msgBox;

  msgBox.setText(pvLastError.text());
  msgBox.setDetailedText(pvLastError.systemText());
  msgBox.setIcon(pvLastError.levelIcon());
  msgBox.exec();
}
