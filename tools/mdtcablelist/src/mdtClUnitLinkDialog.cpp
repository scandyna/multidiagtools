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
#include "mdtClUnitLinkDialog.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtClUnit.h"
#include "mdtError.h"
#include <QWidget>
#include <QModelIndex>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QComboBox>
#include <QString>
#include <QMessageBox>

#include <QDebug>

mdtClUnitLinkDialog::mdtClUnitLinkDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  pvDatabase = db;
  setupUi(this);
  setWindowTitle(tr("Unit link edition"));
  // Setup link type
  pvLinkTypeModel = new QSqlQueryModel(this);
  pvLinkTypeModel->setQuery("SELECT Code_PK, NameEN, ValueUnit FROM LinkType_tbl ORDER BY NameEN", pvDatabase);
  cbLinkType->setModel(pvLinkTypeModel);
  cbLinkType->setModelColumn(1);
  cbLinkType->setCurrentIndex(-1);
  connect(cbLinkType, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbLinkTypeCurrentIndexChanged(int)));
  // Setup link direction
  pvLinkDirectionModel = new QSqlQueryModel(this);
  pvLinkDirectionModel->setQuery("SELECT Code_PK, NameEN FROM LinkDirection_tbl", pvDatabase);
  cbLinkDirection->setModel(pvLinkDirectionModel);
  cbLinkDirection->setModelColumn(1);
  connect(cbLinkDirection, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbLinkDirectionCurrentIndexChanged(int)));
  cbLinkDirection->setCurrentIndex(-1);
  // Setup modification combobox
  cbModification->addItem("new", QVariant("new"));
  // Setup since version combobox
  
  
  // Set default link type
  setLinkTypeCode("CABLELINK");
  // Setup unit
  connect(pbStartUnit, SIGNAL(clicked()), this, SLOT(selectStartUnit()));
  connect(pbEndUnit, SIGNAL(clicked()), this, SLOT(selectEndUnit()));
  // Setup connections
  connect(pbStartConnection, SIGNAL(clicked()), this, SLOT(selectStartConnection()));
  connect(pbEndConnection, SIGNAL(clicked()), this, SLOT(selectEndConnection()));
  // Setup vehicle types
  pvStartVehicleTypesModel = new QSqlQueryModel(this);
  twStartVehicles->setModel(pvStartVehicleTypesModel);
  connect(pbStartVehicles, SIGNAL(clicked()), this, SLOT(selectStartVehicleTypes()));
  pvEndVehicleTypesModel = new QSqlQueryModel(this);
  twEndVehicles->setModel(pvEndVehicleTypesModel);
  connect(pbEndVehicles, SIGNAL(clicked()), this, SLOT(selectEndVehicleTypes()));
  // Clear field labels
  lbStartSchemaPosition->clear();
  lbStartCabinet->clear();
  lbStartConnectorName->clear();
  lbStartContactName->clear();
  lbEndSchemaPosition->clear();
  lbEndCabinet->clear();
  lbEndConnectorName->clear();
  lbEndContactName->clear();
  ///pvValue = 0.0;
  lbUnit->clear();

}

mdtClUnitLinkDialog::~mdtClUnitLinkDialog()
{
}

void mdtClUnitLinkDialog::setStartUnit(const QVariant &unitId)
{
  QSqlQuery query(pvDatabase);
  QSqlError sqlError;
  QString sql;

  sql = "SELECT Id_PK, SchemaPosition, Cabinet FROM Unit_tbl WHERE Id_PK = " + unitId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot select data from Unit_view", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitLinkDialog");
    e.commit();
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot select data from Unit_view."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  if(!query.next()){
    return;
  }
  setStartUnit(unitId, query.value(1), query.value(2));
}

void mdtClUnitLinkDialog::setStartUnit(const QVariant &unitId, const QVariant &schemaPosition, const QVariant &cabinet)
{
  pvStartUnitId = unitId;
  lbStartSchemaPosition->setText(schemaPosition.toString());
  lbStartCabinet->setText(cabinet.toString());
  setStartConnectionData(mdtClUnitConnectionData());
  setStartVehicleTypes(unitId);
}

QVariant mdtClUnitLinkDialog::startUnitId() const
{
  return pvStartUnitId;
}

