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
#include "mdtClUnitLinkDialog.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtClUnit.h"
#include "mdtClUnitConnection.h"
#include "mdtClUnitConnectorData.h"
#include "mdtClUnitConnectionSelectionDialog.h"
#include "mdtClModificationModel.h"
#include "mdtClLinkVersionModel.h"
#include "mdtClLinkTypeModel.h"
#include "mdtClLinkDirectionModel.h"
#include "mdtClVehicleTypeLinkAssignationWidget.h"
#include "mdtClLink.h"
#include "mdtError.h"
#include <QWidget>
#include <QModelIndex>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QComboBox>
#include <QString>
#include <QMessageBox>
#include <QCheckBox>

#include <QDebug>

mdtClUnitLinkDialog::mdtClUnitLinkDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  pvDatabase = db;
  setupUi(this);
  setWindowTitle(tr("Unit link edition"));
  // Setup Modification combobox
  pvModificationModel = new mdtClModificationModel(this, db);
  cbModification->setModel(pvModificationModel);
  cbModification->setModelColumn(1);
  // Setup link version combobox
  pvLinkVersionModel = new mdtClLinkVersionModel(this, db);
  cbLinkVersion->setModel(pvLinkVersionModel);
  cbLinkVersion->setModelColumn(1);
  // Setup link type
  pvLinkTypeModel = new mdtClLinkTypeModel(this, db);
  cbLinkType->setModel(pvLinkTypeModel);
  cbLinkType->setModelColumn(1);
  cbLinkType->setCurrentIndex(-1);
  connect(cbLinkType, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbLinkTypeCurrentIndexChanged(int)));
  // Setup link direction
  pvLinkDirectionModel = new mdtClLinkDirectionModel(this, db);
  cbLinkDirection->setModel(pvLinkDirectionModel);
  cbLinkDirection->setModelColumn(1);
  connect(cbLinkDirection, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbLinkDirectionCurrentIndexChanged(int)));
  cbLinkDirection->setCurrentIndex(-1);
  // Setup wire edition part
  connect(pbSelectWire, SIGNAL(clicked()), this, SLOT(selectWire()));
  connect(pbClearWire, SIGNAL(clicked()), this, SLOT(clearWire()));
  connect(deLength, SIGNAL(valueChanged(double, bool)), this, SLOT(setLinkResistance(double, bool)));
  clearWire();
  // Setup unit
  connect(pbStartUnit, SIGNAL(clicked()), this, SLOT(selectStartUnit()));
  connect(pbEndUnit, SIGNAL(clicked()), this, SLOT(selectEndUnit()));
  // Setup connections
  connect(pbStartConnection, SIGNAL(clicked()), this, SLOT(selectStartConnection()));
  connect(pbEndConnection, SIGNAL(clicked()), this, SLOT(selectEndConnection()));
  cbShowOnlyUnusedStartConnections->setChecked(true);
  cbShowOnlyUnusedEndConnections->setChecked(true);
  // Clear field labels
  lbStartSchemaPosition->clear();
  lbStartAlias->clear();
  lbStartCabinet->clear();
  lbStartConnectorName->clear();
  lbStartContactName->clear();
  lbEndSchemaPosition->clear();
  lbEndAlias->clear();
  lbEndCabinet->clear();
  lbEndConnectorName->clear();
  lbEndContactName->clear();
  // Setup vehicle type assignation part
  pvVehicleTypeAssignationWidget = new mdtClVehicleTypeLinkAssignationWidget(this, pvDatabase);
  saVehicleType->setWidget(pvVehicleTypeAssignationWidget);
}

mdtClUnitLinkDialog::~mdtClUnitLinkDialog()
{
}

void mdtClUnitLinkDialog::setStartUnit(const QVariant &unitId)
{
  ///pvLinkData.setStartConnectionData(mdtClUnitConnectionData());
  pvStartUnitId = unitId;
  updateStartUnit();
  auto pk = pvLinkData.pk();
  pk.connectionStartId.clear();
  pvLinkData.setPk(pk);
  updateStartConnection();
  updateVehicleTypeAssignations(true);
}

void mdtClUnitLinkDialog::setStartUnitSelectionList(const QList< QVariant >& idList)
{
  pvStartUnitSelectionIdList = idList;
}

void mdtClUnitLinkDialog::clearStartUnitSelectionList()
{
  pvStartUnitSelectionIdList.clear();
}

