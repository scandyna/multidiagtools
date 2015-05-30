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
#include "mdtTtTestSystemEditor.h"
#include "ui_mdtTtTestSystemEditor.h"
#include "mdtTtTestSystemComponent.h"
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

mdtTtTestSystemEditor::mdtTtTestSystemEditor(QWidget* parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtTestSystemEditor::setupTables()
{
  if(!setupTestSystemTable()){
    return false;
  }
  if(!setupTestSystemComponentAssignationTable()){
    return false;
  }
  return true;
}

bool mdtTtTestSystemEditor::setupTestSystemTable()
{
  setMainTableUi<Ui::mdtTtTestSystemEditor>();
  if(!setMainTable("TestSystem_tbl", "Test system", database())){
    return false;
  }
  setWindowTitle(tr("Test system edition"));

  return true;
}

void mdtTtTestSystemEditor::addComponentAssignation()
{
  mdtTtTestSystemComponent tsc(database());
  mdtSqlSelectionDialog dialog(this);
  QString sql;
  mdtSqlTableSelection s;
  mdtTtTestSystemAssignationData data;

  // Get current test system ID
  data.testSystemId = currentData("TestSystem_tbl", "Id_PK");
  if(data.testSystemId.isNull()){
    return;
  }
  // Build SQL query
  sql = "SELECT * FROM TestSystemComponent_tbl";
  sql += " WHERE Id_PK NOT IN (";
  sql += " SELECT TestSystemComponent_Id_FK FROM TestSystem_TestSystemComponent_tbl";
  sql += "  WHERE TestSystem_Id_FK = " + data.testSystemId.toString();
  sql += ")";
  // Setup and show dialog
  dialog.setWindowTitle(tr("Test system component selection"));
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
  data.testSystemComponentId = s.data(0, "Id_PK");
  // Add to database
  if(!tsc.addTestSystemAssignation(data)){
    pvLastError = tsc.lastError();
    displayLastError();
    return;
  }
  // Update UI
  ///select("TestSystemOfComponent_view");
  auto * widget = sqlTableWidget("TestSystemComponent_view");
  Q_ASSERT(widget != nullptr);
  widget->select();
  widget->resizeViewToContents();
}

void mdtTtTestSystemEditor::removeComponentAssignation()
{
  mdtSqlTableWidget *widget;
  mdtTtTestSystemComponent tsc(database());
  QMessageBox msgBox(this);
  QStringList fields;

  mdtSqlTableSelection s;

  widget = sqlTableWidget("TestSystemComponent_view");
  Q_ASSERT(widget != 0);
  // Get selection
  fields << "TestSystem_Id_FK" << "TestSystemComponent_Id_FK";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove test system component assignations."));
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
  select("TestSystemComponent_view");
}

bool mdtTtTestSystemEditor::setupTestSystemComponentAssignationTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pb;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("TestSystemComponent_view");
  relationInfo.addRelation("Id_PK", "TestSystem_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Test system components assignations"))){
    return false;
  }
  widget = sqlTableWidget("TestSystemComponent_view");
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
  connect(pb, SIGNAL(clicked()), this, SLOT(addComponentAssignation()));
  widget->addWidgetToLocalBar(pb);
  pb = new QPushButton(tr("Remove assignation ..."));
  pb->setIcon(QIcon::fromTheme("list-remove"));
  connect(pb, SIGNAL(clicked()), this, SLOT(removeComponentAssignation()));
  widget->addWidgetToLocalBar(pb);
  widget->addStretchToLocalBar();

  return true;
}
