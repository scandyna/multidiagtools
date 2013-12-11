/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtCcTestConnectionCable.h"
#include "mdtClPathGraph.h"
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

mdtCcTestConnectionCable::mdtCcTestConnectionCable(QSqlDatabase db)
 : mdtClBase(db)
{
}

QString mdtCcTestConnectionCable::sqlForTestNodeSelection()
{
  QString sql;

  sql = "SELECT * ";
  sql += " FROM TestNode_tbl ";
  sql += " JOIN VehicleType_tbl ";
  sql += "  ON VehicleType_tbl.Id_PK = TestNode_tbl.VehicleType_Id_FK_PK";

  return sql;
}

QString mdtCcTestConnectionCable::sqlForStartConnectorSelection(const QVariant & dutUnitId) const
{
  QString sql;

  sql = "SELECT * FROM UnitConnector_tbl ";
  sql += " WHERE Unit_Id_FK = " + dutUnitId.toString();

  return sql;
}

QString mdtCcTestConnectionCable::sqlForUnitConnectorSelectionFromUnitConnectorIdList(const QList<QVariant> & connectorIdList) const
{
  QString sql;
  int i;

  if(connectorIdList.isEmpty()){
    return sql;
  }
  sql = "SELECT * FROM UnitConnector_tbl WHERE Id_PK = " + connectorIdList.at(0).toString();
  for(i = 1; i < connectorIdList.size(); ++i){
    sql += " OR Id_PK = " + connectorIdList.at(i).toString();
  }

  return sql;
}

QList<QVariant> mdtCcTestConnectionCable::getToUnitConnectorRelatedUnitConnectionIdList(const QVariant & unitConnectorId)
{
  QList<QVariant> connectionIdList;
  QString sql;
  QSqlQuery query(database());
  QSqlError sqlError;

  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE UnitConnector_Id_FK = " + unitConnectorId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get unit connection ID list", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtCcTestConnectionCable");
    pvLastError.commit();
    return connectionIdList;
  }
  while(query.next()){
    connectionIdList.append(query.value(0));
  }

  return connectionIdList;
}

QList<QVariant> mdtCcTestConnectionCable::getToUnitConnectionLinkedUnitConnectorIdList(const QVariant & fromUnitConnectionId)
{
  Q_ASSERT(!fromUnitConnectionId.isNull());

  QList<QVariant> unitConnectionIdList;
  QList<QVariant> unitConnectorIdList;
  mdtClPathGraph graph(database());
  QString sql;
  QSqlQuery query(database());
  QSqlError sqlError;
  QVariant id;
  int i;

  if(!graph.loadLinkList()){
    pvLastError = graph.lastError();
    return unitConnectorIdList;
  }
  unitConnectionIdList = graph.getLinkedConnectionIdList(fromUnitConnectionId);
  if(unitConnectionIdList.isEmpty()){
    return unitConnectorIdList;
  }
  // Get list of connector ID for each connection
  sql = "SELECT UnitConnector_Id_FK FROM UnitConnection_tbl WHERE Id_PK = " + unitConnectionIdList.at(0).toString();
  for(i = 1; i < unitConnectionIdList.size(); ++i){
    sql += " OR Id_PK = " + unitConnectionIdList.at(i).toString();
  }
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get unit connection ID list", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtCcTestConnectionCable");
    pvLastError.commit();
    return unitConnectorIdList;
  }
  while(query.next()){
    id = query.value(0);
    if((!id.isNull())&&(!unitConnectorIdList.contains(id))){
      unitConnectorIdList.append(id);
    }
  }

  return unitConnectorIdList;
}