// void mdtClUnitLinkDialog::setStartConnectorLimitIdList(const QList<QVariant> & unitConnectorIdList)
// {
//   pvStartConnectorLimitIdList = unitConnectorIdList;
// }

void mdtClUnitLinkDialog::setStartConnectionLabel(const QString& labelText)
{
  gbStartConnection->setTitle(labelText);
}

void mdtClUnitLinkDialog::setEndUnit(const QVariant &unitId)
{
  pvEndUnitId = unitId;
  updateEndUnit();
  auto pk = pvLinkData.pk();
  pk.connectionEndId.clear();
  pvLinkData.setPk(pk);
  updateEndConnection();
  updateVehicleTypeAssignations(true);
}

void mdtClUnitLinkDialog::setEndUnitSelectionList(const QList< QVariant >& idList)
{
  pvEndUnitSelectionIdList = idList;
}

void mdtClUnitLinkDialog::clearEndUnitSelectionList()
{
  pvEndUnitSelectionIdList.clear();
}

// void mdtClUnitLinkDialog::setEndConnectorLimitIdList(const QList<QVariant> & unitConnectorIdList)
// {
//   pvEndConnectorLimitIdList = unitConnectorIdList;
// }

void mdtClUnitLinkDialog::setEndConnectionLabel(const QString& labelText)
{
  gbEndConnection->setTitle(labelText);
}

void mdtClUnitLinkDialog::setShowOnlyUnusedStartConnection(bool onlyUnused)
{
  cbShowOnlyUnusedStartConnections->setChecked(onlyUnused);
}

void mdtClUnitLinkDialog::setShowOnlyUnusedEndConnection(bool onlyUnused)
{
  cbShowOnlyUnusedEndConnections->setChecked(onlyUnused);
}

void mdtClUnitLinkDialog::setWorkingOnVehicleTypeIdList(const QList<QVariant> & vtIdList)
{
  pvWorkingOnVehicleTypeIdList = vtIdList;
}

void mdtClUnitLinkDialog::clearWorkingOnVehicleTypeList()
{
  pvWorkingOnVehicleTypeIdList.clear();
}

QList<mdtClVehicleTypeStartEndKeyData> mdtClUnitLinkDialog::selectedVehicleTypeList() const
{
  Q_ASSERT(pvVehicleTypeAssignationWidget != nullptr);

  return pvVehicleTypeAssignationWidget->getSelectedVehicleTypeList();
}

void mdtClUnitLinkDialog::setLinkType(mdtClLinkType_t t)
{
  int row;

  row = pvLinkTypeModel->row(t);
  cbLinkType->setCurrentIndex(row);
}

mdtClLinkTypeKeyData mdtClUnitLinkDialog::linkTypeKeyData() const
{
  return pvLinkTypeModel->currentKeyData(cbLinkType);
}

void mdtClUnitLinkDialog::setLinkDirection(mdtClLinkDirection_t d)
{
  int row;

  row = pvLinkDirectionModel->row(d);
  cbLinkDirection->setCurrentIndex(row);
}

mdtClLinkDirectionKeyData mdtClUnitLinkDialog::linkDirectionKeyData() const
{
  return pvLinkDirectionModel->currentKeyData(cbLinkDirection);
}

void mdtClUnitLinkDialog::setLinkVersion(const mdtClLinkVersionData &v)
{
  int row;

  row = pvLinkVersionModel->row(v.pk());
  cbLinkVersion->setCurrentIndex(row);
}

void mdtClUnitLinkDialog::setLinkModification(const mdtClModificationPkData &m)
{
  int row;

  row = pvModificationModel->row(m);
  cbModification->setCurrentIndex(row);
}

void mdtClUnitLinkDialog::setLinkModification(mdtClModification_t m)
{
  mdtClModificationPkData pk;

  pk.setModification(m);
  setLinkModification(pk);
}

// mdtClLinkModificationKeyData mdtClUnitLinkDialog::linkModificationKeyData() const
// {
//   Q_ASSERT(!pvLinkData.pk().isNull());
// 
//   mdtClLinkModificationKeyData key;
// 
//   key.setLinkFk(pvLinkData.pk());
//   key.setLinkVersionFk(pvLinkVersionModel->currentVersionPk(cbLinkVersion));
//   key.setModificationFk(pvModificationModel->currentModificationPk(cbModification));
//   Q_ASSERT(!key.isNull());
// 
//   return key;
// }

