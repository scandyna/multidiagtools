/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtClApplicationWidgets.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
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
#include <QPushButton>

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

void mdtClVehicleTypeEditor::editSelectedUnit()
{
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;

  widget = sqlTableWidget("VehicleType_Unit_view");
  Q_ASSERT(widget != 0);
  s = widget->currentSelection("Unit_Id_FK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  mdtClApplicationWidgets::editUnit(s.data(0, "Unit_Id_FK"));
}

QVariant mdtClVehicleTypeEditor::currentVehicleTypeId()
{
  return currentData("VehicleType_tbl", "Id_PK");
}

bool mdtClVehicleTypeEditor::setupVehicleTypeTable()
{
  setMainTableUi<Ui::mdtClVehicleTypeEditor>();
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
  QPushButton *pb;

  relationInfo.setChildTableName("VehicleType_Unit_view");
  relationInfo.addRelation("Id_PK", "VehicleType_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Units"))){
    return false;
  }
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
  // Setup unit edition button
  pb = new QPushButton(tr("Edit ..."));
  connect(pb, SIGNAL(clicked()), this, SLOT(editSelectedUnit()));
  widget->addWidgetToLocalBar(pb);
  connect(widget->tableView(), SIGNAL(doubleClicked(const QModelIndex&)),this, SLOT(editSelectedUnit()));
  // Set some attributes on table view
  widget->addStretchToLocalBar();
  widget->resizeViewToContents();

  return true;
}
