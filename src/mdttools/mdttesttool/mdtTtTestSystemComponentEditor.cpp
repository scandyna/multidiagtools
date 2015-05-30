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
#include "mdtTtTestSystemComponentEditor.h"
#include "mdtTtTestSystemComponent.h"
#include "ui_mdtTtTestSystemComponentEditor.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtSqlRelationInfo.h"
#include <QPushButton>
#include <QLocale>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMessageBox>

mdtTtTestSystemComponentEditor::mdtTtTestSystemComponentEditor(QWidget* parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtTestSystemComponentEditor::setupTables()
{
  if(!setupTestSystemComponentTable()){
    return false;
  }
  if(!setupTestSystemAssignationTable()){
    return false;
  }
  return true;
}

void mdtTtTestSystemComponentEditor::addTestSystemAssignation()
{
  mdtTtTestSystemComponent tsc(database());
  mdtSqlSelectionDialog dialog(this);
  QString sql;
  QVariant testSystemComponentId;
  mdtSqlTableSelection s;

  testSystemComponentId = currentData("TestSystemComponent_tbl", "Id_PK");
  if(testSystemComponentId.isNull()){
    return;
  }
  // Build SQL query
  sql = "SELECT * FROM TestSystem_tbl";
  sql += " WHERE Id_PK NOT IN (";
  sql += " SELECT TestSystem_Id_FK FROM TestSystem_TestSystemComponent_tbl";
  sql += "  WHERE TestSystemComponent_Id_FK = " + testSystemComponentId.toString();
  sql += ")";
  // Setup and show dialog
  dialog.setWindowTitle(tr("Test system selection"));
  dialog.setMessage(tr("Select test system to assign:"));
  dialog.setQuery(sql, database(), false);
  dialog.setColumnHidden("Id_PK", true);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add assignation with selected test system
  s = dialog.selection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  // Setup data
  mdtTtTestSystemAssignationData data;
  data.testSystemId = s.data(0, "Id_PK");
  data.testSystemComponentId = testSystemComponentId;
  // Add to database
  if(!tsc.addTestSystemAssignation(data)){
    pvLastError = tsc.lastError();
    displayLastError();
    return;
  }
  // Update UI
  ///select("TestSystemOfComponent_view");
  auto * widget = sqlTableWidget("TestSystemOfComponent_view");
  Q_ASSERT(widget != nullptr);
  widget->select();
  widget->resizeViewToContents();
}

void mdtTtTestSystemComponentEditor::removeTestSystemAssignation()
{
  mdtSqlTableWidget *widget;
  mdtTtTestSystemComponent tsc(database());
  QMessageBox msgBox(this);
  QStringList fields;

  mdtSqlTableSelection s;

  widget = sqlTableWidget("TestSystemOfComponent_view");
  Q_ASSERT(widget != 0);
  // Get selection
  fields << "TestSystem_Id_FK" << "TestSystemComponent_Id_FK";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove test system assignations."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove from database
  if(!tsc.removeTestSystemAssignation(s)){
    pvLastError = tsc.lastError();
    displayLastError();
    return;
  }
  // Update UI
  select("TestSystemOfComponent_view");
}

bool mdtTtTestSystemComponentEditor::setupTestSystemComponentTable()
{
  setMainTableUi<Ui::mdtTtTestSystemComponentEditor>();
  if(!setMainTable("TestSystemComponent_tbl", "Test system component", database())){
    return false;
  }
  setWindowTitle(tr("Test system component edition"));

  return true;
}

bool mdtTtTestSystemComponentEditor::setupTestSystemAssignationTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pb;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("TestSystemOfComponent_view");
  relationInfo.addRelation("Id_PK", "TestSystemComponent_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Test system assignations"))){
    return false;
  }
  widget = sqlTableWidget("TestSystemOfComponent_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("TestSystem_Id_FK", true);
  widget->setColumnHidden("TestSystemComponent_Id_FK", true);
  widget->setColumnHidden("Id_PK", true);
  // Set fields some user friendly name

  // Set some attributes on table view
  ///widget->addColumnToSortOrder("Type_Code_FK", Qt::AscendingOrder);
  ///widget->sort();
  widget->resizeViewToContents();
  // Add buttons
  pb = new QPushButton(tr("Assign ..."));
  pb->setIcon(QIcon::fromTheme("list-add"));
  connect(pb, SIGNAL(clicked()), this, SLOT(addTestSystemAssignation()));
  widget->addWidgetToLocalBar(pb);
  pb = new QPushButton(tr("Remove assignation ..."));
  pb->setIcon(QIcon::fromTheme("list-remove"));
  connect(pb, SIGNAL(clicked()), this, SLOT(removeTestSystemAssignation()));
  widget->addWidgetToLocalBar(pb);
  widget->addStretchToLocalBar();

  return true;
}
