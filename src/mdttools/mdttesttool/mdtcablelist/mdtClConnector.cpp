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
#include "mdtClConnector.h"
#include "mdtSqlRecord.h"
#include <QSqlRecord>
#include <QSqlQuery>
#include <QList>

mdtClConnector::mdtClConnector(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtClConnector::mdtClConnector(QSqlDatabase db)
 : mdtTtBase(nullptr, db)
{
}

mdtClConnectorKeyData mdtClConnector::addConnector(const mdtClConnectorData & data, bool handleTransaction)
{
  mdtClConnectorKeyData key;
  mdtSqlRecord record;
  QSqlQuery query(database());

  // Setup record with given data
  if(!record.addAllFields("Connector_tbl", database())){
    pvLastError = record.lastError();
    return key;
  }
  record.setValue("Id_PK", data.keyData.id);
  record.setValue("Gender", data.gender);
  record.setValue("Form", data.form);
  record.setValue("Manufacturer", data.manufacturer);
  record.setValue("ManufacturerConfigCode", data.manufacturerConfigCode);
  record.setValue("ManufacturerArticleCode", data.manufacturerArticleCode);
  // Save to database
  if(handleTransaction){
    if(!beginTransaction()){
      return key;
    }
  }
  if(!addRecord(record, "Connector_tbl", query)){
    if(handleTransaction){
      rollbackTransaction();
    }
    return key;
  }
  key.id = query.lastInsertId();
  if(handleTransaction){
    if(!commitTransaction()){
      key.clear();
      return key;
    }
  }

  return key;
}

mdtClConnectorData mdtClConnector::getConnectorData(const mdtClConnectorKeyData & key, bool & ok)
{
  mdtClConnectorData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT * FROM Connector_tbl WHERE Id_PK = " + key.id.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data.keyData = key;
  data.gender = dataList.at(0).value("Gender");
  data.form = dataList.at(0).value("Form");
  data.manufacturer = dataList.at(0).value("Manufacturer");
  data.manufacturerConfigCode = dataList.at(0).value("ManufacturerConfigCode");
  data.manufacturerArticleCode = dataList.at(0).value("ManufacturerArticleCode");

  return data;
}

bool mdtClConnector::removeConnector(const mdtClConnectorKeyData & key)
{
  return removeData("Connector_tbl", "Id_PK", key.id.toString());
}