void mdtClUnitLinkDialog::setEndUnit(const QVariant &unitId)
{
  QSqlQuery query(pvDatabase);
  QSqlError sqlError;
  QString sql;

  sql = "SELECT Id_PK, SchemaPosition, Cabinet FROM Unit_tbl WHERE Id_PK = " + unitId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    mdtError e(MDT_DATABASE_ERROR, "Cannot select data from Unit_view", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitLinkDialog");
    e.commit();
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot select data from Unit_view."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  if(!query.next()){
    return;
  }
  setEndUnit(unitId, query.value(1), query.value(2));
}

void mdtClUnitLinkDialog::setEndUnit(const QVariant &unitId, const QVariant &schemaPosition, const QVariant &cabinet)
{
  pvEndUnitId = unitId;
  lbEndSchemaPosition->setText(schemaPosition.toString());
  lbEndCabinet->setText(cabinet.toString());
  setEndConnectionData(mdtClUnitConnectionData());
  setEndVehicleTypes(unitId);
}

QVariant mdtClUnitLinkDialog::endUnitId() const
{
  return pvEndUnitId;
}

void mdtClUnitLinkDialog::setStartConnectionData(const mdtClUnitConnectionData &data)
{
  pvStartConnectionData = data;
  pvLinkData.setUnitConnectionStartId(pvStartConnectionData.id());
  lbStartConnectorName->setText(pvStartConnectionData.unitConnectorName().toString());
  lbStartContactName->setText(pvStartConnectionData.unitContactName().toString());
}

mdtClUnitConnectionData mdtClUnitLinkDialog::startConnectionData() const
{
  return pvStartConnectionData;
}

void mdtClUnitLinkDialog::setEndConnectionData(const mdtClUnitConnectionData &data)
{
  pvEndConnectionData = data;
  pvLinkData.setUnitConnectionEndId(pvEndConnectionData.id());
  lbEndConnectorName->setText(pvEndConnectionData.unitConnectorName().toString());
  lbEndContactName->setText(pvEndConnectionData.unitContactName().toString());
}

mdtClUnitConnectionData mdtClUnitLinkDialog::endConnectionData() const
{
  return pvEndConnectionData;
}

void mdtClUnitLinkDialog::setStartVehicleTypes(const QVariant &unitId)
{
  QString sql;

  sql = "SELECT Type, SubType, SeriesNumber, VehicleType_Id_FK FROM Unit_VehicleType_view WHERE Unit_Id_FK = " + unitId.toString();
  sql += " ORDER BY VehicleType_Id_FK ASC";
  pvStartVehicleTypesModel->setQuery(sql, pvDatabase);
  pvStartVehicleTypesModel->setHeaderData(0, Qt::Horizontal, tr("Type"));
  pvStartVehicleTypesModel->setHeaderData(1, Qt::Horizontal, tr("Sub type"));
  pvStartVehicleTypesModel->setHeaderData(2, Qt::Horizontal, tr("Serie"));
  twStartVehicles->setColumnHidden(3, true);
  twStartVehicles->resizeColumnsToContents();
  pvLinkData.setVehicleTypeStartIdList(startVehicleTypeIdList());
}

const QList<QVariant> mdtClUnitLinkDialog::startVehicleTypeIdList() const
{
  QList<QVariant> idList;
  QModelIndex index;
  int row;

  for(row = 0; row < pvStartVehicleTypesModel->rowCount(); ++row){
    index = pvStartVehicleTypesModel->index(row, 3);
    idList.append(pvStartVehicleTypesModel->data(index));
  }

  return idList;
}

void mdtClUnitLinkDialog::setEndVehicleTypes(const QVariant &unitId)
{
  QString sql;

  sql = "SELECT Type, SubType, SeriesNumber, VehicleType_Id_FK FROM Unit_VehicleType_view WHERE Unit_Id_FK = " + unitId.toString();
  sql += " ORDER BY VehicleType_Id_FK ASC";
  pvEndVehicleTypesModel->setQuery(sql, pvDatabase);
  pvEndVehicleTypesModel->setHeaderData(0, Qt::Horizontal, tr("Type"));
  pvEndVehicleTypesModel->setHeaderData(1, Qt::Horizontal, tr("Sub type"));
  pvEndVehicleTypesModel->setHeaderData(2, Qt::Horizontal, tr("Serie"));
  twEndVehicles->setColumnHidden(3, true);
  twEndVehicles->resizeColumnsToContents();
  pvLinkData.setVehicleTypeEndIdList(endVehicleTypeIdList());
}

