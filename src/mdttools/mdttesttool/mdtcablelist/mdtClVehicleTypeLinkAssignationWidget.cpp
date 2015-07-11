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

#include <QDebug>

mdtClVehicleTypeLinkAssignationWidget::mdtClVehicleTypeLinkAssignationWidget(QWidget *parent, QSqlDatabase db)
 : QWidget(parent),
   pvDatabase(db),
   pvLayout(new QVBoxLayout)
{
  pvLayout->addStretch(1);
  setLayout(pvLayout);
}

bool mdtClVehicleTypeLinkAssignationWidget::buildList(const QVariant & unitId, const mdtClLinkPkData & linkPk)
{
  Q_ASSERT(!unitId.isNull());
  Q_ASSERT(!linkPk.isNull());

  clear();
  if(!buildVehicleTypeList(unitId)){
    return false;
  }
  if(!selectVehicleTypeAssignedToLink(linkPk)){
    return false;
  }

  return true;
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

bool mdtClVehicleTypeLinkAssignationWidget::buildVehicleTypeList(const QVariant & unitId)
{
  mdtClUnitVehicleType uvt(pvDatabase);
  QList<QSqlRecord> vehicleTypeDataList;
  bool ok;

  // Get vehicle type data list
  vehicleTypeDataList = uvt.getVehicleTypeDataList(unitId, ok);
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
  mdtClVehicleTypeLink vtl(pvDatabase);
  QList<mdtClVehicleTypeStartEndKeyData> keydataList;
  bool ok;

  // Get vehicle types assigned to given link
  keydataList = vtl.getVehicleTypeStartEndKeyDataList(linkPk, ok);
  if(!ok){
    pvLastError = vtl.lastError();
    return false;
  }
  // Update items selected state
  for(const auto & key : keydataList){
    setMatchingItemsSelected(key);
  }

  return true;
}

void mdtClVehicleTypeLinkAssignationWidget::addItem(mdtClVehicleTypeCheckBox *item)
{
  pvLayout->addWidget(item);
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

void mdtClVehicleTypeLinkAssignationWidget::clear()
{
  for(auto *item : pvItems){
    Q_ASSERT(item != nullptr);
    pvLayout->removeWidget(item);
  }
}
