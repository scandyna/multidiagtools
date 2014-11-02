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
#include "mdtTtTestModelItemRouteData.h"

mdtTtTestModelItemRouteData::mdtTtTestModelItemRouteData()
{
}

mdtTtTestModelItemRouteData::mdtTtTestModelItemRouteData(const QSqlRecord& record)
{
  Q_ASSERT(record.contains("Id_PK"));
  Q_ASSERT(record.contains("TestModelItem_Id_FK"));
  Q_ASSERT(record.contains("TestLink_Id_FK"));
  Q_ASSERT(record.contains("MeasureTestNodeUnitConnection_Id_FK"));

  // Set data
  setId(record.value("Id_PK"));
  setTestModelItemId(record.value("TestModelItem_Id_FK"));
  setTestLinkId(record.value("TestLink_Id_FK"));
  setMeasureConnectionId(record.value("MeasureTestNodeUnitConnection_Id_FK"));
  // If record contains additionnal fields, seup data
  if(record.contains("TestConnection_Id_FK")){
    setTestConnectionId(record.value("TestConnection_Id_FK"));
  }
  if(record.contains("DutConnection_Id_FK")){
    setDutConnectionId(record.value("DutConnection_Id_FK"));
  }
}

void mdtTtTestModelItemRouteData::clearData()
{
  pvId.clear();
  pvTestModelItemId.clear();
  pvTestLinkId.clear();
  pvDutConnectionId.clear();
  pvTestConnectionId.clear();
  pvMeasureConnectionId.clear();
  pvSetupDataList.clear();
}

void mdtTtTestModelItemRouteData::setId(const QVariant& id)
{
  int i;

  pvId = id;
  for(i = 0; i < pvSetupDataList.size(); ++i){
    pvSetupDataList[i].setValue("TestModelItemRoute_Id_FK", pvId);
  }
}

void mdtTtTestModelItemRouteData::setTestModelItemId(const QVariant& id)
{
  int i;

  pvTestModelItemId = id;
  for(i = 0; i < pvSetupDataList.size(); ++i){
    pvSetupDataList[i].setValue("TestModelItem_Id_FK", pvTestModelItemId);
  }
}

void mdtTtTestModelItemRouteData::setTestLinkId(const QVariant& id)
{
  pvTestLinkId = id;
}

void mdtTtTestModelItemRouteData::setDutConnectionId(const QVariant& id)
{
  pvDutConnectionId = id;
}

void mdtTtTestModelItemRouteData::setTestConnectionId(const QVariant& id)
{
  pvTestConnectionId = id;
}

void mdtTtTestModelItemRouteData::setMeasureConnectionId(const QVariant& id)
{
  pvMeasureConnectionId = id;
}

void mdtTtTestModelItemRouteData::addSetupData(mdtTtTestNodeUnitSetupData data)
{
  Q_ASSERT(data.contains("TestModelItem_Id_FK"));
  Q_ASSERT(data.contains("TestModelItemRoute_Id_FK"));

  data.setValue("TestModelItem_Id_FK", pvTestModelItemId);
  data.setValue("TestModelItemRoute_Id_FK", pvId);
  pvSetupDataList.append(data);
}

mdtValue mdtTtTestModelItemRouteData::routeResistance() const
{
  int i;
  double r = 0.0;

  for(i = 0; i < pvSetupDataList.size(); ++i){
    r += pvSetupDataList.at(i).value("CalibrationOffset").toDouble();
  }

  return r;
}

mdtValue mdtTtTestModelItemRouteData::busCouplingRelaysResistance() const
{
  int i;
  double r = 0.0;

  for(i = 0; i < pvSetupDataList.size(); ++i){
    if(pvSetupDataList.at(i).value("Type_Code_FK").toString() == "BUSCPLREL"){
      r += pvSetupDataList.at(i).value("CalibrationOffset").toDouble();
    }
  }

  return r;
}

mdtValue mdtTtTestModelItemRouteData::channelRelaysResistance() const
{
  int i;
  double r = 0.0;

  for(i = 0; i < pvSetupDataList.size(); ++i){
    if(pvSetupDataList.at(i).value("Type_Code_FK").toString() == "CHANELREL"){
      r += pvSetupDataList.at(i).value("CalibrationOffset").toDouble();
    }
  }

  return r;
}

QList< QVariant > mdtTtTestModelItemRouteData::busCouplingRelaysIdList() const
{
  int i;
  QList<QVariant> idList;

  for(i = 0; i < pvSetupDataList.size(); ++i){
    if(pvSetupDataList.at(i).value("Type_Code_FK").toString() == "BUSCPLREL"){
      Q_ASSERT(!pvSetupDataList.at(i).value("TestNodeUnit_Id_FK").isNull());
      idList << pvSetupDataList.at(i).value("TestNodeUnit_Id_FK");
    }
  }

  return idList;
}

QList< QVariant > mdtTtTestModelItemRouteData::channelRelaysIdList() const
{
  int i;
  QList<QVariant> idList;

  for(i = 0; i < pvSetupDataList.size(); ++i){
    if(pvSetupDataList.at(i).value("Type_Code_FK").toString() == "CHANELREL"){
      Q_ASSERT(!pvSetupDataList.at(i).value("TestNodeUnit_Id_FK").isNull());
      idList << pvSetupDataList.at(i).value("TestNodeUnit_Id_FK");
    }
  }

  return idList;
}