void mdtClUnitLinkDialog::setLinkData(mdtClLinkData &data)
{
  mdtClUnitConnection ucnx(pvDatabase);
  mdtClUnitConnectionPkData ucnxPk;
  mdtClUnitConnectionData ucnxData;
  mdtClLinkVersionData linkVersionData;
  bool ok;

  pvLinkData = data;
  // Update common data
  setLinkType(data.keyData().linkTypeFk().type());
  setLinkDirection(data.keyData().linkDirectionFk().direction());
  deResistance->setValue(data.resistance.value());
  deLength->setValue(data.length.value());
  updateWire(data.keyData().wireId());
  leIdentification->setText(data.identification.toString());
  // Update start unit
  ucnxPk.id = data.pk().connectionStartId;
  ucnxData = ucnx.getUnitConnectionData(ucnxPk, ok);
  if(ok){
    pvStartUnitId = ucnxData.keyData().unitId();
  }else{
    pvStartUnitId.clear();
  }
  updateStartUnit();
  // Update end unit
  ucnxPk.id = data.pk().connectionEndId;
  ucnxData = ucnx.getUnitConnectionData(ucnxPk, ok);
  if(ok){
    pvEndUnitId = ucnxData.keyData().unitId();
  }else{
    pvEndUnitId.clear();
  }
  updateEndUnit();
  // Update start/end connections
  updateStartConnection();
  updateEndConnection();
  // Update vehicle type assignements
  updateVehicleTypeAssignations(true);
  // Update versionning
  linkVersionData.setPk(pvLinkData.pk().versionFk);
  setLinkVersion(linkVersionData);
  setLinkModification(pvLinkData.pk().modificationFk);
}

mdtClLinkData mdtClUnitLinkDialog::linkData()
{
  return pvLinkData;
}

void mdtClUnitLinkDialog::onCbLinkTypeCurrentIndexChanged(int row)
{
  if(row < 0){
    return;
  }
  // We must update available directions regarding link type
  auto key = pvLinkTypeModel->keyData(row);
  pvLinkDirectionModel->setLinkType(key.type());
  if(pvLinkDirectionModel->rowCount() > 1){
    cbLinkDirection->setEnabled(true);
  }else{
    cbLinkDirection->setEnabled(false);
  }
  // Update link data
  pvLinkData.setLinkType(key.type());
}

void mdtClUnitLinkDialog::onCbLinkDirectionCurrentIndexChanged(int row)
{
  if(row < 0){
    lbLinkDirectionAsciiPicture->setText("");
    return;
  }
  auto key = pvLinkDirectionModel->keyData(row);
  // Update the ASCII picture
  lbLinkDirectionAsciiPicture->setText(pvLinkDirectionModel->pictureAscii(row));
  // Update link data
  pvLinkData.setLinkDirection(key.direction());
}

