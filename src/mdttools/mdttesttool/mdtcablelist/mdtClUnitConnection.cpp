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
#include "mdtClUnitConnection.h"
#include "mdtSqlRecord.h"
#include "mdtSqlTransaction.h"
#include <QSqlQuery>
#include <QList>

//#include <QDebug>

mdtClUnitConnection::mdtClUnitConnection(QObject *parent, QSqlDatabase db)
 : mdtClArticleConnection(parent, db)
{
}

mdtClUnitConnection::mdtClUnitConnection(QSqlDatabase db)
 : mdtClUnitConnection(nullptr, db)
{
}

mdtClUnitConnectionKeyData mdtClUnitConnection::addUnitConnection(const mdtClUnitConnectionData & data, bool handleTransaction)
{
  mdtClUnitConnectionKeyData key;
  mdtSqlRecord record;
  QSqlQuery query(database());
  mdtSqlTransaction transaction(database());

  // Setup record with given data
  if(!record.addAllFields("UnitConnection_tbl", database())){
    pvLastError = record.lastError();
    return key;
  }
  fillRecord(record, data);
  // Save to database
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return key;
    }
  }
  if(!addRecord(record, "UnitConnection_tbl", query)){
    return key;
  }
  key = data.keyData();
  key.id = query.lastInsertId();
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      key.clear();
      return key;
    }
  }

  return key;
}

mdtClUnitConnectionData mdtClUnitConnection::getUnitConnectionData(const mdtClUnitConnectionKeyData & key, bool & ok)
{
  mdtClUnitConnectionData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT UCNX.*,\n"\
        " UCNR.Connector_Id_FK AS UCNR_Connector_Id_FK,\n"\
        " UCNR.ArticleConnector_Id_FK AS UCNR_ArticleConnector_Id_FK,\n"\
        " ACNX.ArticleConnector_Id_FK AS ACNX_ArticleConnector_Id_FK,\n"\
        " ACNX.Article_Id_FK,\n"\
        " ACNX.ConnectionType_Code_FK AS ACNX_ConnectionType_Code_FK\n"\
        "FROM UnitConnection_tbl UCNX\n"\
        " LEFT JOIN UnitConnector_tbl UCNR ON UCNR.Id_PK = UCNX.UnitConnector_Id_FK\n"\
        " LEFT JOIN ArticleConnection_tbl ACNX ON ACNX.Id_PK = UCNX.ArticleConnection_Id_FK\n";
  sql += " WHERE UCNX.Id_PK = " + key.id.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  fillData(data, dataList.at(0));

  return data;
}

bool mdtClUnitConnection::removeUnitConnection(const mdtClUnitConnectionKeyData & key)
{
  return removeData("UnitConnection_tbl", "Id_PK", key.id);
}


void mdtClUnitConnection::fillRecord(mdtSqlRecord & record, const mdtClUnitConnectionData & data)
{
  Q_ASSERT(!record.isEmpty());

  auto key = data.keyData();

  record.clearValues();
  record.setValue("Id_PK", key.id);
  record.setValue("Unit_Id_FK", key.unitId());
  record.setValue("UnitConnector_Id_FK", key.unitConnectorFk().id);
  record.setValue("ArticleConnection_Id_FK", key.articleConnectionFk().id);
  record.setValue("ConnectionType_Code_FK", key.connectionTypeFk().code);
  record.setValue("UnitContactName", data.name);
  record.setValue("Resistance", data.resistance);
  record.setValue("SchemaPage", data.schemaPage);
  record.setValue("SignalName", data.signalName);
  record.setValue("SwAddress", data.swAddress);
  record.setValue("FunctionEN", data.functionEN);
  record.setValue("FunctionFR", data.functionFR);
  record.setValue("FunctionDE", data.functionDE);
  record.setValue("FunctionIT", data.functionIT);
}

void mdtClUnitConnection::fillData(mdtClUnitConnectionData & data, const QSqlRecord & record)
{
  Q_ASSERT(record.contains("Id_PK"));
  Q_ASSERT(record.contains("Unit_Id_FK"));
  Q_ASSERT(record.contains("UnitConnector_Id_FK"));
  Q_ASSERT(record.contains("ArticleConnection_Id_FK"));
  Q_ASSERT(record.contains("ConnectionType_Code_FK"));
  Q_ASSERT(record.contains("UnitContactName"));
  Q_ASSERT(record.contains("Resistance"));
  Q_ASSERT(record.contains("SchemaPage"));
  Q_ASSERT(record.contains("SignalName"));
  Q_ASSERT(record.contains("SwAddress"));
  Q_ASSERT(record.contains("functionEN"));
  Q_ASSERT(record.contains("functionFR"));
  Q_ASSERT(record.contains("functionDE"));
  Q_ASSERT(record.contains("functionIT"));
  Q_ASSERT(record.contains("UCNR_Connector_Id_FK"));
  Q_ASSERT(record.contains("UCNR_ArticleConnector_Id_FK"));
  Q_ASSERT(record.contains("ACNX_ArticleConnector_Id_FK"));
  Q_ASSERT(record.contains("Article_Id_FK"));
  Q_ASSERT(record.contains("ACNX_ConnectionType_Code_FK"));

  mdtClConnectorKeyData connectorFk;
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClUnitConnectorKeyData unitConnectorFk;
  mdtClArticleConnectionKeyData articleConnectionFk;
  mdtClUnitConnectionKeyData key;

  // Setup connector FK
  connectorFk.id = record.value("UCNR_Connector_Id_FK");
  // Setup article connector FK
  articleConnectorFk.id = record.value("ACNX_ArticleConnector_Id_FK");
  articleConnectorFk.setArticleId(record.value("Article_Id_FK"));
  if( (!articleConnectionFk.isNull())&&(!connectorFk.isNull()) ){
    articleConnectorFk.setConnectorFk(connectorFk);
  }
  // Setup unit connector FK
  unitConnectorFk.id = record.value("UnitConnector_Id_FK");
  unitConnectorFk.setUnitId(record.value("Unit_Id_FK"));
  if(!articleConnectorFk.isNull()){
    unitConnectorFk.setArticleConnectorFk(articleConnectorFk);
  }else{
    if(!connectorFk.isNull()){
      unitConnectorFk.setConnectorFk(connectorFk);
    }
  }
  // Setup article connection FK
  articleConnectionFk.id = record.value("ArticleConnection_Id_FK");
  articleConnectionFk.setArticleId(record.value("Article_Id_FK"));
  if(!articleConnectorFk.isNull()){
    articleConnectionFk.setArticleConnectorFk(articleConnectorFk);
  }
  articleConnectionFk.setConnectionTypeCode(record.value("ACNX_ConnectionType_Code_FK"));
  // Setup unit connection key
  key.id = record.value("Id_PK");
  key.setUnitId(record.value("Unit_Id_FK"));
  if(!unitConnectorFk.isNull()){
    key.setUnitConnectorFk(unitConnectorFk);
  }
  if(articleConnectionFk.isNull()){
    key.setConnectionTypeCode(record.value("ConnectionType_Code_FK"));
  }else{
    key.setArticleConnectionFk(articleConnectionFk);
  }
  // Fill data
  data.setKeyData(key);
  data.name = record.value("UnitContactName");
  data.resistance = record.value("Resistance");
  data.schemaPage = record.value("SchemaPage");
  data.signalName = record.value("SignalName");
  data.swAddress = record.value("SwAddress");
  data.functionEN = record.value("functionEN");
  data.functionFR = record.value("functionFR");
  data.functionDE = record.value("functionDE");
  data.functionIT = record.value("functionIT");
}
