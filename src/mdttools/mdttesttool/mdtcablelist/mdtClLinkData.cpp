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
#include "mdtClLinkData.h"

void mdtClLinkData::clear()
{
  pvKeyData.clear();
  identification.clear();
  resistance.clear();
  length.clear();
}

/*
 * OLD stuff
 */
mdtClLinkData::mdtClLinkData()
 : mdtSqlRecord()
{
  pvVehicleTypeLinksEdited = false;
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
  Q_ASSERT(contains("Resistance") >= 0);
  clearVehicleTypeLinkDataList();
  /*
   * Currently, mdtClUnit::getArticleLinkListUsingConnectionId() takes data from
   *  ArticleLink_UnitConnection_view, witch doesen't contain LinkBeam_Id_FK.
   *  So, we let this field optional here for the moment.
   */
  //Q_ASSERT(contains("LinkBeam_Id_FK"));
}

bool mdtClLinkData::setup(const QSqlDatabase & db) 
{
  clear();
  pvStartConnectionData.clear();
  pvEndConnectionData.clear();
  if(!addAllFields("Link_tbl", db)){
    return false;
  }
  if(!pvStartConnectionData.setup(db, true)){
    return false;
  }
  if(!pvEndConnectionData.setup(db, true)){
    return false;
  }
  return true;
}

void mdtClLinkData::clearValues()
{
  mdtSqlRecord::clearValues();
  pvStartConnectionData.clearValues();
  pvEndConnectionData.clearValues();
  ///pvVehicleTypeLinkDataList.clear();
  clearVehicleTypeLinkDataList();
}

void mdtClLinkData::setStartConnectionData(const mdtClUnitConnectionData & cnnData)
{
  int i;

  setValue("UnitConnectionStart_Id_FK", cnnData.value("Id_PK"));
  setValue("ArticleConnectionStart_Id_FK", cnnData.value("ArticleConnection_Id_FK"));
  pvStartConnectionData = cnnData;
  for(i = 0; i < pvVehicleTypeLinkDataList.size(); ++i){
    pvVehicleTypeLinkDataList[i].setUnitConnectionStartId(value("UnitConnectionStart_Id_FK"));
  }
}

void mdtClLinkData::setEndConnectionData(const mdtClUnitConnectionData & cnnData)
{
  int i;

  setValue("UnitConnectionEnd_Id_FK", cnnData.value("Id_PK"));
  setValue("ArticleConnectionEnd_Id_FK", cnnData.value("ArticleConnection_Id_FK"));
  pvEndConnectionData = cnnData;
  for(i = 0; i < pvVehicleTypeLinkDataList.size(); ++i){
    pvVehicleTypeLinkDataList[i].setUnitConnectionEndId(value("UnitConnectionEnd_Id_FK"));
  }
}

void mdtClLinkData::setConnectionData(const mdtClUnitConnectionData & startCnnData, const mdtClUnitConnectionData & endCnnData)
{
  setStartConnectionData(startCnnData);
  setEndConnectionData(endCnnData);
}

void mdtClLinkData::addVehicleTypeLinkData(const mdtClVehicleTypeLinkData & data, bool updateVehicleLinksEditedFlags)
{
  mdtClVehicleTypeLinkData _data = data;

  _data.setUnitConnectionStartId(value("UnitConnectionStart_Id_FK"));
  _data.setUnitConnectionEndId(value("UnitConnectionEnd_Id_FK"));
  pvVehicleTypeLinkDataList.append(_data);
  if(updateVehicleLinksEditedFlags){
    pvVehicleTypeLinksEdited = true;
  }
}

void mdtClLinkData::clearVehicleTypeLinkDataList()
{
  pvVehicleTypeLinkDataList.clear();
  pvVehicleTypeLinksEdited = false;
}
