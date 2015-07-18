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
#include "mdtClVehicleTypeLinkAssignationWidget.h"
#include "mdtClVehicleTypeCheckBox.h"
#include "mdtClUnitVehicleType.h"
#include "mdtClVehicleTypeLink.h"
#include <QVBoxLayout>

//#include <QDebug>

mdtClVehicleTypeLinkAssignationWidget::mdtClVehicleTypeLinkAssignationWidget(QWidget *parent, QSqlDatabase db)
 : QWidget(parent),
   pvDatabase(db),
   pvLayout(new QVBoxLayout)
{
  pvLayout->addStretch(1);
  setLayout(pvLayout);
}

bool mdtClVehicleTypeLinkAssignationWidget::buildVehicleTypeList(const QVariant & startUnitId, const QVariant & endUnitId)
{
  Q_ASSERT(!startUnitId.isNull());
  Q_ASSERT(!endUnitId.isNull());

  mdtClUnitVehicleType uvt(pvDatabase);
  QList<QSqlRecord> vehicleTypeDataList;
  QString sql;
  bool ok;

  clear();
  // Get vehicle type data list
  sql = "SELECT\n"\
        " VTUS.Unit_Id_FK AS StartUnit_Id_FK,\n"\
        " VTUE.Unit_Id_FK AS EndUnit_Id_FK,\n"\
        " VTUS.VehicleType_Id_FK,\n"\
        " VT.Type,\n"\
        " VT.SubType,\n"\
        " VT.SeriesNumber\n"\
        " FROM VehicleType_Unit_tbl VTUS\n"\
        "  JOIN VehicleType_Unit_tbl VTUE\n"\
        "   ON VTUS.VehicleType_Id_FK = VTUE.VehicleType_Id_FK\n"\
        "  JOIN VehicleType_tbl VT\n"\
        "   ON VT.Id_PK = VTUS.VehicleType_Id_FK\n"\
        " WHERE StartUnit_Id_FK = " + startUnitId.toString() + " AND EndUnit_Id_FK = " + endUnitId.toString() + \
        " ORDER BY Type, SubType, SeriesNumber ASC";
  vehicleTypeDataList = uvt.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    pvLastError = uvt.lastError();
    return false;
  }
  // Build items
  for(const auto & vehicleTypeData : vehicleTypeDataList){
    mdtClVehicleTypeCheckBox *item = new mdtClVehicleTypeCheckBox(this, vehicleTypeData);
    addItem(item);
  }

  return true;
}

bool mdtClVehicleTypeLinkAssignationWidget::selectVehicleTypeAssignedToLink(const mdtClLinkPkData & linkPk)
{
  Q_ASSERT(!linkPk.isNull());

  mdtClVehicleTypeLink vtl(pvDatabase);
  QList<mdtClVehicleTypeStartEndKeyData> keyDataList;
  bool ok;

  // Get vehicle types assigned to given link
  keyDataList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  if(!ok){
    pvLastError = vtl.lastError();
    return false;
  }
  // Update items selected state
  for(const auto & key : keyDataList){
    setMatchingItemsSelected(key);
  }

  return true;
}

void mdtClVehicleTypeLinkAssignationWidget::unselectAllVehicleType()
{
  for(auto *item : pvItems){
    Q_ASSERT(item != nullptr);
    item->setChecked(false);
  }
}

void mdtClVehicleTypeLinkAssignationWidget::clear()
{
  for(auto *item : pvItems){
    Q_ASSERT(item != nullptr);
    pvLayout->removeWidget(item);
    delete item;
  }
  pvItems.clear();
}

QList<mdtClVehicleTypeStartEndKeyData> mdtClVehicleTypeLinkAssignationWidget::getSelectedVehicleTypeList() const
{
  QList<mdtClVehicleTypeStartEndKeyData> keyList;

  for(auto *item : pvItems){
    Q_ASSERT(item != nullptr);
    if(item->isChecked()){
      mdtClVehicleTypeStartEndKeyData key;
      key.setVehicleTypeStartId(item->vehicleTypeId());
      key.setVehicleTypeEndId(item->vehicleTypeId());
      keyList.append(key);
    }
  }

  return keyList;
}


void mdtClVehicleTypeLinkAssignationWidget::addItem(mdtClVehicleTypeCheckBox *item)
{
  Q_ASSERT(pvLayout->count() > 0);  // A stretch was added in constructor

  pvLayout->insertWidget(pvLayout->count()-1, item);
//   pvLayout->addWidget(item);
  pvItems.push_back(item);
}

void mdtClVehicleTypeLinkAssignationWidget::setMatchingItemsSelected(const mdtClVehicleTypeStartEndKeyData & key)
{
  Q_ASSERT(!key.isNull());

  for(auto *item : pvItems){
    Q_ASSERT(item != nullptr);
    QVariant id = item->vehicleTypeId();
    Q_ASSERT(!id.isNull());
    if( (key.vehicleTypeStartId() == id) && (key.vehicleTypeEndId() == id) ){
      item->setChecked(true);
    }
  }
}
