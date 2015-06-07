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
#include "mdtClArticleConnectorData.h"
#include <QString>

//#include <QDebug>

void mdtClArticleConnectorData::setKeyData(const mdtClArticleConnectorKeyData & key)
{
  pvKeyData = key;
  for(auto & data : pvConnectionDataList){
    auto key = data.keyData();
    key.articleConnectorFk = pvKeyData;
    key.articleId = pvKeyData.articleId;
    data.setKeyData(key);
  }
}

void mdtClArticleConnectorData::clear()
{
  pvKeyData.clear();
  name.clear();
  pvConnectionDataList.clear();
}

void mdtClArticleConnectorData::addConnectionData(mdtClArticleConnectionData data)
{
  mdtClArticleConnectionKeyData key = data.keyData();

  key.articleConnectorFk = pvKeyData;
  key.articleId = pvKeyData.articleId;
  data.setKeyData(key);
  pvConnectionDataList.append(data);
}

void mdtClArticleConnectorData::setConnectionDataList(const QList<mdtClArticleConnectionData> & dataList)
{
  for(const auto & data : dataList){
    addConnectionData(data);
  }
}


/*
 * OLD
 */

mdtClArticleConnectorData::mdtClArticleConnectorData()
{
}

mdtClArticleConnectorData::mdtClArticleConnectorData(const QSqlRecord& record)
 : mdtSqlRecord(record)
{
  Q_ASSERT(contains("Id_PK"));
  Q_ASSERT(contains("Article_Id_FK"));
  Q_ASSERT(contains("Connector_Id_FK"));
  Q_ASSERT(contains("Name"));
}


bool mdtClArticleConnectorData::setup(const QSqlDatabase & db, bool setupCd)
{
  pvConnectionDataList.clear();
  if(!addAllFields("ArticleConnector_tbl", db)){
    return false;
  }
  if(setupCd){
    return pvConnectorData.setup(db);
  }
  return true;
}

void mdtClArticleConnectorData::clearValues()
{
  pvConnectionDataList.clear();
  pvConnectorData.clearValues();
  mdtSqlRecord::clearValues();
}

// void mdtClArticleConnectorData::clear()
// {
//   pvConnectionDataList.clear();
//   pvConnectorData.clear();
//   mdtSqlRecord::clear();
// }


// void mdtClArticleConnectorData::setConnectionDataList(const QList<mdtClArticleConnectionData> & dataList) 
// {
//   pvConnectionDataList = dataList;
// }

// const QList<mdtClArticleConnectionData> & mdtClArticleConnectorData::connectionDataList() const
// {
//   return pvConnectionDataList;
// }

// void mdtClArticleConnectorData::addConnectionData(const mdtClArticleConnectionData & data) 
// {
//   Q_ASSERT(data.contains("Id_PK"));
//   Q_ASSERT(data.contains("Article_Id_FK"));
//   Q_ASSERT(data.contains("ArticleConnector_Id_FK"));
//   Q_ASSERT(data.contains("ArticleContactName"));
//   Q_ASSERT(data.contains("IoType"));
//   Q_ASSERT(data.contains("FunctionEN"));
//   Q_ASSERT(data.contains("FunctionFR"));
//   Q_ASSERT(data.contains("FunctionDE"));
//   Q_ASSERT(data.contains("FunctionFR"));
// 
//   mdtClArticleConnectionData _data = data;
// 
//   _data.setValue("Article_Id_FK", value("Article_Id_FK"));
//   _data.setValue("ArticleConnector_Id_FK", value("Id_PK"));
//   pvConnectionDataList.append(_data);
// }

bool mdtClArticleConnectorData::setConnectionData(const QVariant & connectionId, const mdtClArticleConnectionData & data) 
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
  MDT_ERROR_SET_SRC(pvLastError, "mdtClArticleConnectorData");
  pvLastError.commit();

  return false;
}

mdtClArticleConnectionData mdtClArticleConnectorData::connectionData(const QVariant & connectionId, bool *ok) const
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

  return mdtClArticleConnectionData();

}

void mdtClArticleConnectorData::setConnectorData(const mdtClConnectorData & data)
{
  Q_ASSERT(!data.value("Id_PK").isNull());

  setValue("Connector_Id_FK", data.value("Id_PK"));
  pvConnectorData = data;
}

mdtClConnectorData mdtClArticleConnectorData::connectorData() const
{
  return pvConnectorData;
}

// bool mdtClArticleConnectorData::isBasedOnConnector() const
// {
//   return !value("Connector_Id_FK").isNull();
// }
