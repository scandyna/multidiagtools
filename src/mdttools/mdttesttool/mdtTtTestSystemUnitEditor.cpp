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
#include "mdtTtTestSystemUnitEditor.h"
#include "ui_mdtTtTestSystemUnitEditor.h"
#include "mdtAbstractSqlTableController.h"
#include "mdtSqlDataWidgetController.h"
#include "mdtUiMessageHandler.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QPushButton>
#include <QSqlError>
#include <QMessageBox>
#include <memory>

#include <QDebug>

mdtTtTestSystemUnitEditor::mdtTtTestSystemUnitEditor(QWidget* parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtTestSystemUnitEditor::setupTables()
{
  if(!setupTestSystemUnitTables()){
    return false;
  }
  return true;
}

void mdtTtTestSystemUnitEditor::setTestSystemComponent()
{
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  ///QVariant tsuId;
  QString sql;

  // Check that we are in a existing (or freshly inserted) row
  if(currentRow() < 0){
    return;
  }
  // Get current TS unit ID
  /**
  tsuId = currentData("TestSystemUnit_tbl", "Unit_Id_FK_PK");
  if(tsuId.isNull()){
    return;
  }
  */
  /// \todo checks !
  
  // Let user select TSC
  sql =  "SELECT TSC.Id_PK AS TSCID, TSC.NameEN AS TSCNameEN, TSC.Alias, TS.NameEN AS TSNameEN\n"\
         "FROM TestSystemComponent_tbl TSC\n"\
         " JOIN TestSystem_TestSystemComponent_tbl TS_TSC ON TS_TSC.TestSystemComponent_Id_FK = TSC.Id_PK\n"\
         " JOIN TestSystem_tbl TS ON TS.Id_PK = TS_TSC.TestSystem_Id_FK\n";
  
  selectionDialog.setMessage(tr("Select test system component:"));
  selectionDialog.setWindowTitle(tr("Test system component selection"));
  selectionDialog.setQuery(sql, database());
  selectionDialog.setColumnHidden("TSCID", true);
  selectionDialog.addColumnToSortOrder("TSNameEN", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("TSCNameEN", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 500);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("TSCID");
  if(s.isEmpty()){
    return;
  }
  // Set selected test system component
  if(!setCurrentData("TestSystemUnit_tbl", "TestSystemComponent_Id_FK", s.data(0, "TSCID"), false)){
    displayLastError();
    return;
  }
}

bool mdtTtTestSystemUnitEditor::setupTestSystemUnitTables()
{
  Ui::mdtTtTestSystemUnitEditor tsue;
  std::shared_ptr<mdtSqlDataWidgetController> uController;
  std::shared_ptr<mdtSqlDataWidgetController> tsuController;
  std::shared_ptr<mdtSqlDataWidgetController> tscController;
  std::shared_ptr<mdtSqlDataWidgetController> baseArticleController;
  std::shared_ptr<mdtUiMessageHandler> uiMessageHandler;
  mdtSqlRelationInfo relationInfo;
  
  /// \todo Use mdtSqlSchemaTable for better mapping

  // Setup main form widget part
  setMainTableUi(tsue);
  uiMessageHandler = std::make_shared<mdtUiMessageHandler>();
  /*
   * Setup test system unit and unit part.
   * Here, we must have Unit_tbl as main controller,
   * main reason is for PK auto generation
   * (We cannot have */
  // Setup test system unit table controller
  tsuController = mainTableController<mdtSqlDataWidgetController>();
  Q_ASSERT(tsuController);
  tsuController->setTableName("TestSystemUnit_tbl", database(), tr("Test system unit"));
  tsuController->setMessageHandler(uiMessageHandler);
  // Setup unit table controller
  relationInfo.setChildTableName("Unit_tbl");
  relationInfo.addRelation("Unit_Id_FK_PK", "Id_PK", true);
  relationInfo.setRelationType(mdtSqlRelationInfo::OneToOne);
  if(!tsuController->addChildController<mdtSqlDataWidgetController>(relationInfo, tr("Test system unit"))){
    pvLastError = tsuController->lastError();
    return false;
  }
  uController = tsuController->childController<mdtSqlDataWidgetController>("Unit_tbl");
  Q_ASSERT(uController);
  uController->setMessageHandler(uiMessageHandler);
  // Setup mapping for TestSystemUnit_tbl part
  if(!tsuController->addMapping(tsue.fld_IoPosition, "IoPosition")){
    pvLastError = tsuController->lastError();
    return false;
  }
  if(!tsuController->addMapping(tsue.fld_CalibrationDate, "CalibrationDate")){
    pvLastError = tsuController->lastError();
    return false;
  }

  // Setup mapping for Unit_tbl part
  if(!uController->addMapping(tsue.fld_SchemaPosition, "SchemaPosition")){
    pvLastError = uController->lastError();
    return false;
  }
  /*
   * Setup test system component widget mapping
   */
  // Add a new child controller to test system unit controller
  Q_ASSERT(tsuController);
  relationInfo.clear();
  relationInfo.setChildTableName("TestSystemComponent_tbl");
  relationInfo.addRelation("TestSystemComponent_Id_FK", "Id_PK", false);
  if(!tsuController->addChildController<mdtSqlDataWidgetController>(relationInfo, tr("Test system component"))){
    pvLastError = tsuController->lastError();
    return false;
  }
  // Get freshly added controller and map widgets to it
  tscController = tsuController->childController<mdtSqlDataWidgetController>("TestSystemComponent_tbl");
  Q_ASSERT(tscController);
  tscController->setMessageHandler(uiMessageHandler);
  connect(tsue.pbSelectTsc, SIGNAL(clicked()), this, SLOT(setTestSystemComponent()));
  tscController->addMapping(tsue.fld_TscName, "NameEN");
  tscController->addMapping(tsue.fld_TscAlias, "Alias");
  /*
   * Setup base article widget mapping
   */
  // Add a new child controller to test system unit controller
  
  // Get freshly added controller and map widgets to it
  
  return true;
}
