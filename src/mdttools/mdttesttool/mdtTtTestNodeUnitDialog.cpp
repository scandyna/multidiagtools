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
#include "mdtTtTestNodeUnitDialog.h"
#include "mdtTtTestNodeUnit.h"
#include "mdtSqlRecord.h"
#include "mdtClUnit.h"
#include "mdtClUnitConnectorData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtSqlSelectionDialog.h"
#include <QString>
#include <QLocale>

mdtTtTestNodeUnitDialog::mdtTtTestNodeUnitDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  pvDatabase = db;
  // Setup UI
  setupUi(this);
  setupBusComboBox();
  setupTypeComboBox();
  connect(pbSelectUnit, SIGNAL(clicked()), this, SLOT(selectBaseUnit()));
  updateDialog();
}

void mdtTtTestNodeUnitDialog::setData(const mdtTtTestNodeUnitData & data) 
{
  pvData = data;
  updateDialog();
}

mdtTtTestNodeUnitData mdtTtTestNodeUnitDialog::data()
{
  return pvData;
}

void mdtTtTestNodeUnitDialog::selectBaseUnit() 
{
  mdtTtTestNodeUnit tnu(0, pvDatabase);
  mdtSqlSelectionDialog selectionDialog;
  QString sql;
  mdtSqlRecord unitData;
  bool ok;

  if(pvData.value("TestNode_Id_FK").isNull()){
    return;
  }
  // Setup SQL
  sql = "SELECT * FROM Unit_view ";
  sql += " WHERE VehicleType_Id_PK = " + pvData.value("TestNode_Id_FK").toString();
  // Setup and show dialog
  selectionDialog.setMessage("Please select unit:");
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("VehicleType_Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_PK", true);
  selectionDialog.setColumnHidden("Type", true);
  selectionDialog.setColumnHidden("SubType", true);
  selectionDialog.setColumnHidden("SeriesNumber", true);
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema position"));
  selectionDialog.setHeaderData("ArticleCode", tr("Article code"));
  selectionDialog.setHeaderData("DesignationEN", tr("Designation\n(English)"));
  selectionDialog.setHeaderData("DesignationFR", tr("Designation\n(French)"));
  selectionDialog.setHeaderData("DesignationDE", tr("Designation\n(German)"));
  selectionDialog.setHeaderData("DesignationIT", tr("Designation\n(Italian)"));
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema position"));
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("Unit_Id_PK");
  selectionDialog.resize(700, 400);
  selectionDialog.setWindowTitle(tr("Unit selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);
  // Store result
  unitData = tnu.getUnitData(selectionDialog.selectionResult().at(0), &ok);
  /// \todo Message on error

  pvData.setUnitData(unitData);
  updateDialog();
}

void mdtTtTestNodeUnitDialog::selectTestConnection() 
{
}

void mdtTtTestNodeUnitDialog::accept() 
{
}

void mdtTtTestNodeUnitDialog::reject() 
{
  pvData.clearValues();
  QDialog::reject();
}

void mdtTtTestNodeUnitDialog::updateDialog()
{
  mdtSqlRecord unitData;

  // Get data
  unitData = pvData.unitData();
  // Update UI
  lbSchemaPosition->setText(unitData.value("SchemaPosition").toString());
  lbAlias->setText(unitData.value("Alias").toString());
  displayTestConnection();
  displayBus();
  displayType();
}

void mdtTtTestNodeUnitDialog::displayTestConnection()
{
  mdtClUnit unit(0, pvDatabase);
  mdtClUnitConnectorData connectorData;
  QVariant unitConnectorId;
  bool ok;

  // Set connector name
  unitConnectorId = pvData.testConnectionData().value("UnitConnector_Id_FK");
  if(unitConnectorId.isNull()){
    lbTestConnector->clear();
  }else{
    connectorData = unit.getConnectorData(unitConnectorId, &ok, false, false, false);
    if(ok){
      lbTestConnector->setText(connectorData.value("Name").toString());
    }else{
      lbTestConnector->setText("<error!>");
    }
  }
  // Set contact name
  lbContactName->setText(pvData.testConnectionData().value("UnitContactName").toString());
}

void mdtTtTestNodeUnitDialog::displayBus()
{

}

void mdtTtTestNodeUnitDialog::displayType()
{

}

void mdtTtTestNodeUnitDialog::setupBusComboBox() 
{
  cbBus->clear();
  cbBus->addItem("BUSA");
  cbBus->addItem("BUSB");
}

void mdtTtTestNodeUnitDialog::setupTypeComboBox() 
{
  QLocale locale;
  QString fieldName;
  QString sql;

  // Select field regarding language
  switch(locale.language()){
    case QLocale::French:
      fieldName = "NameFR";
      break;
    case QLocale::German:
      fieldName = "NameDE";
      break;
    case QLocale::Italian:
      fieldName = "NameIT";
      break;
    default:
      fieldName = "NameEN";
  }
  // Setup type model and combobox
  sql = "SELECT Code_PK, " + fieldName + " FROM TestNodeUnitType_tbl";
  pvTypeModel.setQuery(sql, pvDatabase);
  cbType->setModel(&pvTypeModel);
  cbType->setModelColumn(1);

}

