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
#include "mdtClArticleConnectorData.h"
#include <QString>

bool mdtClArticleConnectorData::setup(const QSqlDatabase & db, bool setupCd)
{
  if(!addAllFields("ArticleConnector_tbl", db)){
    return false;
  }
  if(setupCd){
    return pvConnectorData.setup(db);
  }
  return true;
}

void mdtClArticleConnectorData::setConnectionDataList(const QList<mdtClArticleConnectionData> & dataList) 
{
  pvConnectionDataList = dataList;
}

QList<mdtClArticleConnectionData> mdtClArticleConnectorData::connectionDataList() const
{
  return pvConnectionDataList;
}

void mdtClArticleConnectorData::addConnectionData(const mdtClArticleConnectionData & data) 
{
  Q_ASSERT(data.indexOf("Id_PK") > 0);
  Q_ASSERT(data.indexOf("Article_Id_FK") > 0);
  Q_ASSERT(data.indexOf("ArticleConnector_Id_FK") > 0);
  Q_ASSERT(data.indexOf("ArticleContactName") > 0);
  Q_ASSERT(data.indexOf("IoType") > 0);
  Q_ASSERT(data.indexOf("FunctionEN") > 0);
  Q_ASSERT(data.indexOf("FunctionFR") > 0);
  Q_ASSERT(data.indexOf("FunctionDE") > 0);
  Q_ASSERT(data.indexOf("FunctionFR") > 0);

  pvConnectionDataList.append(data);
}

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

mdtClConnectorData mdtClArticleConnectorData::connectorData() const
{
  return pvConnectorData;
}