const QList<QVariant> mdtClUnitLinkDialog::endVehicleTypeIdList() const
{
  QList<QVariant> idList;
  QModelIndex index;
  int row;

  for(row = 0; row < pvEndVehicleTypesModel->rowCount(); ++row){
    index = pvEndVehicleTypesModel->index(row, 3);
    idList.append(pvEndVehicleTypesModel->data(index));
  }

  return idList;
}

void mdtClUnitLinkDialog::setLinkTypeCode(const QVariant & code)
{
  QModelIndex index;
  int row;
  QVariant data;

  for(row = 0; row < pvLinkTypeModel->rowCount(); ++row){
    index = pvLinkTypeModel->index(row, 0);
    data = pvLinkTypeModel->data(index);
    if(data == code){
      cbLinkType->setCurrentIndex(row);
      return;
    }
  }
}

QVariant mdtClUnitLinkDialog::linkTypeCode() const
{
  QModelIndex index;
  int row;
  QVariant data;

  row = cbLinkType->currentIndex();
  if(row < 0){
    return data;
  }
  index = pvLinkTypeModel->index(row, 0);
  data = pvLinkTypeModel->data(index);

  return data;
}

void mdtClUnitLinkDialog::setLinkDirectionCode(const QVariant & code)
{
  QModelIndex index;
  int row;
  QVariant data;

  for(row = 0; row < pvLinkDirectionModel->rowCount(); ++row){
    index = pvLinkDirectionModel->index(row, 0);
    data = pvLinkDirectionModel->data(index);
    if(data == code){
      cbLinkDirection->setCurrentIndex(row);
      return;
    }
  }
}

QVariant mdtClUnitLinkDialog::linkDirectionCode() const
{
  QModelIndex index;
  int row;
  QVariant data;

  row = cbLinkDirection->currentIndex();
  if(row < 0){
    return data;
  }
  index = pvLinkDirectionModel->index(row, 0);
  data = pvLinkDirectionModel->data(index);

  return data;
}

void mdtClUnitLinkDialog::setLinkData(mdtClLinkData &data)
{
  pvLinkData = data;
  leIdentification->setText(data.identification().toString());
  updateModificationCombobox(data.modification());
  updateSinceVersionCombobox(data.sinceVersion());
  setLinkTypeCode(data.linkTypeCode());
  setLinkDirectionCode(data.linkDirectionCode());
  sbValue->setValue(data.value().toDouble());
}

mdtClLinkData &mdtClUnitLinkDialog::linkData()
{
  return pvLinkData;
}

void mdtClUnitLinkDialog::onCbLinkTypeCurrentIndexChanged(int row)
{
  QModelIndex index;
  QVariant data;

  if(row < 0){
    lbUnit->setText("");
    return;
  }
  // We must update available directions regarding link type
  index = pvLinkTypeModel->index(row, 0);
  data = pvLinkTypeModel->data(index);
  if(data == QVariant("DIODE")){
    pvLinkDirectionModel->setQuery("SELECT Code_PK, NameEN, PictureAscii FROM LinkDirection_tbl WHERE Code_PK <> 'BID'", pvDatabase);
    cbLinkDirection->setEnabled(true);
    sbValue->setValue(0.7);
  }else{
    pvLinkDirectionModel->setQuery("SELECT Code_PK, NameEN, PictureAscii FROM LinkDirection_tbl WHERE Code_PK = 'BID'", pvDatabase);
    cbLinkDirection->setEnabled(false);
    sbValue->setValue(0.0);
  }
  // Update displayed unit (V, Ohm, ...)
  index = pvLinkTypeModel->index(row, 2);
  data = pvLinkTypeModel->data(index);
  lbUnit->setText("[" + data.toString() + "]");
  // Update link data
  pvLinkData.setLinkTypeCode(linkTypeCode());
}

void mdtClUnitLinkDialog::onCbLinkDirectionCurrentIndexChanged(int row)
{
  QModelIndex index;
  QVariant data;

  if(row < 0){
    lbLinkDirectionAsciiPicture->setText("");
    return;
  }
  // Update the ASCII picture
  index = pvLinkDirectionModel->index(row, 2);
  data = pvLinkDirectionModel->data(index);
  lbLinkDirectionAsciiPicture->setText(data.toString());
  // Update link data
  pvLinkData.setLinkDirectionCode(linkDirectionCode());
}

