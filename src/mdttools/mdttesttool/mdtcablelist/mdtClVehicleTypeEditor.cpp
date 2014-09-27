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
#include "mdtClVehicleTypeEditor.h"
#include "ui_mdtClVehicleTypeEditor.h"
///#include "mdtSqlFormWidget.h"
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
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>

#include <QDebug>

mdtClVehicleTypeEditor::mdtClVehicleTypeEditor(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

mdtClVehicleTypeEditor::~mdtClVehicleTypeEditor()
{
}

bool mdtClVehicleTypeEditor::setupTables()
{
  if(!setupVehicleTypeTable()){
    return false;
  }
  if(!setupUnitTable()){
    return false;
  }

  return true;
}

QVariant mdtClVehicleTypeEditor::currentVehicleTypeId()
{
  return currentData("VehicleType_tbl", "Id_PK");
}

bool mdtClVehicleTypeEditor::setupVehicleTypeTable()
{
  ///Ui::mdtClVehicleTypeEditor vte;

  // Setup main form widget
  ///vte.setupUi(mainSqlWidget());
  setMainTableUi<Ui::mdtClVehicleTypeEditor>();
  ///connect(this, SIGNAL(vehicleTypeEdited()), mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!setMainTable("VehicleType_tbl", "Vehicle types", database())){
    return false;
  }
  setWindowTitle(tr("Vehicle types edition"));

  return true;
}

bool mdtClVehicleTypeEditor::setupUnitTable()
{
  mdtSqlTableWidget *widget;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("VehicleType_Unit_view");
  relationInfo.addRelation("Id_PK", "VehicleType_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Units"))){
    return false;
  }
  /**
  // Add unit table
  if(!addChildTable("VehicleType_Unit_view", tr("Units"), database())){
    return false;
  }
  // Setup Unit <-> VehicleType relation
  if(!addRelation("Id_PK", "VehicleType_Unit_view", "VehicleType_Id_FK")){
    return false;
  }
  */
  // Get widget to continue setup
  widget = sqlTableWidget("VehicleType_Unit_view");
  Q_ASSERT(widget != 0);
  // Hide relation fields and PK
  widget->setColumnHidden("Type", true);
  widget->setColumnHidden("SubType", true);
  widget->setColumnHidden("SeriesNumber", true);
  widget->setColumnHidden("VehicleType_Unit_Id_PK", true);
  widget->setColumnHidden("VehicleType_Id_FK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("SchemaPosition", tr("Schema position"));
  // Enable sorting
  widget->addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  widget->sort();
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();

  return true;
}
