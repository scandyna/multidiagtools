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
#include "mdtClLinkData.h"

mdtClLinkData::mdtClLinkData()
 : mdtSqlRecord()
{
}

mdtClLinkData::mdtClLinkData(const QSqlRecord & record)
 : mdtSqlRecord(record)
{
  Q_ASSERT(indexOf("UnitConnectionStart_Id_FK") >= 0);
  Q_ASSERT(indexOf("UnitConnectionEnd_Id_FK") >= 0);
  Q_ASSERT(indexOf("LinkType_Code_FK") >= 0);
  Q_ASSERT(indexOf("LinkDirection_Code_FK") >= 0);
  Q_ASSERT(indexOf("ArticleConnectionStart_Id_FK") >= 0);
  Q_ASSERT(indexOf("ArticleConnectionEnd_Id_FK") >= 0);
  Q_ASSERT(indexOf("Identification") >= 0);
  Q_ASSERT(indexOf("SinceVersion") >= 0);
  Q_ASSERT(indexOf("Modification") >= 0);
  Q_ASSERT(indexOf("Value") >= 0);
}

bool mdtClLinkData::setup(const QSqlDatabase & db) 
{
  return addAllFields("Link_tbl", db);
}

void mdtClLinkData::clearValues()
{
  mdtSqlRecord::clearValues();
  pvStartConnectionData.clearValues();
  pvEndConnectionData.clearValues();
  pvVehicleTypeLinkDataList.clear();
}

void mdtClLinkData::setConnectionData(const mdtClUnitConnectionData & startCnnData, const mdtClUnitConnectionData & endCnnData)
{
  int i;

  setValue("UnitConnectionStart_Id_FK", startCnnData.value("Id_PK"));
  setValue("UnitConnectionEnd_Id_FK", endCnnData.value("Id_PK"));
  setValue("ArticleConnectionStart_Id_FK", startCnnData.value("ArticleConnection_Id_FK"));
  setValue("ArticleConnectionEnd_Id_FK", endCnnData.value("ArticleConnection_Id_FK"));
  pvStartConnectionData = startCnnData;
  pvEndConnectionData = endCnnData;
  for(i = 0; i < pvVehicleTypeLinkDataList.size(); ++i){
    pvVehicleTypeLinkDataList[i].setUnitConnectionStartId(value("UnitConnectionStart_Id_FK"));
    pvVehicleTypeLinkDataList[i].setUnitConnectionEndId(value("UnitConnectionEnd_Id_FK"));
  }
}

void mdtClLinkData::addVehicleTypeLinkData(const mdtClVehicleTypeLinkData& data)
{
  mdtClVehicleTypeLinkData _data = data;

  _data.setUnitConnectionStartId(value("UnitConnectionStart_Id_FK"));
  _data.setUnitConnectionEndId(value("UnitConnectionEnd_Id_FK"));
  pvVehicleTypeLinkDataList.append(_data);
}

void mdtClLinkData::clearVehicleTypeLinkDataList()
{
  pvVehicleTypeLinkDataList.clear();
}
