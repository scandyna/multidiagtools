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
#include "mdtClUnitConnectorData.h"
#include <QString>

bool mdtClUnitConnectorData::setup(const QSqlDatabase & db, bool setupCd, bool setupAcd)
{
  if(!addAllFields("UnitConnector_tbl", db)){
    return false;
  }
  if(setupCd){
    if(!pvConnectorData.setup(db)){
      return false;
    }
  }
  if(setupAcd){
    return pvArticleConnectorData.setup(db, setupCd);
  }
  return true;
}

void mdtClUnitConnectorData::setConnectionDataList(const QList<mdtClUnitConnectionData> & dataList) 
{
  pvConnectionDataList = dataList;
}

QList<mdtClUnitConnectionData> mdtClUnitConnectorData::connectionDataList() const
{
  return pvConnectionDataList;
}

void mdtClUnitConnectorData::addConnectionData(const mdtClUnitConnectionData & data) 
{
  Q_ASSERT(data.contains("Id_PK"));
  Q_ASSERT(data.contains("Unit_Id_FK"));
  Q_ASSERT(data.contains("UnitConnector_Id_FK"));
  Q_ASSERT(data.contains("ArticleConnection_Id_FK"));
  Q_ASSERT(data.contains("UnitContactName"));
  Q_ASSERT(data.contains("SchemaPage"));
  Q_ASSERT(data.contains("SignalName"));
  Q_ASSERT(data.contains("SwAddress"));
  Q_ASSERT(data.contains("FunctionEN"));
  Q_ASSERT(data.contains("FunctionFR"));
  Q_ASSERT(data.contains("FunctionDE"));
  Q_ASSERT(data.contains("FunctionFR"));

  pvConnectionDataList.append(data);
}

bool mdtClUnitConnectorData::setConnectionData(const QVariant & connectionId, const mdtClUnitConnectionData & data) 
{
  int i;

  for(i = 0; i < pvConnectionDataList.size(); ++i){
    if(pvConnectionDataList.at(i).value("Id_PK") == connectionId){
      pvConnectionDataList[i] = data;
      return true;
    }
  }
  // ID was not found
  pvLastError.setError("Could not find connection data with ID " + connectionId.toString(), mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtClUnitConnectorData");
  pvLastError.commit();

  return false;
}

mdtClUnitConnectionData mdtClUnitConnectorData::connectionData(const QVariant & connectionId, bool *ok) const
{
  Q_ASSERT(ok != 0);

  int i;

  for(i = 0; i < pvConnectionDataList.size(); ++i){
    if(pvConnectionDataList.at(i).value("Id_PK") == connectionId){
      *ok = true;
      return pvConnectionDataList.at(i);
    }
  }
  // ID was not found
  *ok = false;

  return mdtClUnitConnectionData();

}

void mdtClUnitConnectorData::setConnectorData(const mdtClConnectorData & data)
{
  setValue("Connector_Id_FK", data.value("Id_PK"));
  pvConnectorData = data;
}

mdtClConnectorData mdtClUnitConnectorData::connectorData() const
{
  return pvConnectorData;
}

mdtClArticleConnectorData mdtClUnitConnectorData::articleConnectorData() const
{
  return pvArticleConnectorData;
}

bool mdtClUnitConnectorData::isBasedOnArticleConnector() const
{
  return !value("ArticleConnection_Id_FK").isNull();
}

bool mdtClUnitConnectorData::isBasedOnConnector() const
{
  return !value("Connector_Id_FK").isNull();
}
