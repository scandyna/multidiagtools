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
#include "mdtTtTestItemEditor.h"
#include "ui_mdtTtTestItemEditor.h"
#include "mdtTtTestItem.h"
#include "mdtSqlForm.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlWindow.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QTableView>
#include <QVariant>
#include <QString>
#include <QPushButton>
#include <QSqlError>
#include <QModelIndex>
#include <QInputDialog>
#include <QMessageBox>

#include <QDebug>

mdtTtTestItemEditor::mdtTtTestItemEditor(QObject *parent, QSqlDatabase db)
 : mdtClEditor(parent, db)
{
}

void mdtTtTestItemEditor::setTestLink()
{
  mdtTtTestItem ti(database());
  QVariant testItemId;
  QVariant testLinkBusAId;
  QVariant testLinkBusBId;

  // Get current test ID
  testItemId = form()->currentData("TestItem_tbl", "Id_PK");
  if(testItemId.isNull()){
    return;
  }
  // Select test link to use on bus A
  testLinkBusAId = selectTestLink("BUSA");
  if(testLinkBusAId.isNull()){
    return;
  }
  // Select test link to use on bus B
  testLinkBusBId = selectTestLink("BUSB");
  if(testLinkBusBId.isNull()){
    return;
  }
  // Add to db
  if(!ti.setTestLink(testItemId, testLinkBusAId, testLinkBusBId)){
    pvLastError = ti.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  form()->select("TestItemLink_view");
}

void mdtTtTestItemEditor::generateTestNodeUnitSetup()
{
  mdtTtTestItem ti(database());
  QVariant testItemId;

  // Get current test ID
  testItemId = form()->currentData("TestItem_tbl", "Id_PK");
  if(testItemId.isNull()){
    return;
  }
  if(!ti.generateTestNodeUnitSetup(testItemId)){
    pvLastError = ti.lastError();
    displayLastError();
    return;
  }
  // Update GUI
  form()->select("TestItemNodeUnitSetup_view");
}

void mdtTtTestItemEditor::removeTestNodeUnitSetup()
{
  mdtSqlTableWidget *widget;
  mdtTtTestItem ti(database());
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
  if(!ti.removeTestNodeUnitSetups(indexes)){
    pvLastError = ti.lastError();
    displayLastError();
    return;
  }
  // Update connections table
  form()->select("TestItemNodeUnitSetup_view");
}

QVariant mdtTtTestItemEditor::selectTestLink(const QString & bus)
{
  mdtSqlSelectionDialog selectionDialog;
  QSqlError sqlError;
  QSqlQueryModel model;
  QString sql;

  // Setup model
  sql = "SELECT * FROM TestLink_view WHERE Bus = '" + bus + "'";
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get list of test links."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestEditor");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select link for bus '") + bus + tr("'"));
  selectionDialog.setModel(&model, false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("TestConnection_Id_FK", true);
  selectionDialog.setColumnHidden("DutConnection_Id_FK", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

bool mdtTtTestItemEditor::setupTables()
{
  if(!setupTestItemTable()){
    return false;
  }
  if(!setupTestLinkTable()){
    return false;
  }
  if(!setupTestNodeUnitSetupTable()){
    return false;
  }
  return true;
}

bool mdtTtTestItemEditor::setupTestItemTable() 
{
  Ui::mdtTtTestItemEditor tie;

  // Setup main form widget
  tie.setupUi(form()->mainSqlWidget());
  // Setup form
  if(!form()->setTable("TestItem_tbl", "Test item", database())){
    return false;
  }
  sqlWindow()->enableNavigation();
  sqlWindow()->enableEdition();
  sqlWindow()->resize(800, 500);
  sqlWindow()->setWindowTitle(tr("Test item edition"));
  // Force a update
  form()->mainSqlWidget()->setCurrentIndex(form()->mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtTestItemEditor::setupTestLinkTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbSetTestLink;

  if(!form()->addChildTable("TestItemLink_view", tr("Link"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "TestItemLink_view", "TestItemId")){
    return false;
  }
  widget = form()->sqlTableWidget("TestItemLink_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  ///widget->setColumnHidden("", true);
  // Set fields a user friendly name
  ///widget->setHeaderData("", tr(""));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  pbSetTestLink = new QPushButton(tr("Set test link ..."));
  connect(pbSetTestLink, SIGNAL(clicked()), this, SLOT(setTestLink()));
  widget->addWidgetToLocalBar(pbSetTestLink);
  widget->addStretchToLocalBar();

  return true;
}

bool mdtTtTestItemEditor::setupTestNodeUnitSetupTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbGenerateTestNodeUnitSetup;
  QPushButton *pbRemoveTestNodeUnitSetup;

  if(!form()->addChildTable("TestItemNodeUnitSetup_view", tr("Node unit setup"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "TestItemNodeUnitSetup_view", "TestItem_Id_FK")){
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
  pbGenerateTestNodeUnitSetup = new QPushButton(tr("Generate setup"));
  connect(pbGenerateTestNodeUnitSetup, SIGNAL(clicked()), this, SLOT(generateTestNodeUnitSetup()));
  widget->addWidgetToLocalBar(pbGenerateTestNodeUnitSetup);
  pbRemoveTestNodeUnitSetup = new QPushButton(tr("Remove setup item"));
  connect(pbRemoveTestNodeUnitSetup, SIGNAL(clicked()), this, SLOT(removeTestNodeUnitSetup()));
  widget->addWidgetToLocalBar(pbRemoveTestNodeUnitSetup);
  widget->addStretchToLocalBar();

  return true;
}