void mdtClUnitLinkDialog::selectStartUnit()
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  QSqlQueryModel model;
  QList<QVariant> result;

  // Setup and run query
  sql = "SELECT * FROM Unit_view ";
  if(!pvStartUnitId.isNull()){
    sql += " WHERE Unit_Id_PK <> " + pvStartUnitId.toString();
  }
  model.setQuery(sql, pvDatabase);
  // Setup and show dialog
  selectionDialog.setMessage("Please select start unit");
  selectionDialog.setModel(&model, false);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_PK", true);
  selectionDialog.setHeaderData("Vehicle", tr("Type"));
  selectionDialog.setHeaderData("SubType", tr("Sub type"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema position"));
  selectionDialog.setHeaderData("ArticleCode", tr("Article code"));
  selectionDialog.setHeaderData("DesignationEN", tr("Designation (ENG)"));
  selectionDialog.addSelectionResultColumn("Unit_Id_PK");
  selectionDialog.addSelectionResultColumn("SchemaPosition");
  selectionDialog.addSelectionResultColumn("Cabinet");
  selectionDialog.resize(800, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  result = selectionDialog.selectionResult();
  Q_ASSERT(result.size() == 3);
  // Store unit and update GUI
  setStartUnit(result.at(0), result.at(1), result.at(2));
  // We have to update vehicle types
  ///setStartVehicleTypes(pvStartUnitId);
  // Clear connections
  ///setStartConnectionData(mdtClUnitConnectionData());
}

void mdtClUnitLinkDialog::selectEndUnit()
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  QSqlQueryModel model;
  QList<QVariant> result;

  // Setup and run query
  sql = "SELECT * FROM Unit_view ";
  if(!pvEndUnitId.isNull()){
    sql += " WHERE Unit_Id_PK <> " + pvEndUnitId.toString();
  }
  model.setQuery(sql, pvDatabase);
  // Setup and show dialog
  selectionDialog.setMessage("Please select end unit");
  selectionDialog.setModel(&model, false);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_PK", true);
  selectionDialog.setHeaderData("Type", tr("Vehicle"));
  selectionDialog.setHeaderData("SubType", tr("Sub type"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema position"));
  selectionDialog.setHeaderData("ArticleCode", tr("Article code"));
  selectionDialog.setHeaderData("DesignationEN", tr("Designation (ENG)"));
  selectionDialog.addSelectionResultColumn("Unit_Id_PK");
  selectionDialog.addSelectionResultColumn("SchemaPosition");
  selectionDialog.addSelectionResultColumn("Cabinet");
  selectionDialog.resize(800, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  result = selectionDialog.selectionResult();
  Q_ASSERT(result.size() == 3);
  // Store unit and update GUI
  setEndUnit(result.at(0), result.at(1), result.at(2));
  // We have to update vehicle types
  ///setStartVehicleTypes(pvStartUnitId);
  // Clear connections
  ///setStartConnectionData(mdtClUnitConnectionData());
}

void mdtClUnitLinkDialog::selectStartConnection()
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  QSqlQueryModel model;
  QList<QVariant> result;
  mdtClUnitConnectionData data;
  mdtClUnit unit(pvDatabase);

  // Setup and run query
  sql = "SELECT * FROM UnitConnection_view WHERE Unit_Id_FK = " + pvStartUnitId.toString();
  model.setQuery(sql, pvDatabase);
  // Setup and show dialog
  selectionDialog.setMessage("Please select start connection");
  selectionDialog.setModel(&model, false);
  selectionDialog.setColumnHidden("UnitConnection_Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnection_Id_FK", true);
  selectionDialog.setHeaderData("SchemaPage", tr("Schema\npage"));
  selectionDialog.setHeaderData("UnitFunctionEN", tr("Unit\nfunction (ENG)"));
  selectionDialog.setHeaderData("SignalName", tr("Signal name"));
  selectionDialog.setHeaderData("SwAddress", tr("SW address"));
  selectionDialog.setHeaderData("UnitConnectorName", tr("Unit\nconnector"));
  selectionDialog.setHeaderData("UnitContactName", tr("Unit\ncontact"));
  selectionDialog.setHeaderData("ArticleConnectorName", tr("Article\nconnector"));
  selectionDialog.setHeaderData("ArticleContactName", tr("Article\ncontact"));
  selectionDialog.setHeaderData("IoType", tr("I/O type"));
  selectionDialog.setHeaderData("ArticleFunctionEN", tr("Article\nfunction (ENG)"));
  selectionDialog.addSelectionResultColumn("UnitConnection_Id_PK");
  selectionDialog.resize(700, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  result = selectionDialog.selectionResult();
  Q_ASSERT(result.size() == 1);
  // Get connection data and update
  data = unit.getUnitConnectionData(result.at(0));
  setStartConnectionData(data);
}

void mdtClUnitLinkDialog::selectEndConnection()
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  QSqlQueryModel model;
  QList<QVariant> result;
  mdtClUnitConnectionData data;
  mdtClUnit unit(pvDatabase);

  // Setup and run query
  sql = "SELECT * FROM UnitConnection_view WHERE Unit_Id_FK = " + pvEndUnitId.toString();
  model.setQuery(sql, pvDatabase);
  // Setup and show dialog
  selectionDialog.setMessage("Please select start connection");
  selectionDialog.setModel(&model, false);
  selectionDialog.setColumnHidden("UnitConnection_Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnection_Id_FK", true);
  selectionDialog.setHeaderData("SchemaPage", tr("Schema\npage"));
  selectionDialog.setHeaderData("UnitFunctionEN", tr("Unit\nfunction (ENG)"));
  selectionDialog.setHeaderData("SignalName", tr("Signal name"));
  selectionDialog.setHeaderData("SwAddress", tr("SW address"));
  selectionDialog.setHeaderData("UnitConnectorName", tr("Unit\nconnector"));
  selectionDialog.setHeaderData("UnitContactName", tr("Unit\ncontact"));
  selectionDialog.setHeaderData("ArticleConnectorName", tr("Article\nconnector"));
  selectionDialog.setHeaderData("ArticleContactName", tr("Article\ncontact"));
  selectionDialog.setHeaderData("IoType", tr("I/O type"));
  selectionDialog.setHeaderData("ArticleFunctionEN", tr("Article\nfunction (ENG)"));
  selectionDialog.addSelectionResultColumn("UnitConnection_Id_PK");
  selectionDialog.resize(700, 400);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  result = selectionDialog.selectionResult();
  Q_ASSERT(result.size() == 1);
  // Get connection data and update
  data = unit.getUnitConnectionData(result.at(0));
  setEndConnectionData(data);
}

void mdtClUnitLinkDialog::selectStartVehicleTypes()
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  QSqlQueryModel model;
  QSqlError sqlError;
  int row;
  QModelIndex index;
  QModelIndexList vehicleIdList;
  QVariant data;

  // Check that start unit ID is set
  if(pvStartUnitId.isNull()){
    QMessageBox msgBox;
    msgBox.setText(tr("Unit is not set for start connection."));
    msgBox.setInformativeText(tr("Please select a unit before choosing vehicle types to use."));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }
  // Setup and run query
  sql = "SELECT Type, SubType, SeriesNumber, VehicleType_Id_FK FROM Unit_VehicleType_view WHERE Unit_Id_FK = " + pvStartUnitId.toString();
  sql += " ORDER BY VehicleType_Id_FK ASC";
  model.setQuery(sql, pvDatabase);
  // Setup and show dialog
  selectionDialog.setMessage("Please select start vehicles");
  selectionDialog.setModel(&model, true);
  selectionDialog.setAllowEmptyResult(true);
  for(row = 0; row < pvStartVehicleTypesModel->rowCount(); ++row){
    index = pvStartVehicleTypesModel->index(row, 3);
    data = pvStartVehicleTypesModel->data(index);
    selectionDialog.selectRows("VehicleType_Id_FK", data);
  }
  selectionDialog.setColumnHidden("VehicleType_Id_FK", true);
  selectionDialog.setHeaderData("SubType", tr("Sub type"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("VehicleType_Id_FK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  vehicleIdList = selectionDialog.selectionResults();
  // Set new query to vehicle types model
  if(vehicleIdList.isEmpty()){
    sql = "SELECT Type, SubType, SeriesNumber, VehicleType_Id_FK FROM Unit_VehicleType_view WHERE VehicleType_Id_FK = -1";
  }else{
    sql = "SELECT Type, SubType, SeriesNumber, VehicleType_Id_FK FROM Unit_VehicleType_view ";
    for(row = 0; row < vehicleIdList.size(); ++row){
      if(row == 0){
        sql += " WHERE ( ";
      }else{
        sql += " OR ";
      }
      sql += " VehicleType_Id_FK = " + vehicleIdList.at(row).data().toString();
    }
    sql += " ) AND Unit_Id_FK = " + pvStartUnitId.toString();
  }
  sql += " ORDER BY VehicleType_Id_FK ASC";
  pvStartVehicleTypesModel->setQuery(sql, pvDatabase);
  sqlError = pvStartVehicleTypesModel->lastError();
  if(sqlError.isValid()){
    mdtError e(MDT_DATABASE_ERROR, "Cannot get start vehicle type list.", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitLinkDialog");
    e.commit();
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot get start vehicle type list."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  twStartVehicles->resizeColumnsToContents();
}

void mdtClUnitLinkDialog::selectEndVehicleTypes()
{
  mdtSqlSelectionDialog selectionDialog(this);
  QString sql;
  QSqlQueryModel model;
  QSqlError sqlError;
  int row;
  QModelIndex index;
  QModelIndexList vehicleIdList;
  QVariant data;

  // Check that end unit ID is set
  if(pvEndUnitId.isNull()){
    QMessageBox msgBox;
    msgBox.setText(tr("Unit is not set for end connection."));
    msgBox.setInformativeText(tr("Please select a unit before choosing vehicle types to use."));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }
  // Setup and run query
  sql = "SELECT Type, SubType, SeriesNumber, VehicleType_Id_FK FROM Unit_VehicleType_view WHERE Unit_Id_FK = " + pvEndUnitId.toString();
  sql += " ORDER BY VehicleType_Id_FK ASC";
  model.setQuery(sql, pvDatabase);
  // Setup and show dialog
  selectionDialog.setMessage("Please select end vehicles");
  selectionDialog.setModel(&model, true);
  selectionDialog.setAllowEmptyResult(true);
  for(row = 0; row < pvEndVehicleTypesModel->rowCount(); ++row){
    index = pvEndVehicleTypesModel->index(row, 3);
    data = pvEndVehicleTypesModel->data(index);
    selectionDialog.selectRows("VehicleType_Id_FK", data);
  }
  selectionDialog.setColumnHidden("VehicleType_Id_FK", true);
  selectionDialog.setHeaderData("SubType", tr("Sub type"));
  selectionDialog.setHeaderData("SeriesNumber", tr("Serie"));
  selectionDialog.addSelectionResultColumn("VehicleType_Id_FK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  vehicleIdList = selectionDialog.selectionResults();
  // Set new query to vehicle types model
  if(vehicleIdList.isEmpty()){
    sql = "SELECT Type, SubType, SeriesNumber, VehicleType_Id_FK FROM Unit_VehicleType_view WHERE VehicleType_Id_FK = -1";
  }else{
    sql = "SELECT Type, SubType, SeriesNumber, VehicleType_Id_FK FROM Unit_VehicleType_view ";
    for(row = 0; row < vehicleIdList.size(); ++row){
      if(row == 0){
        sql += " WHERE ( ";
      }else{
        sql += " OR ";
      }
      sql += " VehicleType_Id_FK = " + vehicleIdList.at(row).data().toString();
    }
    sql += " ) AND Unit_Id_FK = " + pvEndUnitId.toString();
  }
  sql += " ORDER BY VehicleType_Id_FK ASC";
  pvEndVehicleTypesModel->setQuery(sql, pvDatabase);
  sqlError = pvEndVehicleTypesModel->lastError();
  if(sqlError.isValid()){
    mdtError e(MDT_DATABASE_ERROR, "Cannot get end vehicle type list.", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClUnitLinkDialog");
    e.commit();
    QMessageBox msgBox;
    msgBox.setText(tr("Cannot get end vehicle type list."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  twEndVehicles->resizeColumnsToContents();
}

void mdtClUnitLinkDialog::accept()
{
  // Most of data are dynamically stored during edition, but some are to do here
  pvLinkData.setIdentification(leIdentification->text());
  pvLinkData.setModification(cbModification->currentText());
  pvLinkData.setSinceVersion(cbSinceVersion->currentText());
  pvLinkData.setValue(sbValue->value());
  
  /// \todo checks
  
  QDialog::accept();
}

void mdtClUnitLinkDialog::updateModificationCombobox(const QVariant &data)
{
  int row;

  for(row = 0; row < cbModification->count(); ++row){
    if(cbModification->itemData(row) == data){
      cbModification->setCurrentIndex(row);
      return;
    }
  }
  // Data not found
  cbModification->setCurrentIndex(-1);
}

void mdtClUnitLinkDialog::updateSinceVersionCombobox(const QVariant &data)
{
  int row;

  for(row = 0; row < cbSinceVersion->count(); ++row){
    if(cbSinceVersion->itemData(row) == data){
      cbSinceVersion->setCurrentIndex(row);
      return;
    }
  }
  // Data not found
  cbSinceVersion->setCurrentIndex(-1);
}
