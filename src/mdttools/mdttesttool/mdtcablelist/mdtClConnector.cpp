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

#include <QDebug>

mdtClConnector::mdtClConnector(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtClConnector::mdtClConnector(QSqlDatabase db)
 : mdtTtBase(nullptr, db)
{
}

mdtClConnectionTypeData mdtClConnector::getConnectionTypeData(const QString & code, bool & ok)
{
  mdtClConnectionTypeData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT * FROM ConnectionType_tbl WHERE Code_PK = '" + code + "'";
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data.keyData.code = dataList.at(0).value("Code_PK");
  data.nameEN = dataList.at(0).value("nameEN");
  data.nameFR = dataList.at(0).value("nameFR");
  data.nameDE = dataList.at(0).value("nameDE");
  data.nameIT = dataList.at(0).value("nameIT");

  return data;
}

mdtClConnectorContactKeyData mdtClConnector::addContact(const mdtClConnectorContactData & data, bool handleTransaction)
{
  mdtClConnectorContactKeyData key;
  mdtSqlRecord record;
  QSqlQuery query(database());

  // Setup record with given data
  if(!record.addAllFields("ConnectorContact_tbl", database())){
    pvLastError = record.lastError();
    return key;
  }
  fillRecord(record, data);
  // Save to database
  if(handleTransaction){
    if(!beginTransaction()){
      return key;
    }
  }
  if(!addRecord(record, "ConnectorContact_tbl", query)){
    if(handleTransaction){
      rollbackTransaction();
    }
    return key;
  }
  key = data.keyData();
  key.id = query.lastInsertId();
  if(handleTransaction){
    if(!commitTransaction()){
      key.clear();
      return key;
    }
  }

  return key;
}

mdtClConnectorContactData mdtClConnector::getContactData(const mdtClConnectorContactKeyData & key, bool & ok)
{
  mdtClConnectorContactData data;
  QList<QSqlRecord> dataList;
  QString sql;
  mdtClConnectorContactKeyData keyData;

  sql = "SELECT * FROM ConnectorContact_tbl WHERE Id_PK = " + key.id.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  keyData.id = dataList.at(0).value("Id_PK");
  keyData.connectorFk.id = dataList.at(0).value("Connector_Id_FK");
  keyData.connectionTypeFk.code = dataList.at(0).value("ConnectionType_Code_FK");
  data.setKeyData(keyData);
  data.name = dataList.at(0).value("Name");

  return data;
}

bool mdtClConnector::removeContact(const mdtClConnectorContactKeyData & key)
{
  return removeData("ConnectorContact_tbl", "Id_PK", key.id.toString());
}

bool mdtClConnector::addContactList(const QList<mdtClConnectorContactData> & dataList, bool handleTransaction)
{
  mdtSqlRecord record;

  if(!record.addAllFields("ConnectorContact_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  if(handleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  for(const auto & data : dataList){
    fillRecord(record, data);
    if(!addRecord(record, "ConnectorContact_tbl")){
      if(handleTransaction){
        rollbackTransaction();
      }
      return false;
    }
  }
  if(handleTransaction){
    if(!commitTransaction()){
      return false;
    }
  }

  return true;
}

QList<mdtClConnectorContactData> mdtClConnector::getContactDataList(const mdtClConnectorKeyData & key, bool & ok)
{
  QList<mdtClConnectorContactData> dataList;
  QList<QSqlRecord> recordList;
  QString sql;

  sql = "SELECT * FROM ConnectorContact_tbl WHERE Connector_Id_FK = " + key.id.toString();
  recordList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return dataList;
  }
  for(const auto & record : recordList){
    mdtClConnectorContactData data;
    mdtClConnectorContactKeyData keyData;
    keyData.id = record.value("Id_PK");
    keyData.connectorFk.id = record.value("Connector_Id_FK");
    keyData.connectionTypeFk.code = record.value("ConnectionType_Code_FK");
    data.setKeyData(keyData);
    data.name = record.value("Name");
    dataList.append(data);
  }

  return dataList;
}

bool mdtClConnector::removeContactList(const mdtClConnectorKeyData & key)
{
  return removeData("ConnectorContact_tbl", "Connector_Id_FK", key.id.toString());
}

mdtClConnectorKeyData mdtClConnector::addConnector(mdtClConnectorData data, bool handleTransaction)
{
  mdtClConnectorKeyData key;
  mdtSqlRecord record;
  QSqlQuery query(database());

  // Setup record with given data
  if(!record.addAllFields("Connector_tbl", database())){
    pvLastError = record.lastError();
    return key;
  }
  record.setValue("Id_PK", data.keyData().id);
  record.setValue("Gender", data.gender);
  record.setValue("Form", data.form);
  record.setValue("Manufacturer", data.manufacturer);
  record.setValue("ManufacturerConfigCode", data.manufacturerConfigCode);
  record.setValue("ManufacturerArticleCode", data.manufacturerArticleCode);
  // Save connector to database
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
  data.setKeyData(key);
  // Save contacts to database
  if(!addContactList(data.contactDataList(), false)){
    if(handleTransaction){
      rollbackTransaction();
    }
    key.clear();
    return key;
  }
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
  data.setKeyData(key);
  data.gender = dataList.at(0).value("Gender");
  data.form = dataList.at(0).value("Form");
  data.manufacturer = dataList.at(0).value("Manufacturer");
  data.manufacturerConfigCode = dataList.at(0).value("ManufacturerConfigCode");
  data.manufacturerArticleCode = dataList.at(0).value("ManufacturerArticleCode");
  data.setContactDataList(getContactDataList(data.keyData(), ok));
  if(!ok){
    data.clear();
  }

  return data;
}

bool mdtClConnector::removeConnector(const mdtClConnectorKeyData & key, bool handleTransaction)
{
  if(handleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  if(!removeContactList(key)){
    if(handleTransaction){
      rollbackTransaction();
    }
    return false;
  }
  if(!removeData("Connector_tbl", "Id_PK", key.id.toString())){
    if(handleTransaction){
      rollbackTransaction();
    }
    return false;
  }
  if(handleTransaction){
    if(!commitTransaction()){
      return false;
    }
  }

  return true;
}

void mdtClConnector::fillRecord(mdtSqlRecord &record, const mdtClConnectorContactData &data)
{
  Q_ASSERT(!record.isEmpty());

  record.clearValues();
  record.setValue("Id_PK", data.keyData().id);
  record.setValue("Connector_Id_FK", data.keyData().connectorFk.id);
  record.setValue("ConnectionType_Code_FK", data.keyData().connectionTypeFk.code);
  record.setValue("Name", data.name);
}
