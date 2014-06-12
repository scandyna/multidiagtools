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
#include "mdtClLinkBeamEditor.h"
#include "ui_mdtClLinkBeamEditor.h"
#include "mdtClLinkBeam.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtClUnitLinkDialog.h"
#include "mdtClLink.h"
#include "mdtClLinkData.h"
#include <QPushButton>
#include <QIcon>
#include <QMessageBox>
#include <QList>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QStringList>

#include <QDebug>

mdtClLinkBeamEditor::mdtClLinkBeamEditor(QWidget* parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtClLinkBeamEditor::setupTables() 
{
  if(!setupLinkBeamTable()){
    return false;
  }
  if(!setupLinkTable()){
    return false;
  }
  if(!setupStartUnitTable()){
    return false;
  }
  if(!setupEndUnitTable()){
    return false;
  }
  return true;
}

void mdtClLinkBeamEditor::addStartUnit()
{
  mdtClLinkBeam lb(0, database());
  mdtSqlSelectionDialog selectionDialog;
  mdtSqlTableSelection s;
  QVariant unitId;
  QVariant linkBeamId;
  QString sql;

  // Get current link beam ID
  linkBeamId = currentData("LinkBeam_tbl", "Id_PK");
  if(linkBeamId.isNull()){
    return;
  }
  // Setup and show dialog
  sql = lb.sqlForStartUnitSelection(linkBeamId);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setMessage(tr("Select start unit:"));
  selectionDialog.setColumnHidden("VehicleType_Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_PK", true);
  selectionDialog.setHeaderData("SubType", tr("Variant"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema\nposition"));
  selectionDialog.setHeaderData("ArticleCode", tr("Article\ncode"));
  selectionDialog.setHeaderData("DesignationEN", tr("Designation\nEnglish"));
  selectionDialog.setHeaderData("DesignationFR", tr("Designation\nFrench"));
  selectionDialog.setHeaderData("DesignationDE", tr("Designation\nGerman"));
  selectionDialog.setHeaderData("DesignationIT", tr("Designation\nItalian"));
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Unit_Id_PK");
  if(s.rowCount() < 1){
    return;
  }
  unitId = s.data(0, "Unit_Id_PK");
  if(unitId.isNull()){
    return;
  }
  // Add unit
  if(!lb.addStartUnit(unitId, linkBeamId)){
    pvLastError = lb.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("LinkBeam_UnitStart_view");
}

void mdtClLinkBeamEditor::removeStartUnits()
{
  mdtClLinkBeam lb(0, database());
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QMessageBox msgBox;

  // Get widget and selection
  widget = sqlTableWidget("LinkBeam_UnitStart_view");
  Q_ASSERT(widget != 0);
  s = widget->currentSelection("Unit_Id_FK");
  if(s.rowCount() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove assignations between selected units and current link beam."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove selected units
  if(!lb.removeStartUnits(s)){
    pvLastError = lb.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("LinkBeam_UnitStart_view");
}

void mdtClLinkBeamEditor::addEndUnit()
{
  mdtClLinkBeam lb(0, database());
  mdtSqlSelectionDialog selectionDialog;
  mdtSqlTableSelection s;
  QVariant unitId;
  QVariant linkBeamId;
  QString sql;

  // Get current link beam ID
  linkBeamId = currentData("LinkBeam_tbl", "Id_PK");
  if(linkBeamId.isNull()){
    return;
  }
  // Setup and show dialog
  sql = lb.sqlForEndUnitSelection(linkBeamId);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setMessage(tr("Select end unit:"));
  selectionDialog.setColumnHidden("VehicleType_Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_PK", true);
  selectionDialog.setHeaderData("SubType", tr("Variant"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema\nposition"));
  selectionDialog.setHeaderData("ArticleCode", tr("Article\ncode"));
  selectionDialog.setHeaderData("DesignationEN", tr("Designation\nEnglish"));
  selectionDialog.setHeaderData("DesignationFR", tr("Designation\nFrench"));
  selectionDialog.setHeaderData("DesignationDE", tr("Designation\nGerman"));
  selectionDialog.setHeaderData("DesignationIT", tr("Designation\nItalian"));
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Unit_Id_PK");
  if(s.rowCount() < 1){
    return;
  }
  unitId = s.data(0, "Unit_Id_PK");
  if(unitId.isNull()){
    return;
  }
  // Add unit
  if(!lb.addEndUnit(unitId, linkBeamId)){
    pvLastError = lb.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("LinkBeam_UnitEnd_view");
}

void mdtClLinkBeamEditor::removeEndUnits()
{
  mdtClLinkBeam lb(0, database());
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QMessageBox msgBox;

  // Get widget and selection
  widget = sqlTableWidget("LinkBeam_UnitEnd_view");
  Q_ASSERT(widget != 0);
  s = widget->currentSelection("Unit_Id_FK");
  if(s.rowCount() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove assignations between selected units and current link beam."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove selected units
  if(!lb.removeEndUnits(s)){
    pvLastError = lb.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("LinkBeam_UnitEnd_view");
}

void mdtClLinkBeamEditor::createLink()
{
  mdtClUnitLinkDialog dialog(0, database());
  mdtClLink lnk(0, database());
  mdtClLinkData linkData;
  QVariant linkBeamId;
  QVariant startUnitId;
  QList<QVariant> startUnitIdList;
  QVariant endUnitId;
  QList<QVariant> endUnitIdList;
  QSqlTableModel *m;
  QModelIndex index;
  int row;
  int col;

  // Get current leank beam ID
  linkBeamId = currentData("LinkBeam_tbl", "Id_PK");
  if(linkBeamId.isNull()){
    return;
  }
  // Get start units
  startUnitId = currentData("LinkBeam_UnitStart_view", "Unit_Id_FK");
  m = model("LinkBeam_UnitStart_view");
  Q_ASSERT(m != 0);
  col = m->fieldIndex("Unit_Id_FK");
  Q_ASSERT(col >= 0);
  for(row = 0; row < m->rowCount(); ++row){
    index = m->index(row, col);
    startUnitIdList.append(m->data(index));
  }
  // Get end units
  endUnitId = currentData("LinkBeam_UnitEnd_view", "Unit_Id_FK");
  m = model("LinkBeam_UnitEnd_view");
  Q_ASSERT(m != 0);
  col = m->fieldIndex("Unit_Id_FK");
  Q_ASSERT(col >= 0);
  for(row = 0; row < m->rowCount(); ++row){
    index = m->index(row, col);
    endUnitIdList.append(m->data(index));
  }
  // Setup and show dialog
  dialog.setStartUnitSelectionList(startUnitIdList);
  dialog.setStartUnit(startUnitId);
  dialog.setEndUnitSelectionList(endUnitIdList);
  dialog.setEndUnit(endUnitId);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update link data with current beam ID and add link to DB
  linkData = dialog.linkData();
  linkData.setValue("LinkBeam_Id_FK", linkBeamId);
  if(!lnk.addLink(linkData)){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitLink_view");
}

void mdtClLinkBeamEditor::addLink()
{
  mdtClLinkBeam lb(0, database());
  mdtSqlSelectionDialog selectionDialog;
  QSqlTableModel *m;
  QStringList fields;
  mdtSqlTableSelection s;
  QVariant startUnitConnectionId;
  QVariant endUnitConnectionId;
  QVariant linkBeamId;
  QString sql;
  QList<QVariant> startUnitIdList;
  QList<QVariant> endUnitIdList;
  int row;
  int col;
  QModelIndex index;

  // Get current link beam ID
  linkBeamId = currentData("LinkBeam_tbl", "Id_PK");
  if(linkBeamId.isNull()){
    return;
  }
  // Get start units
  m = model("LinkBeam_UnitStart_view");
  Q_ASSERT(m != 0);
  col = m->fieldIndex("Unit_Id_FK");
  Q_ASSERT(col >= 0);
  for(row = 0; row < m->rowCount(); ++row){
    index = m->index(row, col);
    startUnitIdList.append(m->data(index));
  }
  // Get end units
  m = model("LinkBeam_UnitEnd_view");
  Q_ASSERT(m != 0);
  col = m->fieldIndex("Unit_Id_FK");
  Q_ASSERT(col >= 0);
  for(row = 0; row < m->rowCount(); ++row){
    index = m->index(row, col);
    endUnitIdList.append(m->data(index));
  }
  // Setup and show dialog
  sql = lb.sqlForLinkSelection(startUnitIdList, endUnitIdList);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setMessage(tr("Select link:"));
  selectionDialog.setColumnHidden("LinkBeam_Id_FK", true);
  selectionDialog.setColumnHidden("UnitConnectionStart_Id_FK", true);
  selectionDialog.setColumnHidden("UnitConnectionEnd_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleLink_Id_FK", true);
  selectionDialog.setColumnHidden("StartUnit_Id_FK", true);
  selectionDialog.setColumnHidden("EndUnit_Id_FK", true);
  selectionDialog.setColumnHidden("LinkType_Code_FK", true);
  selectionDialog.setColumnHidden("LinkDirection_Code_FK", true);
  selectionDialog.setColumnHidden("ArticleConnectionStart_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnectionEnd_Id_FK", true);
  selectionDialog.setHeaderData("StartSchemaPosition", tr("Start\nschema pos."));
  selectionDialog.setHeaderData("StartAlias", tr("Start\nalias"));
  selectionDialog.setHeaderData("StartUnitConnectorName", tr("Start\nconnector"));
  selectionDialog.setHeaderData("StartUnitContactName", tr("Start\ncontact"));
  selectionDialog.setHeaderData("EndSchemaPosition", tr("End\nschema pos."));
  selectionDialog.setHeaderData("EndAlias", tr("End\nalias"));
  selectionDialog.setHeaderData("EndUnitConnectorName", tr("End\nconnector"));
  selectionDialog.setHeaderData("EndUnitContactName", tr("End\ncontact"));
  selectionDialog.setHeaderData("SinceVersion", tr("Since\nversion"));
  selectionDialog.setHeaderData("LinkTypeNameEN", tr("Link type"));
  selectionDialog.setHeaderData("ValueUnit", tr("Unit"));
  selectionDialog.setHeaderData("LinkDirectionPictureAscii", tr("Direction"));
  selectionDialog.setHeaderData("StartSchemaPage", tr("Start\nschema\npage"));
  selectionDialog.setHeaderData("EndSchemaPage", tr("End\nschema\npage"));
  selectionDialog.setHeaderData("StartFunctionEN", tr("Start\nfunction (ENG)"));
  selectionDialog.setHeaderData("EndFunctionEN", tr("End\nfunction (ENG)"));
  selectionDialog.setHeaderData("StartSignalName", tr("Start\nsignal"));
  selectionDialog.setHeaderData("EndSignalName", tr("End\nsignal"));
  selectionDialog.setHeaderData("StartSwAddress", tr("Start\nSW address"));
  selectionDialog.setHeaderData("EndSwAddress", tr("End\nSW address"));
  selectionDialog.addColumnToSortOrder("Identification", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Get selected link
  fields << "UnitConnectionStart_Id_FK" << "UnitConnectionEnd_Id_FK";
  s = selectionDialog.selection(fields);
  if(s.rowCount() < 1){
    return;
  }
  startUnitConnectionId = s.data(0, "UnitConnectionStart_Id_FK");
  if(startUnitConnectionId.isNull()){
    return;
  }
  endUnitConnectionId = s.data(0, "UnitConnectionEnd_Id_FK");
  if(endUnitConnectionId.isNull()){
    return;
  }
  // Add link
  if(!lb.addLink(startUnitConnectionId, endUnitConnectionId, linkBeamId)){
    pvLastError = lb.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitLink_view");
}

void mdtClLinkBeamEditor::removeLinks()
{
  mdtClLinkBeam lb(0, database());
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QVariant linkBeamId;
  QMessageBox msgBox;
  QStringList fields;

  // Get current link beam ID
  linkBeamId = currentData("LinkBeam_tbl", "Id_PK");
  if(linkBeamId.isNull()){
    return;
  }
  // Get widget and selection
  widget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  fields << "UnitConnectionStart_Id_FK" << "UnitConnectionEnd_Id_FK";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove selected links from current link beam."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove selected units
  if(!lb.removeLinks(s)){
    pvLastError = lb.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitLink_view");
}

void mdtClLinkBeamEditor::deleteLinks()
{
  mdtClLink lnk(0, database());
  mdtSqlTableSelection s;
  mdtSqlTableWidget *widget;
  QVariant linkBeamId;
  QMessageBox msgBox;
  QStringList fields;

  // Get current leank beam ID
  linkBeamId = currentData("LinkBeam_tbl", "Id_PK");
  if(linkBeamId.isNull()){
    return;
  }
  // Get widget and selection
  widget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  fields << "UnitConnectionStart_Id_FK" << "UnitConnectionEnd_Id_FK";
  s = widget->currentSelection(fields);
  if(s.isEmpty()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to delete selected links."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Remove selected units
  if(!lnk.removeLinks(s)){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Update views
  select("UnitLink_view");
}

bool mdtClLinkBeamEditor::setupLinkBeamTable() 
{
  Ui::mdtClLinkBeamEditor lbe;

  // Setup main form widget
  lbe.setupUi(mainSqlWidget());
  ///connect(this, SIGNAL(unitEdited()), mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!setMainTable("LinkBeam_tbl", "Link beams", database())){
    return false;
  }
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  return true;
}

bool mdtClLinkBeamEditor::setupLinkTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbCreateLink;
  QPushButton *pbAddLink;
  QPushButton *pbRemoveLink;
  QPushButton *pbDeleteLink;

  // Add link table
  if(!addChildTable("UnitLink_view", tr("Links"), database())){
    return false;
  }
  // Setup LinkBeam <-> Link relation
  if(!addRelation("Id_PK", "UnitLink_view", "LinkBeam_Id_FK")){
    return false;
  }
  // Get widget to continue setup
  widget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(widget != 0);
  // Hide relation fields and PK
  widget->setColumnHidden("UnitConnectionStart_Id_FK", true);
  widget->setColumnHidden("UnitConnectionEnd_Id_FK", true);
  widget->setColumnHidden("ArticleLink_Id_FK", true);
  widget->setColumnHidden("StartUnit_Id_FK", true);
  widget->setColumnHidden("EndUnit_Id_FK", true);
  widget->setColumnHidden("LinkType_Code_FK", true);
  widget->setColumnHidden("LinkDirection_Code_FK", true);
  widget->setColumnHidden("ArticleConnectionStart_Id_FK", true);
  widget->setColumnHidden("ArticleConnectionEnd_Id_FK", true);
  widget->setColumnHidden("LinkBeam_Id_FK", true);
  // Give fields a user friendly name
  widget->setHeaderData("StartSchemaPosition", tr("Start\nschema pos."));
  widget->setHeaderData("StartAlias", tr("Start\nalias"));
  widget->setHeaderData("StartUnitConnectorName", tr("Start\nconnector"));
  widget->setHeaderData("StartUnitContactName", tr("Start\ncontact"));
  widget->setHeaderData("EndSchemaPosition", tr("End\nschema pos."));
  widget->setHeaderData("EndAlias", tr("End\nalias"));
  widget->setHeaderData("EndUnitConnectorName", tr("End\nconnector"));
  widget->setHeaderData("EndUnitContactName", tr("End\ncontact"));
  widget->setHeaderData("SinceVersion", tr("Since\nversion"));
  widget->setHeaderData("LinkTypeNameEN", tr("Link type"));
  widget->setHeaderData("ValueUnit", tr("Unit"));
  widget->setHeaderData("LinkDirectionPictureAscii", tr("Direction"));
  widget->setHeaderData("StartSchemaPage", tr("Start\nschema\npage"));
  widget->setHeaderData("EndSchemaPage", tr("End\nschema\npage"));
  widget->setHeaderData("StartFunctionEN", tr("Start\nfunction (ENG)"));
  widget->setHeaderData("EndFunctionEN", tr("End\nfunction (ENG)"));
  widget->setHeaderData("StartSignalName", tr("Start\nsignal"));
  widget->setHeaderData("EndSignalName", tr("End\nsignal"));
  widget->setHeaderData("StartSwAddress", tr("Start\nSW address"));
  widget->setHeaderData("EndSwAddress", tr("End\nSW address"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Create link button
  pbCreateLink = new QPushButton(tr("New ..."));
  pbCreateLink->setIcon(QIcon::fromTheme("document-new"));
  widget->addWidgetToLocalBar(pbCreateLink);
  connect(pbCreateLink, SIGNAL(clicked()), this, SLOT(createLink()));
  // Add link button
  pbAddLink = new QPushButton(tr("Add ..."));
  pbAddLink->setIcon(QIcon::fromTheme("list-add"));
  widget->addWidgetToLocalBar(pbAddLink);
  connect(pbAddLink, SIGNAL(clicked()), this, SLOT(addLink()));
  // Remove links button
  pbRemoveLink = new QPushButton(tr("Remove ..."));
  pbRemoveLink->setIcon(QIcon::fromTheme("list-remove"));
  widget->addWidgetToLocalBar(pbRemoveLink);
  connect(pbRemoveLink, SIGNAL(clicked()), this, SLOT(removeLinks()));
  // Delete links button
  pbDeleteLink = new QPushButton(tr("Delete ..."));
  pbDeleteLink->setIcon(QIcon::fromTheme("edit-delete"));
  widget->addWidgetToLocalBar(pbDeleteLink);
  connect(pbDeleteLink, SIGNAL(clicked()), this, SLOT(deleteLinks()));
  widget->addStretchToLocalBar();

  return true;
}

bool mdtClLinkBeamEditor::setupStartUnitTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddUnit;
  QPushButton *pbRemoveUnits;

  // Add link table
  if(!addChildTable("LinkBeam_UnitStart_view", tr("Start units"), database())){
    return false;
  }
  // Setup LinkBeam <-> start unit relation
  if(!addRelation("Id_PK", "LinkBeam_UnitStart_view", "LinkBeam_Id_FK")){
    return false;
  }
  // Get widget to continue setup
  widget = sqlTableWidget("LinkBeam_UnitStart_view");
  Q_ASSERT(widget != 0);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("LinkBeam_Id_FK", true);
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Composite_Id_FK", true);
  widget->setColumnHidden("Article_Id_FK", true);
  widget->setHeaderData("SchemaPosition", tr("Schema\nposition"));
  // Add unit button
  pbAddUnit = new QPushButton(tr("Add ..."));
  pbAddUnit->setIcon(QIcon::fromTheme("list-add"));
  widget->addWidgetToLocalBar(pbAddUnit);
  connect(pbAddUnit, SIGNAL(clicked()), this, SLOT(addStartUnit()));
  // Remove units button
  pbRemoveUnits = new QPushButton(tr("Remove ..."));
  pbRemoveUnits->setIcon(QIcon::fromTheme("edit-delete"));
  widget->addWidgetToLocalBar(pbRemoveUnits);
  connect(pbRemoveUnits, SIGNAL(clicked()), this, SLOT(removeStartUnits()));
  widget->addStretchToLocalBar();

  return true;
}

bool mdtClLinkBeamEditor::setupEndUnitTable() 
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddUnit;
  QPushButton *pbRemoveUnits;

  // Add link table
  if(!addChildTable("LinkBeam_UnitEnd_view", tr("End units"), database())){
    return false;
  }
  // Setup LinkBeam <-> start unit relation
  if(!addRelation("Id_PK", "LinkBeam_UnitEnd_view", "LinkBeam_Id_FK")){
    return false;
  }
  // Get widget to continue setup
  widget = sqlTableWidget("LinkBeam_UnitEnd_view");
  Q_ASSERT(widget != 0);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("LinkBeam_Id_FK", true);
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Composite_Id_FK", true);
  widget->setColumnHidden("Article_Id_FK", true);
  widget->setHeaderData("SchemaPosition", tr("Schema\nposition"));
  // Add unit button
  pbAddUnit = new QPushButton(tr("Add ..."));
  pbAddUnit->setIcon(QIcon::fromTheme("list-add"));
  widget->addWidgetToLocalBar(pbAddUnit);
  connect(pbAddUnit, SIGNAL(clicked()), this, SLOT(addEndUnit()));
  // Remove units button
  pbRemoveUnits = new QPushButton(tr("Remove ..."));
  pbRemoveUnits->setIcon(QIcon::fromTheme("edit-delete"));
  widget->addWidgetToLocalBar(pbRemoveUnits);
  connect(pbRemoveUnits, SIGNAL(clicked()), this, SLOT(removeEndUnits()));
  widget->addStretchToLocalBar();

  return true;
}

