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
#include "mdtClLink.h"
#include "mdtClLinkData.h"
#include "mdtClUnitLinkDialog.h"
#include "mdtClPathGraphDialog.h"
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
#include <QFileDialog>

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
  if(!setupLinkListTable()){
    return false;
  }
  if(!setupLinkMarkingTable()){
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

void mdtClVehicleTypeEditor::editLink()
{
  mdtSqlTableWidget *linkWidget;
  mdtClUnitLinkDialog dialog(this, database());
  QVariant unitId;
  QVariant var;
  mdtClLinkPkData pk;
  mdtClLinkData linkData;
  mdtClLinkVersionData linkVersionData;
  mdtClModificationPkData modificationPk;
  ///mdtClLinkModificationKeyData oldModificationKey;
  mdtClLink lnk(database());
  bool ok;

  linkWidget = sqlTableWidget("UnitLink_view");
  Q_ASSERT(linkWidget != 0);
  // Get unit ID
  ///unitId = currentUnitId();
  if(unitId.isNull()){
    return;
  }
  // Get connection PK
  pk.connectionStartId = linkWidget->currentData("UnitConnectionStart_Id_FK");
  pk.connectionEndId = linkWidget->currentData("UnitConnectionEnd_Id_FK");
  if(pk.isNull()){
    QMessageBox msgBox;
    msgBox.setText(tr("Please select a link."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
    return;
  }
  ///oldModificationKey.setLinkFk(pk);
  // Get link version and modification
  var = linkWidget->currentData("Version_FK");
  if(!var.isNull()){
    linkVersionData.setVersionPk(var);
    ///oldModificationKey.setLinkVersionFk(linkVersionData.pk());
  }
  var = linkWidget->currentData("Modification_Code_FK");
  if(!var.isNull()){
    modificationPk.code = var.toString();
    ///oldModificationKey.setModificationFk(modificationPk);
  }
  // Get current link data
  linkData = lnk.getLinkData(pk, ok);
  if(!ok){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Setup and show dialog
  ///dialog.setWorkingOnVehicleTypeIdList(pvWorkingOnVehicleTypeIdList);
  dialog.setLinkData(linkData);
  if(!linkVersionData.isNull()){
    dialog.setLinkVersion(linkVersionData);
  }
  if(!modificationPk.isNull()){
    dialog.setLinkModification(modificationPk);
  }
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Update link
  if(!lnk.updateLink(pk, dialog.linkData(), dialog.selectedVehicleTypeList(), true)){
    pvLastError = lnk.lastError();
    displayLastError();
    return;
  }
  // Update links view
  select("UnitLink_view");

//   mdtSqlTableWidget *linkWidget;
//   mdtClUnitLinkDialog dialog(0, database());
//   QVariant vehicleTypeId, startConnectionId, endConnectionId;
//   mdtClLinkData linkData;
//   mdtClLink lnk(0, database());
//   bool ok;
// 
//   linkWidget = sqlTableWidget("LinkList_view");
//   Q_ASSERT(linkWidget != 0);
//   // Get vehicle type ID
//   vehicleTypeId = currentVehicleTypeId();
//   if(vehicleTypeId.isNull()){
//     return;
//   }
//   // Get connection IDs
//   startConnectionId = linkWidget->currentData("UnitConnectionStart_Id_FK");
//   endConnectionId = linkWidget->currentData("UnitConnectionEnd_Id_FK");
//   if(startConnectionId.isNull() || endConnectionId.isNull()){
//     QMessageBox msgBox;
//     msgBox.setText(tr("Please select a link."));
//     msgBox.setIcon(QMessageBox::Information);
//     msgBox.exec();
//     return;
//   }
//   // Get current link data
//   linkData = lnk.getLinkData(startConnectionId, endConnectionId,true, true, ok);
//   if(!ok){
//     pvLastError = lnk.lastError();
//     displayLastError();
//     return;
//   }
//   // Setup and show dialog
//   dialog.setWorkingOnVehicleTypeId(vehicleTypeId);
//   dialog.setLinkData(linkData);
//   if(dialog.exec() != QDialog::Accepted){
//     return;
//   }
//   // Edit link
//   if(!lnk.editLink(startConnectionId, endConnectionId, dialog.linkData())){
//     pvLastError = lnk.lastError();
//     displayLastError();
//     return;
//   }
//   // Update links view
//   select("LinkList_view");
}

void mdtClVehicleTypeEditor::viewLinkPath()
{
  mdtClPathGraphDialog dialog(0, database());
  mdtSqlTableWidget *widget;
  QVariant startConnectionId;

  widget = sqlTableWidget("LinkList_view");
  Q_ASSERT(widget != 0);
  // Get selected row
  startConnectionId = widget->currentData("UnitConnectionStart_Id_FK");
  if(startConnectionId.isNull()){
    QMessageBox msgBox;
    msgBox.setText(tr("Please select a link."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
    return;
  }
  // Setup and show dialog
  if(!dialog.loadLinkList()){
    return;
  }
  if(!dialog.drawPath(startConnectionId)){
    return;
  }

  dialog.exec();
}

// void mdtClVehicleTypeEditor::exportMarkingsToCsvFile()
// {
//   QFileDialog fileDialog(this);
//   QMessageBox msgBox(this);
//   QFileInfo csvFileInfo;
//   QString selectedFile;
//   mdtCsvFileSettings csvSettings;
//   mdtSqlTableWidget *widget;
//   QStringList fields;
// 
//   // Get SQL widget
//   widget = sqlTableWidget("LinkMarking_view");
//   Q_ASSERT(widget != 0);
//   // Setup file dialog
//   fileDialog.setFileMode(QFileDialog::AnyFile);
//   fileDialog.setNameFilter(tr("CSV files (*.csv)"));
//   fileDialog.setAcceptMode(QFileDialog::AcceptSave);
//   fileDialog.setDefaultSuffix("csv");
//   fileDialog.setDirectory(QDir::home());
//   if(fileDialog.exec() != QDialog::Accepted){
//     return;
//   }
//   Q_ASSERT(fileDialog.selectedFiles().size() == 1);
//   selectedFile = fileDialog.selectedFiles().at(0);
//   // Complete path with extension if missing
//   csvFileInfo.setFile(selectedFile);
//   if( (csvFileInfo.suffix() != "csv") && (csvFileInfo.suffix() != "CSV") ){
//     selectedFile += ".csv";
//     csvFileInfo.setFile(selectedFile);
//   }
//   /**
//   qDebug() << "File: " << csvFileInfo.fileName();
//   qDebug() << "Path: " << csvFileInfo.absoluteFilePath();
//   qDebug() << "Ext: " << csvFileInfo.suffix();
//   */
//   // If file allready exists, ask user vefore continue
//   /**
//   if(csvFileInfo.exists()){
//     msgBox.setText(tr("File ") + " '" + csvFileInfo.fileName() + "' " + tr("allready exists"));
//     msgBox.setInformativeText(tr("Do you want to overwrite it ?"));
//     msgBox.setIcon(QMessageBox::QMessageBox::Information);
//     msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
//     msgBox.setDefaultButton(QMessageBox::No);
//     if(msgBox.exec() != QMessageBox::Yes){
//       return;
//     }
//   }
//   */
//   // Export
//   fields << "StartMarking" << "EndMarking";
//   if(!widget->exportToCsvFile(csvFileInfo, csvSettings, false, fields)){
//     pvLastError = widget->lastError();
//     displayLastError();
//     return;
//   }
//   // Tell user that CSV was successfully exported
//   msgBox.setText(tr("CSV export done."));
//   msgBox.setInformativeText(tr("File was written to") + "'" + csvFileInfo.absoluteFilePath() + "'");
//   msgBox.setIcon(QMessageBox::QMessageBox::Information);
//   msgBox.exec();
// }

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

bool mdtClVehicleTypeEditor::setupLinkListTable()
{
  mdtSqlTableWidget *widget;
  mdtSqlRelationInfo relationInfo;
  QPushButton *pb;

  relationInfo.setChildTableName("LinkList_view");
  relationInfo.addRelation("Id_PK", "StartVehicleType_Id_PK", false);
  relationInfo.addRelation("Id_PK", "EndVehicleType_Id_PK", false, "OR");
  if(!addChildTable(relationInfo, tr("Link list"))){
    return false;
  }
  // Get widget to continue setup
  widget = sqlTableWidget("LinkList_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("UnitConnectionStart_Id_FK", true);
  widget->setColumnHidden("UnitConnectionEnd_Id_FK", true);
  widget->setColumnHidden("LinkBeam_Id_FK", true);
  widget->setColumnHidden("StartVehicleType_Id_PK", true);
  widget->setColumnHidden("StartVehicleType", true);
  widget->setColumnHidden("StartVehicleSubType", true);
  widget->setColumnHidden("StartVehicleSerie", true);
  widget->setColumnHidden("SinceVersion", true);
  widget->setColumnHidden("Modification", true);
  widget->setColumnHidden("LinkDirection_Code_FK", true);
  widget->setColumnHidden("LinkType_Code_FK", true);
  widget->setColumnHidden("StartUnit_Id_PK", true);
  widget->setColumnHidden("UnitConnectorStart_Id_FK", true);
  widget->setColumnHidden("StartConnectionType_Code_FK", true);
  widget->setColumnHidden("EndConnectionType_Code_FK", true);
  widget->setColumnHidden("EndVehicleType_Id_PK", true);
  widget->setColumnHidden("EndUnit_Id_PK", true);
  widget->setColumnHidden("UnitConnectorEnd_Id_FK", true);
  widget->setColumnHidden("EndVehicleType", true);
  widget->setColumnHidden("EndVehicleSubType", true);
  widget->setColumnHidden("EndVehicleSerie", true);
  widget->setColumnHidden("StartCabinet", true);
  widget->setColumnHidden("StartCoordinate", true);
  widget->setColumnHidden("StartFunctionFR", true);
  widget->setColumnHidden("StartFunctionEN", true);
  widget->setColumnHidden("StartFunctionDE", true);
  widget->setColumnHidden("StartFunctionIT", true);
  widget->setColumnHidden("EndCabinet", true);
  widget->setColumnHidden("EndCoordinate", true);
  // Give fields a user friendly name
  widget->setHeaderData("StartSchemaPosition", tr("Start\nschema pos."));
  widget->setHeaderData("StartAlias", tr("Start\nalias"));
  widget->setHeaderData("StartUnitConnectorName", tr("Start\nconnector"));
  widget->setHeaderData("StartUnitContactName", tr("Start\ncontact"));
  ///widget->setHeaderData("StartUnitConnectionResistance", tr("Start\nconnection\nresistance"));
  widget->setHeaderData("EndSchemaPosition", tr("End\nschema pos."));
  widget->setHeaderData("EndAlias", tr("End\nalias"));
  widget->setHeaderData("EndUnitConnectorName", tr("End\nconnector"));
  widget->setHeaderData("EndUnitContactName", tr("End\ncontact"));
  ///widget->setHeaderData("EndUnitConnectionResistance", tr("End\nconnection\nresistance"));
  widget->setHeaderData("SinceVersion", tr("Since\nversion"));
  ///widget->setHeaderData("LinkTypeNameEN", tr("Link type"));
  ///widget->setHeaderData("Length", tr("Link\nlength"));
  ///widget->setHeaderData("Resistance", tr("Link\nresistance"));
  ///widget->setHeaderData("LinkDirectionPictureAscii", tr("Direction"));
  ///widget->setHeaderData("StartSchemaPage", tr("Start\nschema\npage"));
  ///widget->setHeaderData("EndSchemaPage", tr("End\nschema\npage"));
  widget->setHeaderData("StartFunctionEN", tr("Start\nfunction (ENG)"));
  widget->setHeaderData("EndFunctionEN", tr("End\nfunction (ENG)"));
  ///widget->setHeaderData("StartSignalName", tr("Start\nsignal"));
  ///widget->setHeaderData("EndSignalName", tr("End\nsignal"));
  ///widget->setHeaderData("StartSwAddress", tr("Start\nSW address"));
  ///widget->setHeaderData("EndSwAddress", tr("End\nSW address"));
  // Setup sorting
  widget->addColumnToSortOrder("Identification", Qt::AscendingOrder);
  widget->addColumnToSortOrder("StartSchemaPosition", Qt::AscendingOrder);
  widget->addColumnToSortOrder("StartUnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("StartUnitContactName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndSchemaPosition", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndUnitConnectorName", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndUnitContactName", Qt::AscendingOrder);
  widget->sort();
  // Setup link edition button
  pb = new QPushButton(tr("Edit ..."));
  connect(pb, SIGNAL(clicked()), this, SLOT(editLink()));
  widget->addWidgetToLocalBar(pb);
  connect(widget->tableView(), SIGNAL(doubleClicked(const QModelIndex&)),this, SLOT(editLink()));
  // Setup link path view button
  pb = new QPushButton(tr("View path"));
  connect(pb, SIGNAL(clicked()), this, SLOT(viewLinkPath()));
  widget->addWidgetToLocalBar(pb);

  widget->addStretchToLocalBar();
  widget->resizeViewToContents();

  return true;
}

bool mdtClVehicleTypeEditor::setupLinkMarkingTable()
{
  mdtSqlTableWidget *widget;
  mdtSqlRelationInfo relationInfo;
  QString sql;
  QSqlQuery query(database());
  QPushButton *pb;

  /*
   * Query to generate LinkMarking_view
   * Should be generated by a dialog in some later time..
   */
  // Drop views
  sql = "DROP VIEW IF EXISTS 'LinkMarking_view'\n";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot drop old 'LinkMarking_view'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClVehicleTypeEditor");
    pvLastError.commit();
    return false;
  }
  sql = "DROP VIEW IF EXISTS 'LinkMarkingFormatBase_view'\n";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot drop old 'LinkMarking_view'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClVehicleTypeEditor");
    pvLastError.commit();
    return false;
  }
  // Create view that provides formated basis
  sql = "CREATE TEMP VIEW LinkMarkingFormatBase_view AS\n"\
         "SELECT\n"\
         " StartVehicleType_Id_PK,\n"\
         " EndVehicleType_Id_PK,\n"\
         // Format identification part
         " CASE\n"\
         "  WHEN Identification <> ''\n"\
         "   THEN Identification\n"\
         "  ELSE ' '\n"\
         " END AS IdentificationPart ,\n"\
         // Format start connection part
         " CASE\n"\
         "  WHEN (StartSchemaPosition <> '' AND StartUnitConnectorName <> '' AND StartUnitContactName <> '')\n"\
         "   THEN StartSchemaPosition || '-' || StartUnitConnectorName || ';' || StartUnitContactName\n"\
         "  WHEN (StartSchemaPosition <> '' AND StartUnitConnectorName <> '')\n"\
         "   THEN StartSchemaPosition || '-' || StartUnitConnectorName\n"\
         "  WHEN (StartSchemaPosition <> '' AND StartUnitContactName <> '')\n"\
         "   THEN StartSchemaPosition || ';' || StartUnitContactName\n"\
         "  WHEN (StartSchemaPosition <> '')\n"\
         "   THEN StartSchemaPosition\n"\
         "  ELSE '<Unknown StartConnectionPart>'\n"\
         " END AS StartConnectionPart ,\n"\
         // Format end connection part
         " CASE\n"\
         "  WHEN (EndSchemaPosition <> '' AND EndUnitConnectorName <> '' AND EndUnitContactName <> '')\n"\
         "   THEN EndSchemaPosition || '-' || EndUnitConnectorName || ';' || EndUnitContactName\n"\
         "  WHEN (EndSchemaPosition <> '' AND EndUnitConnectorName <> '')\n"\
         "   THEN EndSchemaPosition || '-' || EndUnitConnectorName\n"\
         "  WHEN (EndSchemaPosition <> '' AND EndUnitContactName <> '')\n"\
         "   THEN EndSchemaPosition || ';' || EndUnitContactName\n"\
         "  WHEN (EndSchemaPosition <> '')\n"\
         "   THEN EndSchemaPosition\n"\
         "  ELSE '<Unknown EndConnectionPart>'\n"\
         " END AS EndConnectionPart \n"\
         "FROM LinkList_view WHERE LinkType_Code_FK = 'CABLELINK'";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot generate 'LinkMarkingFormatBase_view'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClVehicleTypeEditor");
    pvLastError.commit();
    return false;
  }
  // Create second base for link marking table
  sql = "CREATE TEMP VIEW LinkMarkingBase2_view AS\n"\
         "SELECT\n"\
         " StartVehicleType_Id_PK,\n"\
         " EndVehicleType_Id_PK,\n"\
         " IdentificationPart || ' (' || StartConnectionPart || ')-(' || EndConnectionPart || ')' AS StartMarking ,\n"\
         " IdentificationPart || ' (' || EndConnectionPart || ')-(' || StartConnectionPart || ')' AS EndMarking\n"\
         "FROM LinkMarkingFormatBase_view";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot generate 'LinkMarkingBase2_view'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClVehicleTypeEditor");
    pvLastError.commit();
    return false;
  }
  // Create link marking table itself
  sql = "CREATE TEMP VIEW LinkMarking_view AS\n"\
         "SELECT\n"\
         " StartVehicleType_Id_PK,\n"\
         " EndVehicleType_Id_PK,\n"\
         " StartMarking ,\n"\
         " EndMarking ,\n"\
         " length(StartMarking) AS StartMarkinLength ,\n"\
         " length(EndMarking) AS EndMarkinLength\n"\
         "FROM LinkMarkingBase2_view";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot generate 'LinkMarking_view'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClVehicleTypeEditor");
    pvLastError.commit();
    return false;
  }

  // Setup relation and table view
  relationInfo.setChildTableName("LinkMarking_view");
  relationInfo.addRelation("Id_PK", "StartVehicleType_Id_PK", false);
  relationInfo.addRelation("Id_PK", "EndVehicleType_Id_PK", false, "OR");
  if(!addChildTable(relationInfo, tr("Link marking list"))){
    return false;
  }
  // Get widget to continue setup
  widget = sqlTableWidget("LinkMarking_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("StartVehicleType_Id_PK", true);
  widget->setColumnHidden("EndVehicleType_Id_PK", true);
  // Give fields a user friendly name
  widget->setHeaderData("StartMarking", tr("Start\nmarking"));
  widget->setHeaderData("EndMarking", tr("End\nmarking"));
  // Setup sorting
  widget->addColumnToSortOrder("StartMarking", Qt::AscendingOrder);
  widget->addColumnToSortOrder("EndMarking", Qt::AscendingOrder);
  widget->sort();
  // Setup export to CSV button
  pb = new QPushButton(tr("Export to CSV ..."));
  connect(pb, SIGNAL(clicked()), this, SLOT(exportMarkingsToCsvFile()));
  widget->addWidgetToLocalBar(pb);

  widget->addStretchToLocalBar();
  widget->resizeViewToContents();

  return true;
}
