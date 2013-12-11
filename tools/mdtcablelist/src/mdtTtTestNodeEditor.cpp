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
#include "mdtTtTestNodeEditor.h"
#include "ui_mdtTtTestNodeEditor.h"
#include "mdtSqlForm.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlWindow.h"
#include "mdtSqlRelation.h"
#include "mdtSqlSelectionDialog.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QVariant>
#include <QString>

#include <QDebug>

mdtTtTestNodeEditor::mdtTtTestNodeEditor(QObject *parent, QSqlDatabase db)
 : mdtClEditor(parent, db)
{
}

void mdtTtTestNodeEditor::setBaseVehicleType()
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlQueryModel model;
  QString sql;
  QVariant vehicleTypeId;

  // Setup vehicle type selection dialog and show it to user
  sql = "SELECT * FROM VehicleType_tbl;";
  model.setQuery(sql, database());
  selectionDialog.setMessage("Please select vehicle type.");
  selectionDialog.setModel(&model, false);
  ///selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("SubType", tr("Variant"));
  ///selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);
  vehicleTypeId = selectionDialog.selectionResult().at(0);
  qDebug() << "VHC ID: " << vehicleTypeId;
  if(!form()->setCurrentData("TestNode_tbl", "VehicleType_Id_FK_PK", vehicleTypeId)){
    return;
  }
}

bool mdtTtTestNodeEditor::setupTables()
{
  if(!setupTestNodeTable()){
    return false;
  }
  return true;
}

bool mdtTtTestNodeEditor::setupTestNodeTable()
{
  Ui::mdtTtTestNodeEditor tne;
  QSqlTableModel *testNodeModel;
  QDataWidgetMapper *baseVehicleTypeMapper;
  QSqlTableModel *baseVehicleTypeModel;
  mdtSqlRelation *baseVehicleTypeRelation;

  // Setup main form widget
  tne.setupUi(form()->mainSqlWidget());
  connect(tne.pbSetVehicleType, SIGNAL(clicked()), this, SLOT(setBaseVehicleType()));
  ///connect(this, SIGNAL(unitEdited()), form()->mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!form()->setTable("TestNode_tbl", "Test node", database())){
    return false;
  }
  sqlWindow()->enableNavigation();
  sqlWindow()->enableEdition();
  sqlWindow()->resize(800, 500);
  sqlWindow()->setWindowTitle(tr("Test node edition"));
  /*
   * Setup base unit widget mapping
   */
  testNodeModel = form()->model("TestNode_tbl");
  Q_ASSERT(testNodeModel != 0);
  // Setup base article model
  baseVehicleTypeModel = new QSqlTableModel(this, database());
  baseVehicleTypeModel->setTable("VehicleType_tbl");
  if(!baseVehicleTypeModel->select()){
    return false;
  }
  // Setup base article widget mapper
  baseVehicleTypeMapper = new QDataWidgetMapper(this);
  baseVehicleTypeMapper->setModel(baseVehicleTypeModel);
  baseVehicleTypeMapper->addMapping(tne.leType, baseVehicleTypeModel->fieldIndex("Type"));
  baseVehicleTypeMapper->addMapping(tne.leSubType, baseVehicleTypeModel->fieldIndex("SubType"));
  baseVehicleTypeMapper->addMapping(tne.leSeriesNumber, baseVehicleTypeModel->fieldIndex("SeriesNumber"));
  // Setup base article relation
  baseVehicleTypeRelation = new mdtSqlRelation(this);
  baseVehicleTypeRelation->setParentModel(testNodeModel);
  baseVehicleTypeRelation->setChildModel(baseVehicleTypeModel);
  if(!baseVehicleTypeRelation->addRelation("VehicleType_Id_FK_PK", "Id_PK", false)){
    return false;
  }
  connect(form()->mainSqlWidget(), SIGNAL(currentRowChanged(int)), baseVehicleTypeRelation, SLOT(setParentCurrentIndex(int)));
  connect(baseVehicleTypeRelation, SIGNAL(childModelFilterApplied()), baseVehicleTypeMapper, SLOT(toFirst()));
  connect(baseVehicleTypeRelation, SIGNAL(childModelIsEmpty()), baseVehicleTypeMapper, SLOT(revert()));
  // Force a update
  form()->mainSqlWidget()->setCurrentIndex(form()->mainSqlWidget()->currentRow());

  return true;
}
