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
#include "mdtClVehicleTypeEditor.h"
#include "ui_mdtClVehicleTypeEditor.h"
#include "mdtSqlWindow.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtError.h"
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QStringList>
#include <QString>
#include <QList>
#include <QVariant>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>

#include <QDebug>

mdtClVehicleTypeEditor::mdtClVehicleTypeEditor(QObject *parent, const QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  // Setup unit
  pvVehicleTypeWidget = new mdtSqlFormWidget;
  Ui::mdtClVehicleTypeEditor *e = new Ui::mdtClVehicleTypeEditor;
  e->setupUi(pvVehicleTypeWidget);
  pvVehicleTypeModel = new QSqlTableModel(this, pvDatabase);
  // Setup connection
  pvUnitWidget = new mdtSqlTableWidget;
  pvUnitModel = new QSqlTableModel(this, pvDatabase);

  pvUnitRelation = new mdtSqlRelation;
}

mdtClVehicleTypeEditor::~mdtClVehicleTypeEditor()
{
  ///delete pvVehicleTypeWidget;
  ///delete pvUnitWidget;
  delete pvUnitRelation;
  ///delete pvVehicleTypeModel;
  ///delete pvUnitModel;
  ///delete pvVehicleTypeUnitModel;
}

bool mdtClVehicleTypeEditor::setupTables(bool includeConnections)
{
  QSqlError sqlError;

  // Setup Unit table
  if(!setupVehicleTypeTable()){
    return false;
  }
  // Setup connection table
  if(!setupUnitTable()){
    return false;
  }

  return true;
}

void mdtClVehicleTypeEditor::setupUi(mdtSqlWindow *window)
{
  Q_ASSERT(window != 0);

  window->setSqlWidget(pvVehicleTypeWidget);
  window->addChildWidget(pvUnitWidget, "Units");
  window->enableNavigation();
  window->enableEdition();
  window->resize(700, 500);
}

int mdtClVehicleTypeEditor::currentVehicleTypeId()
{
  int vehicleTypeIdColumn;
  QModelIndex index;

  if(pvVehicleTypeWidget->currentRow() < 0){
    return -1;
  }
  vehicleTypeIdColumn = pvVehicleTypeModel->record().indexOf("Id_PK");
  if(vehicleTypeIdColumn < 0){
    return -1;
  }
  index = pvVehicleTypeModel->index(pvVehicleTypeWidget->currentRow(), vehicleTypeIdColumn);
  if(!index.isValid()){
    return -1;
  }

  return pvVehicleTypeModel->data(index).toInt();
}

bool mdtClVehicleTypeEditor::setupVehicleTypeTable()
{
  QSqlError sqlError;

  pvVehicleTypeModel->setTable("VehicleType_tbl");
  if(!pvVehicleTypeModel->select()){
    sqlError = pvVehicleTypeModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'VehicleType_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClVehicleTypeEditor");
    e.commit();
    return false;
  }
  pvVehicleTypeWidget->setModel(pvVehicleTypeModel);
  pvVehicleTypeWidget->mapFormWidgets();

  return true;
}

bool mdtClVehicleTypeEditor::setupUnitTable()
{
  QSqlError sqlError;

  pvUnitModel->setTable("Unit_tbl");
  if(!pvUnitModel->select()){
    sqlError = pvUnitModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'Unit_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClVehicleTypeEditor");
    e.commit();
    return false;
  }
  pvUnitWidget->setModel(pvUnitModel);
  pvUnitWidget->enableLocalEdition();
  // Hide relation fields and PK
  ///pvUnitWidget->setColumnHidden("Id_PK", true);
  ///pvUnitWidget->setColumnHidden("Article_Id_FK", true);
  // Give fields a user friendly name
  ///pvUnitWidget->setHeaderData("ConnectorName", tr("Connector"));
  ///pvUnitWidget->setHeaderData("ContactName", tr("Contact"));
  ///pvUnitWidget->setHeaderData("IoType", tr("I/O type"));
  // Setup Article <-> Connection relation
  pvUnitRelation->setParentModel(pvVehicleTypeModel);
  pvUnitRelation->setChildModel(pvUnitModel);
  if(!pvUnitRelation->addRelation("Id_PK", "VehicleType_Id_FK", true)){
    return false;
  }
  pvVehicleTypeWidget->addChildWidget(pvUnitWidget, pvUnitRelation);

  return true;
}
