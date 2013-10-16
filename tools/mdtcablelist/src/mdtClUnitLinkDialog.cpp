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
  // Set default link type
  setLinkTypeCode("CABLELINK");
  // Setup vehicle types
  pvStartVehicleTypesModel = new QSqlQueryModel(this);
  twStartVehicles->setModel(pvStartVehicleTypesModel);

  // Setup start unit
  connect(pbStartUnit, SIGNAL(clicked()), this, SLOT(selectStartUnit()));
  
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

  pvStartUnitId = unitId;
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
  /// \todo Update connections + vehicles + Prevent infinite update loop ..
}

void mdtClUnitLinkDialog::setStartUnit(const QVariant &unitId, const QVariant &schemaPosition, const QVariant &cabinet)
{
  pvStartUnitId = unitId;
  lbStartSchemaPosition->setText(schemaPosition.toString());
  lbStartCabinet->setText(cabinet.toString());
}

QVariant mdtClUnitLinkDialog::startUnitId() const
{
  return pvStartUnitId;
}

void mdtClUnitLinkDialog::setStartConnectionData(const mdtClUnitConnectionData &data)
{
  qDebug() << "mdtClUnitLinkDialog::setStartConnectionData() : NOT implemented yet !!";
}

void mdtClUnitLinkDialog::setStartVehicleTypes(const QVariant &unitId)
{
  QSqlError sqlError;
  QString sql;

  pvStartUnitId = unitId;
  sql = "SELECT Type, SubType, SeriesNumber, VehicleType_Id_FK FROM Unit_VehicleType_view WHERE Unit_Id_FK = " + unitId.toString();
  pvStartVehicleTypesModel->setQuery(sql, pvDatabase);
  pvStartVehicleTypesModel->setHeaderData(0, Qt::Horizontal, tr("Type"));
  pvStartVehicleTypesModel->setHeaderData(1, Qt::Horizontal, tr("Sub type"));
  pvStartVehicleTypesModel->setHeaderData(2, Qt::Horizontal, tr("Serie"));
  twStartVehicles->setColumnHidden(3, true);
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
}

void mdtClUnitLinkDialog::selectStartUnit()
{
  mdtSqlSelectionDialog selectionDialog(this);

  // Setup and show dialog
  selectionDialog.setMessage("Please select start unit");
  ///selectionDialog.setModel(&uvt.vehicleTypeNotAssignedToUnitModel(unitId), true);
  ///selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("SubType", tr("Variant"));
  ///selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(500, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }

}