void mdtClUnitLinkDialog::selectWire()
{
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  QString sql;

  sql = "SELECT * FROM Wire_tbl";
  selectionDialog.setMessage(tr("Please select wire:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  ///selectionDialog.setColumnHidden("Unit_Id_PK", true);
  ///selectionDialog.setHeaderData("Vehicle", tr("Type"));
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Id_PK");
  Q_ASSERT(s.rowCount() == 1);
  updateWire(s.data(0, "Id_PK"));
}

void mdtClUnitLinkDialog::clearWire()
{
  updateWire(QVariant());
}

void mdtClUnitLinkDialog::setLinkResistance(double linkLength, bool linkLengthIsValid)
{
  mdtClLink lnk(0, pvDatabase);
  QString sql;
  QList<QVariant> dataList;
  QVariant lineicResistance;
  bool ok;

  if((!linkLengthIsValid)||(deLength->isNull())){
    deResistance->setValue(QVariant());
    return;
  }
  sql = "SELECT LineicResistance FROM Wire_tbl WHERE Id_PK = " + pvLinkData.keyData().wireId().toString();
  dataList = lnk.getDataList<QVariant>(sql, ok);
  if(!ok){
    deResistance->setValue(QVariant());
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  lineicResistance = dataList.at(0);
  updateLinkResistance(linkLength, lineicResistance);
}

void mdtClUnitLinkDialog::selectStartUnit()
{
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  QString sql;
  int i;

  // Setup SQL query statement
  sql = "SELECT * FROM Unit_view ";
  if(pvStartUnitSelectionIdList.size() > 0){
    sql += " WHERE (Unit_Id_PK = " + pvStartUnitSelectionIdList.at(0).toString();
    for(i = 1; i < pvStartUnitSelectionIdList.size(); ++i){
      sql += " OR Unit_Id_PK = " + pvStartUnitSelectionIdList.at(i).toString();
    }
    sql += ")";
  }
  if(!pvStartUnitId.isNull()){
    if(pvStartUnitSelectionIdList.isEmpty()){
      sql += " WHERE (";
    }else{
      sql += " AND (";
    }
    sql += "Unit_Id_PK <> " + pvStartUnitId.toString();
    sql += ")";
  }
  if(pvWorkingOnVehicleTypeIdList.size() > 0){
    if((pvStartUnitSelectionIdList.isEmpty())&&(pvStartUnitId.isNull())){
      sql += " WHERE (";
    }else{
      sql += " AND (";
    }
    Q_ASSERT(pvWorkingOnVehicleTypeIdList.size() > 0);
    sql += " VehicleType_Id_PK = " + pvWorkingOnVehicleTypeIdList.at(0).toString();
    for(i = 1; i < pvWorkingOnVehicleTypeIdList.size(); ++i){
      sql += " OR VehicleType_Id_PK = " + pvWorkingOnVehicleTypeIdList.at(i).toString();
    }
    sql += ")";
  }
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select start unit:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setColumnHidden("VehicleType_Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_PK", true);
  selectionDialog.setHeaderData("SubType", tr("Sub type"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema position"));
  selectionDialog.setHeaderData("ArticleCode", tr("Article code"));
  selectionDialog.setHeaderData("DesignationEN", tr("Designation (ENG)"));
  selectionDialog.addColumnToSortOrder("Type", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SubType", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SeriesNumber", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Unit_Id_PK");
  Q_ASSERT(s.rowCount() == 1);
  // Store unit and update GUI
  setStartUnit(s.data(0, "Unit_Id_PK"));
}

void mdtClUnitLinkDialog::selectEndUnit()
{
  mdtSqlSelectionDialog selectionDialog(this);
  mdtSqlTableSelection s;
  QString sql;
  int i;

  // Setup and run query
  sql = "SELECT * FROM Unit_view ";
  if(pvEndUnitSelectionIdList.size() > 0){
    sql += " WHERE (Unit_Id_PK = " + pvEndUnitSelectionIdList.at(0).toString();
    for(i = 1; i < pvEndUnitSelectionIdList.size(); ++i){
      sql += " OR Unit_Id_PK = " + pvEndUnitSelectionIdList.at(i).toString();
    }
    sql += ")";
  }
  if(!pvEndUnitId.isNull()){
    if(pvEndUnitSelectionIdList.isEmpty()){
      sql += " WHERE (";
    }else{
      sql += " AND (";
    }
    sql += "Unit_Id_PK <> " + pvEndUnitId.toString();
    sql += ")";
  }
  if(pvWorkingOnVehicleTypeIdList.size() > 0){
    if((pvEndUnitSelectionIdList.isEmpty())&&(pvEndUnitId.isNull())){
      sql += " WHERE (";
    }else{
      sql += " AND (";
    }
    Q_ASSERT(pvWorkingOnVehicleTypeIdList.size() > 0);
    sql += " VehicleType_Id_PK = " + pvWorkingOnVehicleTypeIdList.at(0).toString();
    for(i = 1; i < pvWorkingOnVehicleTypeIdList.size(); ++i){
      sql += " OR VehicleType_Id_PK = " + pvWorkingOnVehicleTypeIdList.at(i).toString();
    }
    sql += ")";
  }
  // Setup and show dialog
  ///selectionDialog.setMessage(tr("Please select end unit:"));
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setColumnHidden("VehicleType_Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_PK", true);
  selectionDialog.setHeaderData("Type", tr("Vehicle"));
  selectionDialog.setHeaderData("SubType", tr("Sub type"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema position"));
  selectionDialog.setHeaderData("ArticleCode", tr("Article code"));
  selectionDialog.setHeaderData("DesignationEN", tr("Designation (ENG)"));
  selectionDialog.addColumnToSortOrder("Type", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SubType", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SeriesNumber", Qt::AscendingOrder);
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.resize(800, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  s = selectionDialog.selection("Unit_Id_PK");
  Q_ASSERT(s.rowCount() == 1);
  // Store unit and update GUI
  setEndUnit(s.data(0, "Unit_Id_PK"));
}

void mdtClUnitLinkDialog::selectStartConnection()
{
  mdtClUnitConnectionSelectionDialog selectionDialog(this);
  mdtClUnitConnectionSelectionDialog::LinkUsage_t linkUsage;
  mdtClUnitConnectionPkData ucnxPk;

  // Setup and show dialog
  if(cbShowOnlyUnusedStartConnections->isChecked()){
    linkUsage = mdtClUnitConnectionSelectionDialog::LinkUsage_t::NotUsed;
  }else{
    linkUsage = mdtClUnitConnectionSelectionDialog::LinkUsage_t::All;
  }
  if(!selectionDialog.select(pvDatabase, pvStartUnitId, pvStartConnectorLimitIdList, linkUsage, false)){
    displayError(selectionDialog.lastError());
    return;
  }
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Get connection data and update
  ucnxPk = selectionDialog.selectedUnitConnectionPk();
  auto pk = pvLinkData.pk();
  pk.connectionStartId = ucnxPk.id;
  pvLinkData.setPk(pk);
  updateStartConnection();
  // Update vehicle type assignations
  updateVehicleTypeAssignations(false);
}

void mdtClUnitLinkDialog::selectEndConnection()
{
  mdtClUnitConnectionSelectionDialog selectionDialog(this);
  mdtClUnitConnectionSelectionDialog::LinkUsage_t linkUsage;
  mdtClUnitConnectionPkData ucnxPk;

  // Setup and show dialog
  if(cbShowOnlyUnusedEndConnections->isChecked()){
    linkUsage = mdtClUnitConnectionSelectionDialog::LinkUsage_t::NotUsed;
  }else{
    linkUsage = mdtClUnitConnectionSelectionDialog::LinkUsage_t::All;
  }
  if(!selectionDialog.select(pvDatabase, pvEndUnitId, pvStartConnectorLimitIdList, linkUsage, false)){
    displayError(selectionDialog.lastError());
    return;
  }
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  // Get connection data and update
  ucnxPk = selectionDialog.selectedUnitConnectionPk();
  auto pk = pvLinkData.pk();
  pk.connectionEndId = ucnxPk.id;
  pvLinkData.setPk(pk);
  updateEndConnection();
  // Update vehicle type assignations
  updateVehicleTypeAssignations(false);
}

void mdtClUnitLinkDialog::accept()
{
  // Some data are dynamically stored during edition, but some are to handle here
  mdtClLinkPkData pk = pvLinkData.pk();
  pk.versionFk = pvLinkVersionModel->currentVersionPk(cbLinkVersion);
  pk.modificationFk = pvModificationModel->currentModificationPk(cbModification);
  pvLinkData.setPk(pk);
  pvLinkData.identification = leIdentification->text();
  pvLinkData.resistance = deResistance->valueDouble();
  pvLinkData.length = deLength->valueDouble();

  /// \todo checks
  
  QDialog::accept();
}

void mdtClUnitLinkDialog::updateWire(const QVariant& wireId)
{
  mdtClLink lnk(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  bool ok;

  pvLinkData.setWireId(wireId);
  if(wireId.isNull()){
    lbWireModel->clear();
    lbWireSection->clear();
    lbWireColorEn->clear();
    lbWireArticleCode->clear();
    updateLinkResistance(QVariant(), QVariant());
    return;
  }
  sql = "SELECT ArticleCode, Model, Section, ColorEN, LineicResistance FROM Wire_tbl WHERE Id_PK = " + wireId.toString();
  dataList = lnk.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    lbWireModel->setText("<Error!>");
    lbWireSection->setText("<Error!>");
    lbWireColorEn->setText("<Error!>");
    lbWireArticleCode->setText("<Error!>");
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  lbWireModel->setText(data.value("Model").toString());
  lbWireSection->setText(data.value("Section").toString());
  lbWireColorEn->setText(data.value("ColorEN").toString());
  lbWireArticleCode->setText(data.value("ArticleCode").toString());

  updateLinkResistance(deLength->value(), data.value("LineicResistance"));
}

void mdtClUnitLinkDialog::updateLinkResistance(const QVariant& linkLength, const QVariant& lineicResistance)
{
  double r;

  if((linkLength.isNull())||(lineicResistance.isNull())){
    deResistance->setValue(QVariant());
    return;
  }
  r = linkLength.toDouble() * lineicResistance.toDouble();
  deResistance->setValue(r);
}

void mdtClUnitLinkDialog::updateStartUnit()
{
  mdtClUnit unit(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  bool ok;
  if(pvStartUnitId.isNull()){
    lbStartSchemaPosition->setText("");
    lbStartAlias->setText("");
    lbStartCabinet->setText("");
    return;
  }
  // Get unit data
  sql = "SELECT SchemaPosition, Alias, Cabinet FROM Unit_tbl WHERE Id_PK = " + pvStartUnitId.toString();
  dataList = unit.getData(sql, &ok);
  if(!ok){
    lbStartSchemaPosition->setText("<Error!>");
    lbStartAlias->setText("<Error!>");
    lbStartCabinet->setText("<Error!>");
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  lbStartSchemaPosition->setText(data.value("SchemaPosition").toString());
  lbStartAlias->setText(data.value("Alias").toString());
  lbStartCabinet->setText(data.value("Cabinet").toString());
}

void mdtClUnitLinkDialog::updateEndUnit()
{
  mdtClUnit unit(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  bool ok;

  if(pvEndUnitId.isNull()){
    lbEndSchemaPosition->setText("");
    lbEndAlias->setText("");
    lbEndCabinet->setText("");
    return;
  }
  // Get unit data
  sql = "SELECT SchemaPosition, Alias, Cabinet FROM Unit_tbl WHERE Id_PK = " + pvEndUnitId.toString();
  dataList = unit.getData(sql, &ok);
  if(!ok){
    lbEndSchemaPosition->setText("<Error!>");
    lbEndAlias->setText("<Error!>");
    lbEndCabinet->setText("<Error!>");
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  lbEndSchemaPosition->setText(data.value("SchemaPosition").toString());
  lbEndAlias->setText(data.value("Alias").toString());
  lbEndCabinet->setText(data.value("Cabinet").toString());
}

void mdtClUnitLinkDialog::updateStartConnection()
{
  mdtClUnitConnection ucnx(pvDatabase);
  mdtClUnitConnectionPkData connectionPk;
  mdtClUnitConnectionData connectionData;
  mdtClUnitConnectorData connectorData;
  bool ok;

  // Set connection name
  connectionPk.id = pvLinkData.pk().connectionStartId;
  if(connectionPk.isNull()){
    lbStartContactName->setText("");
    lbStartConnectorName->setText("");
    return;
  }
  connectionData = ucnx.getUnitConnectionData(connectionPk, ok);
  if(ok){
    lbStartContactName->setText(connectionData.name.toString());
  }else{
    lbStartContactName->setText("<Error!>");
  }
  // Set connector name
  connectorData = ucnx.getUnitConnectorData(connectionPk, ok);
  if(ok){
    lbStartConnectorName->setText(connectorData.name.toString());
  }else{
    lbStartConnectorName->setText("<Error!>");
  }
}

void mdtClUnitLinkDialog::updateEndConnection()
{
  mdtClUnitConnection ucnx(pvDatabase);
  mdtClUnitConnectionPkData connectionPk;
  mdtClUnitConnectionData connectionData;
  mdtClUnitConnectorData connectorData;
  bool ok;

  // Set connection name
  connectionPk.id = pvLinkData.pk().connectionEndId;
  if(connectionPk.isNull()){
    lbEndContactName->setText("");
    lbEndConnectorName->setText("");
    return;
  }
  connectionData = ucnx.getUnitConnectionData(connectionPk, ok);
  if(ok){
    lbEndContactName->setText(connectionData.name.toString());
  }else{
    lbEndContactName->setText("<Error!>");
  }
  // Set connector name
  connectorData = ucnx.getUnitConnectorData(connectionPk, ok);
  if(ok){
    lbEndConnectorName->setText(connectorData.name.toString());
  }else{
    lbEndConnectorName->setText("<Error!>");
  }
}

void mdtClUnitLinkDialog::updateVehicleTypeAssignations(bool rebuildVehicleTypeList)
{
  // If vehicle type - link assignation is disabled, we simply do nothing
  if(pvVehicleTypeAssignationWidget == nullptr){
    return;
  }
  if(rebuildVehicleTypeList){
    pvVehicleTypeAssignationWidget->clear();
    if(pvStartUnitId.isNull() || pvEndUnitId.isNull()){
      return;
    }
    if(!pvVehicleTypeAssignationWidget->buildVehicleTypeList(pvStartUnitId, pvEndUnitId)){
      displayError(pvVehicleTypeAssignationWidget->lastError());
      return;
    }
  }
  if(pvLinkData.pk().isNull()){
    return;
  }
  if(!pvVehicleTypeAssignationWidget->selectVehicleTypeAssignedToLink(pvLinkData.pk())){
    displayError(pvVehicleTypeAssignationWidget->lastError());
    return;
  }
}

void mdtClUnitLinkDialog::displayError(const mdtError & error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
