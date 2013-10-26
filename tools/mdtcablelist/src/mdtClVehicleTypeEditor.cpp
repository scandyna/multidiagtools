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
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>

#include <QDebug>

mdtClVehicleTypeEditor::mdtClVehicleTypeEditor(QObject *parent, const QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  pvForm = new mdtSqlFormWindow;
}

mdtClVehicleTypeEditor::~mdtClVehicleTypeEditor()
{
  delete pvForm;
}

bool mdtClVehicleTypeEditor::setupTables()
{
  QSqlError sqlError;

  if(!setupVehicleTypeTable()){
    return false;
  }
  if(!setupUnitTable()){
    return false;
  }

  return true;
}

mdtSqlFormWindow *mdtClVehicleTypeEditor::form()
{
  Q_ASSERT(pvForm != 0);

  return pvForm;
}

QVariant mdtClVehicleTypeEditor::currentVehicleTypeId()
{
  return pvForm->currentData("VehicleType_tbl", "Id_PK");
}

bool mdtClVehicleTypeEditor::setupVehicleTypeTable()
{
  Ui::mdtClVehicleTypeEditor vte;

  // Setup main form widget
  vte.setupUi(pvForm->mainSqlWidget());
  connect(this, SIGNAL(vehicleTypeEdited()), pvForm->mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!pvForm->setTable("VehicleType_tbl", "Vehicle types", pvDatabase)){
    return false;
  }
  pvForm->sqlWindow()->enableNavigation();
  pvForm->sqlWindow()->enableEdition();
  pvForm->sqlWindow()->resize(800, 500);
  pvForm->sqlWindow()->setWindowTitle(tr("Vehicle types edition"));
  // Force a update
  pvForm->mainSqlWidget()->setCurrentIndex(pvForm->mainSqlWidget()->currentRow());

  return true;
}

bool mdtClVehicleTypeEditor::setupUnitTable()
{
  mdtSqlTableWidget *widget;

  // Add unit table
  if(!pvForm->addChildTable("VehicleType_Unit_view", tr("Units"), pvDatabase)){
    return false;
  }
  // Setup Unit <-> VehicleType relation
  if(!pvForm->addRelation("Id_PK", "VehicleType_Unit_view", "VehicleType_Id_FK")){
    return false;
  }
  // Get widget to continue setup
  widget = pvForm->sqlTableWidget("VehicleType_Unit_view");
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

  return true;
}
