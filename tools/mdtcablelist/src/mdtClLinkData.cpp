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
#include "mdtClLinkData.h"
#include <QObject>

#include <QDebug>

bool mdtClLinkData::isValid()
{
  if(pvUnitConnectionStartId.isNull()){
    return false;
  }
  if(pvUnitConnectionEndId.isNull()){
    return false;
  }
  if(pvLinkDirectionCode.isNull()){
    return false;
  }
  if(pvLinkTypeCode.isNull()){
    return false;
  }
  return true;
}

void mdtClLinkData::clear()
{
  pvArticleConnectionStartId.clear();
  pvArticleConnectionEndId.clear();
  pvUnitConnectionStartId.clear();
  pvUnitConnectionEndId.clear();
  pvSinceVersion.clear();
  pvModification.clear();
  pvIdentification.clear();
  pvLinkDirectionCode.clear();
  pvLinkTypeCode.clear();
  pvValue.clear();
  pvVehicleTypeStartIdList.clear();
  pvVehicleTypeEndIdList.clear();
  pvVehicleTypeStartEndIdList.clear();
  pvErrorList.clear();
}

/**
void mdtClLinkData::setArticleLinkId(const QVariant & value) 
{
  pvArticleLinkId = value;
}

QVariant mdtClLinkData::articleLinkId() const 
{
  return pvArticleLinkId;
}
*/

void mdtClLinkData::setArticleConnectionStartId(const QVariant & value)
{
  pvArticleConnectionStartId = value;
}

QVariant mdtClLinkData::articleConnectionStartId() const
{
  return pvArticleConnectionStartId;
}

void mdtClLinkData::setArticleConnectionEndId(const QVariant & value)
{
  pvArticleConnectionEndId = value;
}

QVariant mdtClLinkData::articleConnectionEndId() const
{
  return pvArticleConnectionEndId;
}

void mdtClLinkData::setUnitConnectionStartId(const QVariant & value) 
{
  pvUnitConnectionStartId = value;
}

QVariant mdtClLinkData::unitConnectionStartId() const 
{
  return pvUnitConnectionStartId;
}

void mdtClLinkData::setUnitConnectionEndId(const QVariant & value) 
{
  pvUnitConnectionEndId = value;
}

QVariant mdtClLinkData::unitConnectionEndId() const 
{
  return pvUnitConnectionEndId;
}

void mdtClLinkData::setSinceVersion(const QVariant & value) 
{
  pvSinceVersion = value;
}

QVariant mdtClLinkData::sinceVersion() const 
{
  return pvSinceVersion;
}

void mdtClLinkData::setModification(const QVariant & value) 
{
  pvModification = value;
}

QVariant mdtClLinkData::modification() const 
{
  return pvModification;
}

void mdtClLinkData::setIdentification(const QVariant & value) 
{
  pvIdentification = value;
}

QVariant mdtClLinkData::identification() const 
{
  return pvIdentification;
}

void mdtClLinkData::setLinkDirectionCode(const QVariant & value) 
{
  pvLinkDirectionCode = value;
}

QVariant mdtClLinkData::linkDirectionCode() const 
{
  return pvLinkDirectionCode;
}

void mdtClLinkData::setLinkTypeCode(const QVariant & value) 
{
  pvLinkTypeCode = value;
}

QVariant mdtClLinkData::linkTypeCode() const 
{
  return pvLinkTypeCode;
}

void mdtClLinkData::setValue(const QVariant & value) 
{
  pvValue = value;
}

QVariant mdtClLinkData::value() const 
{
  return pvValue;
}

void mdtClLinkData::setVehicleTypeStartIdList(const QList<QVariant> & idList) 
{
  pvVehicleTypeStartIdList = idList;
}

void mdtClLinkData::addVehicleTypeStartId(const QVariant &id)
{
  pvVehicleTypeStartIdList.append(id);
}

const QList<QVariant> &mdtClLinkData::vehicleTypeStartIdList() const 
{
  return pvVehicleTypeStartIdList;
}

void mdtClLinkData::setVehicleTypeEndIdList(const QList<QVariant> & idList) 
{
  pvVehicleTypeEndIdList = idList;
}

void mdtClLinkData::addVehicleTypeEndId(const QVariant &id)
{
  pvVehicleTypeEndIdList.append(id);
}

const QList<QVariant> &mdtClLinkData::vehicleTypeEndIdList() const 
{
  return pvVehicleTypeEndIdList;
}

bool mdtClLinkData::buildVehicleTypeStartEndIdList()
{
  QPair<QVariant, QVariant> item;
  QVariant id;
  int i;

  pvVehicleTypeStartEndIdList.clear();
  if(!checkVehicleTypes()){
    pvErrorList.clear();
    return false;
  }
  // Case of same size
  if(pvVehicleTypeStartIdList.size() == pvVehicleTypeEndIdList.size()){
    for(i = 0; i < pvVehicleTypeStartIdList.size(); ++i){
      item.first = pvVehicleTypeStartIdList.at(i);
      item.second = pvVehicleTypeEndIdList.at(i);
      pvVehicleTypeStartEndIdList.append(item);
    }
  }else{
    if(pvVehicleTypeStartIdList.size() == 1){
      id = pvVehicleTypeStartIdList.at(0);
      for(i = 0; i < pvVehicleTypeEndIdList.size(); ++i){
        item.first = id;
        item.second = pvVehicleTypeEndIdList.at(i);
        pvVehicleTypeStartEndIdList.append(item);
      }
    }else{
      Q_ASSERT(pvVehicleTypeEndIdList.size() == 1);
      id = pvVehicleTypeEndIdList.at(0);
      for(i = 0; i < pvVehicleTypeStartIdList.size(); ++i){
        item.first = pvVehicleTypeStartIdList.at(i);
        item.second = id;
        pvVehicleTypeStartEndIdList.append(item);
      }
    }
  }

  return true;
}

const QList<QPair<QVariant, QVariant> > mdtClLinkData::vehicleTypeStartEndIdList() const
{
  return pvVehicleTypeStartEndIdList;
}

bool mdtClLinkData::checkVehicleTypes()
{
  QStringList errorList;

  // Check about empty list
  if(pvVehicleTypeStartIdList.isEmpty()){
    errorList << QObject::tr("No vehicle type assigned to start connection");
  }
  if(pvVehicleTypeEndIdList.isEmpty()){
    errorList << QObject::tr("No vehicle type assigned to end connection");
  }
  if(!errorList.isEmpty()){
    pvErrorList.append(errorList);
    return false;
  }
  // If both lists have different size, check that one list has only one item
  if(pvVehicleTypeStartIdList.size() != pvVehicleTypeEndIdList.size()){
    if((pvVehicleTypeStartIdList.size() > 1)&&(pvVehicleTypeEndIdList.size() > 1)){
      errorList << QObject::tr("Linking many vehicle types from start connection to many vehicle types at end connection is not possible");
      pvErrorList.append(errorList);
      return false;
    }
  }

  return true;
}
