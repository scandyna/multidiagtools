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
#include "mdtTtTestNodeUnitEditor.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtError.h"
#include <QWidget>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QSqlError>

#include <QDebug>

mdtTtTestNodeUnitEditor::mdtTtTestNodeUnitEditor(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtTestNodeUnitEditor::setupTables()
{
  if(!setupTestNodeUnitTable()){
    return false;
  }
  return true;
}

bool mdtTtTestNodeUnitEditor::setupTestNodeUnitTable() 
{
  // Setup main form widget
  setupUi(mainSqlWidget());
  setWindowTitle(tr("Test node unit edition"));
  // Setup form
  if(!setMainTable("TestNodeUnit_tbl", "Test node unit", database())){
    return false;
  }
  // Setup widget mapping
  if(!setupTestNodeTableWidgetMapping()){
    return false;
  }
  if(!setupUnitTableWidgetMapping()){
    return false;
  }
  if(!setupUnitTableWidgetMapping()){
    return false;
  }
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtTestNodeUnitEditor::setupTestNodeTableWidgetMapping()
{
  /**
  QSqlTableModel *testNodeUnitModel;
  QSqlTableModel *testNodeModel;
  QDataWidgetMapper *testNodeMapper;
  mdtSqlRelation *relation;

  // Get main table model and create unit model
  testNodeUnitModel = model("TestNodeUnit_tbl");
  Q_ASSERT(testNodeUnitModel != 0);
  testNodeModel = new QSqlTableModel(this, database());
  testNodeModel->setTable("TestNode_tbl");
  if(!testNodeModel->select()){
    QSqlError sqlError = testNodeModel->lastError();
    lastErrorW().setError(tr("Cannot select data in TestNode_tbl"), mdtError::Error);
    lastErrorW().setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(lastErrorW(), "mdtTtTestNodeUnitEditor");
    lastErrorW().commit();
    return false;
  }
  // Setup widget mapping
  testNodeMapper = new QDataWidgetMapper(this);
  testNodeMapper->setModel(testNodeModel);
  testNodeMapper->addMapping(leTestNodeType, testNodeModel->fieldIndex("Type"));
  testNodeMapper->addMapping(leTestNodeSubType, testNodeModel->fieldIndex("SubType"));
  testNodeMapper->addMapping(leTestNodeSeriesNumber, testNodeModel->fieldIndex("SeriesNumber"));
  // Setup relation
  relation = new mdtSqlRelation(this);
  relation->setParentModel(testNodeUnitModel);
  relation->setChildModel(testNodeModel);
  if(!relation->addRelation("TestNode_Id_FK", "VehicleType_Id_FK_PK", false)){
    return false;
  }
  connect(mainSqlWidget(), SIGNAL(currentRowChanged(int)), relation, SLOT(setParentCurrentIndex(int)));
  connect(relation, SIGNAL(childModelFilterApplied()), testNodeMapper, SLOT(toFirst()));
  connect(relation, SIGNAL(childModelIsEmpty()), testNodeMapper, SLOT(revert()));
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());
  */

  return true;
}

bool mdtTtTestNodeUnitEditor::setupUnitTableWidgetMapping()
{
  QSqlTableModel *testNodeUnitModel;
  QSqlTableModel *baseUnitModel;
  QDataWidgetMapper *baseUnitMapper;
  mdtSqlRelation *relation;

  // Get main table model and create unit model
  testNodeUnitModel = model("TestNodeUnit_tbl");
  Q_ASSERT(testNodeUnitModel != 0);
  baseUnitModel = new QSqlTableModel(this, database());
  baseUnitModel->setTable("Unit_tbl");
  if(!baseUnitModel->select()){
    QSqlError sqlError = baseUnitModel->lastError();
    lastErrorW().setError(tr("Cannot select data in Unit_tbl"), mdtError::Error);
    lastErrorW().setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(lastErrorW(), "mdtTtTestNodeUnitEditor");
    lastErrorW().commit();
    return false;
  }
  // Setup widget mapping
  baseUnitMapper = new QDataWidgetMapper(this);
  baseUnitMapper->setModel(baseUnitModel);
  baseUnitMapper->addMapping(leSchemaPosition, baseUnitModel->fieldIndex("SchemaPosition"));
  // Setup relation
  relation = new mdtSqlRelation(this);
  relation->setParentModel(testNodeUnitModel);
  relation->setChildModel(baseUnitModel);
  if(!relation->addRelation("Unit_Id_FK_PK", "Id_PK", false)){
    return false;
  }
  connect(mainSqlWidget(), SIGNAL(currentRowChanged(int)), relation, SLOT(setParentCurrentIndex(int)));
  connect(relation, SIGNAL(childModelFilterApplied()), baseUnitMapper, SLOT(toFirst()));
  connect(relation, SIGNAL(childModelIsEmpty()), baseUnitMapper, SLOT(revert()));
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtTestNodeUnitEditor::setupTypeTableWidgetMapping()
{
  /**
  QSqlTableModel *testNodeUnitModel;
  QSqlTableModel *typeModel;
  QDataWidgetMapper *typeMapper;
  mdtSqlRelation *relation;

  // Get main table model and create unit model
  testNodeUnitModel = model("TestNodeUnit_tbl");
  Q_ASSERT(testNodeUnitModel != 0);
  typeModel = new QSqlTableModel(this, database());
  typeModel->setTable("TestNodeUnitType_tbl");
  if(!typeModel->select()){
    QSqlError sqlError = typeModel->lastError();
    lastErrorW().setError(tr("Cannot select data in TestNodeUnitType_tbl"), mdtError::Error);
    lastErrorW().setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(lastErrorW(), "mdtTtTestNodeUnitEditor");
    lastErrorW().commit();
    return false;
  }
  // Setup widget mapping
  typeMapper = new QDataWidgetMapper(this);
  typeMapper->setModel(typeModel);
  typeMapper->addMapping(leSchemaPosition, typeModel->fieldIndex("SchemaPosition"));
  // Setup relation
  relation = new mdtSqlRelation(this);
  relation->setParentModel(testNodeUnitModel);
  relation->setChildModel(typeModel);
  if(!relation->addRelation("Unit_Id_FK_PK", "Id_PK", false)){
    return false;
  }
  connect(mainSqlWidget(), SIGNAL(currentRowChanged(int)), relation, SLOT(setParentCurrentIndex(int)));
  connect(relation, SIGNAL(childModelFilterApplied()), baseUnitMapper, SLOT(toFirst()));
  connect(relation, SIGNAL(childModelIsEmpty()), baseUnitMapper, SLOT(revert()));
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());
*/
  return true;
}
