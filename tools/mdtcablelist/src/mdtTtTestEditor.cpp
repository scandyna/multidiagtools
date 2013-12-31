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
#include "mdtTtTestEditor.h"
#include "ui_mdtTtTestEditor.h"
#include "mdtTtTest.h"
#include "mdtTtTestItemEditor.h"
#include "mdtSqlFormOld.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlWindow.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTableView>
///#include <QDataWidgetMapper>
#include <QVariant>
#include <QString>
#include <QPushButton>
#include <QSqlError>
#include <QModelIndex>
#include <QInputDialog>
#include <QMessageBox>

#include <QDebug>

mdtTtTestEditor::mdtTtTestEditor(QObject *parent, QSqlDatabase db)
 : mdtClEditor(parent, db)
{
}

void mdtTtTestEditor::addTestItem() 
{
  mdtTtTest t(database());
  QVariant testId;
  QVariant testLinkBusAId;
  QVariant testLinkBusBId;
  QVariant expectedValue;

  // Get current test ID
  testId = form()->currentData("Test_tbl", "Id_PK");
  if(testId.isNull()){
    return;
  }
  // Select test link to use on bus A
  /**
  testLinkBusAId = selectTestLink(tr("Select BUS A test link"));
  if(testLinkBusAId.isNull()){
    return;
  }
  // Select test link to use on bus B
  testLinkBusBId = selectTestLink(tr("Select BUS B test link"));
  if(testLinkBusBId.isNull()){
    return;
  }
  */
  // Set a expectedValue
  expectedValue = 1.2;
  // Add to db
  if(!t.addTestItem(testId, testLinkBusAId, testLinkBusBId, expectedValue)){
    pvLastError = t.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  form()->select("TestItemLink_view");
  form()->select("TestItemNode_view");
}

void mdtTtTestEditor::editTestItem()
{
  mdtTtTestItemEditor tie(this, database());

  tie.setupAsDialog();
  tie.exec();
}

void mdtTtTestEditor::removeTestItem() 
{
  mdtSqlTableWidget *widget;
  mdtTtTest t(database());
  QMessageBox msgBox;
  QModelIndexList indexes;

  widget = form()->sqlTableWidget("TestItemLink_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("TestItemId");
  if(indexes.size() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove selected test items."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!t.removeTestItems(indexes)){
    pvLastError = t.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  form()->select("TestItemLink_view");
  form()->select("TestItemNode_view");
}

void mdtTtTestEditor::generateTestNodeUnitSetupList()
{
  mdtTtTest t(database());
  QVariant testId;

  // Get current test ID
  testId = form()->currentData("Test_tbl", "Id_PK");
  if(testId.isNull()){
    return;
  }
  if(!t.generateTestNodeUnitSetupForTest(testId)){
    pvLastError = t.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  form()->select("TestItemNodeUnitSetup_view");
}

/**
void mdtTtTestEditor::removeTestNodeUnitSetup()
{
  mdtSqlTableWidget *widget;
  mdtTtTest t(database());
  QMessageBox msgBox;
  QModelIndexList indexes;

  widget = form()->sqlTableWidget("TestItemNodeUnitSetup_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("Id_PK");
  if(indexes.size() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove selected setup items."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!t.removeTestNodeUnitSetups(indexes)){
    pvLastError = t.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  form()->select("TestItemNodeUnitSetup_view");
}
*/

bool mdtTtTestEditor::setupTables() 
{
  if(!setupTestTable()){
    return false;
  }
  if(!setupTestLinkTable()){
    return false;
  }
  if(!setupTestNodeUnitSetupTable()){
    return false;
  }
  if(!setupTestNodeTable()){
    return false;
  }
  return true;
}

bool mdtTtTestEditor::setupTestTable()
{
  Ui::mdtTtTestEditor te;

  // Setup main form widget
  te.setupUi(form()->mainSqlWidget());
  connect(te.pbAddTestItem, SIGNAL(clicked()), this, SLOT(addTestItem()));
  connect(te.pbRemoveTestItem, SIGNAL(clicked()), this, SLOT(removeTestItem()));
  connect(te.pbGenerateNodeUnitSetup, SIGNAL(clicked()), this, SLOT(generateTestNodeUnitSetupList()));
  // Setup form
  if(!form()->setTable("Test_tbl", "Test", database())){
    return false;
  }
  sqlWindow()->enableNavigation();
  sqlWindow()->enableEdition();
  sqlWindow()->resize(800, 500);
  sqlWindow()->setWindowTitle(tr("Test edition"));
  // Force a update
  form()->mainSqlWidget()->setCurrentIndex(form()->mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtTestEditor::setupTestLinkTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbEditTestItem;

  if(!form()->addChildTable("TestItemLink_view", tr("Links"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "TestItemLink_view", "Test_Id_FK")){
    return false;
  }
  widget = form()->sqlTableWidget("TestItemLink_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Test_Id_FK", true);
  widget->setColumnHidden("TestLinkBusA_Id_FK", true);
  widget->setColumnHidden("TestLinkBusB_Id_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("ExpectedValue", tr("Value\nExpected"));
  widget->setHeaderData("TestConnectorNameBusA", tr("Test\nConnector\nBus A"));
  widget->setHeaderData("TestConnectorNameBusB", tr("Test\nConnector\nBus B"));
  widget->setHeaderData("TestContactNameBusA", tr("Test\nContact\nBus A"));
  widget->setHeaderData("TestContactNameBusB", tr("Test\nContact\nBus B"));
  widget->setHeaderData("DutUnitSchemaPositionBusA", tr("DUT BUS A"));
  widget->setHeaderData("DutUnitSchemaPositionBusB", tr("DUT BUS B"));
  widget->setHeaderData("DutConnectorNameBusA", tr("DUT\nConnector\nBus A"));
  widget->setHeaderData("DutConnectorNameBusB", tr("DUT\nConnector\nBus B"));
  widget->setHeaderData("DutContactNameBusA", tr("DUT\nContact\nBus A"));
  widget->setHeaderData("DutContactNameBusB", tr("DUT\nContact\nBus B"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  pbEditTestItem = new QPushButton(tr("Edit item"));
  connect(pbEditTestItem, SIGNAL(clicked()), this, SLOT(editTestItem()));
  widget->addWidgetToLocalBar(pbEditTestItem);
  widget->addStretchToLocalBar();


  return true;
}

bool mdtTtTestEditor::setupTestNodeUnitSetupTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbRemoveTestNodeUnitSetup;

  if(!form()->addChildTable("TestItemNodeUnitSetup_view", tr("Node unit setup"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "TestItemNodeUnitSetup_view", "Test_Id_FK")){
    return false;
  }
  widget = form()->sqlTableWidget("TestItemNodeUnitSetup_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  ///widget->setColumnHidden("", true);
  // Set fields a user friendly name
  ///widget->setHeaderData("", tr(""));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  pbRemoveTestNodeUnitSetup = new QPushButton(tr("Remove setup item"));
  connect(pbRemoveTestNodeUnitSetup, SIGNAL(clicked()), this, SLOT(removeTestNodeUnitSetup()));
  widget->addWidgetToLocalBar(pbRemoveTestNodeUnitSetup);
  widget->addStretchToLocalBar();

  return true;
}

bool mdtTtTestEditor::setupTestNodeTable()
{
  mdtSqlTableWidget *widget;

  if(!form()->addChildTable("TestItemNode_view", tr("Used nodes"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "TestItemNode_view", "Test_Id_FK")){
    return false;
  }
  widget = form()->sqlTableWidget("TestItemNode_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  ///widget->setColumnHidden("", true);
  // Set fields a user friendly name
  ///widget->setHeaderData("", tr(""));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons

  return true;
}
