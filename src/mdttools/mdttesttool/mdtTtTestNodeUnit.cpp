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
#include "mdtTtTestNodeUnit.h"
#include "mdtClUnit.h"
#include <QSqlRecord>

#include <QDebug>

mdtTtTestNodeUnit::mdtTtTestNodeUnit(QObject *parent, const QSqlDatabase & db)
 : mdtTtBase(parent, db)
{
}

mdtSqlRecord mdtTtTestNodeUnit::getUnitData(const QVariant & unitId, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;
  QList<QSqlRecord> dataList;
  mdtSqlRecord data;

  if(unitId.isNull()){
    pvLastError.setError("Trying to get unit data for a NULL unit ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeUnit");
    pvLastError.commit();
    *ok = false;
    return data;
  }
  sql = "SELECT * FROM Unit_tbl WHERE Id_PK = " + unitId.toString();
  dataList = mdtTtBase::getData(sql, ok);
  if(!*ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  *ok = true;

  return data;
}

mdtTtTestNodeUnitData mdtTtTestNodeUnit::getData(const QVariant & nodeUnitId, bool *ok, bool includeTestConnectionData) 
{
  Q_ASSERT(ok != 0);

  mdtClUnit unit(0, database());
  QString sql;
  mdtTtTestNodeUnitData data;
  QList<QSqlRecord> dataList;
  mdtSqlRecord unitData;

  if(nodeUnitId.isNull()){
    pvLastError.setError("Trying to get node unit data for a NULL node unit ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeUnit");
    pvLastError.commit();
    *ok = false;
    return data;
  }
  // Get TestNodeUnit_tbl part
  sql = "SELECT * FROM TestNodeUnit_tbl WHERE Unit_Id_FK_PK = " + nodeUnitId.toString();
  dataList = mdtTtBase::getData(sql, ok);
  if(!*ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  // Get Unit_tbl part
  unitData = getUnitData(nodeUnitId, ok);
  if(!*ok){
    data.clearValues();
    return data;
  }
  data.setUnitData(dataList.at(0));
  // Get test connection data, if required
  if(includeTestConnectionData){
    mdtClUnitConnectionData connectionData;
    connectionData = unit.getConnectionData(data.value("TestConnection_Id_FK"), false, ok);
    if(!*ok){
      return data;
    }
    data.setTestConnectionData(connectionData);
  }
  // Done
  *ok = true;

  return data;
}

bool mdtTtTestNodeUnit::add(const mdtTtTestNodeUnitData & data) 
{
}

bool mdtTtTestNodeUnit::edit(const QVariant & nodeUnitId, const mdtTtTestNodeUnitData & data) 
{
}

bool mdtTtTestNodeUnit::remove(const QVariant & nodeUnitId) 
{
}

