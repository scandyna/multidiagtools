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
#include "mdtClLinkData.h"
#include "mdtClUnit.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMutableListIterator>

#include <QDebug>

mdtCcTestConnectionCable::mdtCcTestConnectionCable(QSqlDatabase db)
 : mdtClBase(db)
{
  pvPathGraph = new mdtClPathGraph(db);
}

mdtCcTestConnectionCable::~mdtCcTestConnectionCable()
{
  delete pvPathGraph;
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

bool mdtCcTestConnectionCable::loadLinkList()
{
  if(!pvPathGraph->loadLinkList()){
    pvLastError = pvPathGraph->lastError();
    return false;
  }
  return true;
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

/**
QList<QVariant> mdtCcTestConnectionCable::getToUnitConnectorRelatedUnitConnectionIdList(const QVariant & unitConnectorId, const QList<QVariant> & unitConnectionIdList)
{
  QList<QVariant> allConnectionIdList;
  QList<QVariant> connectionIdList;
  int i;

  allConnectionIdList = getToUnitConnectorRelatedUnitConnectionIdList(unitConnectorId);
  for(i = 0; i < unitConnectionIdList.size(); ++i){
    if(allConnectionIdList.contains(unitConnectionIdList.at(i))){
      connectionIdList.append(unitConnectionIdList.at(i));
    }
  }

  return connectionIdList;
}
*/

QList<QVariant> mdtCcTestConnectionCable::getToUnitConnectionLinkedUnitConnectorIdList(const QVariant & fromUnitConnectionId)
{
  Q_ASSERT(!fromUnitConnectionId.isNull());

  QList<QVariant> unitConnectionIdList;
  QList<QVariant> unitConnectorIdList;
  QString sql;
  QSqlQuery query(database());
  QSqlError sqlError;
  QVariant id;
  int i;

  unitConnectionIdList = pvPathGraph->getLinkedConnectionIdList(fromUnitConnectionId);
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

QList<QVariant> mdtCcTestConnectionCable::getToUnitConnectionIdLinkedUnitConnectionIdListPartOfUnitConnector(const QVariant & fromUnitConnectionId, const QVariant & unitConnectorId)
{
  Q_ASSERT(!fromUnitConnectionId.isNull());
  Q_ASSERT(!unitConnectorId.isNull());

  QList<QVariant> allUnitConnectionIdList;
  QList<QVariant> unitConnectionIdList;
  QString sql;
  QSqlQuery query(database());
  QSqlError sqlError;
  QVariant id;
  int i;

  allUnitConnectionIdList = pvPathGraph->getLinkedConnectionIdList(fromUnitConnectionId);
  if(allUnitConnectionIdList.isEmpty()){
    return unitConnectionIdList;
  }
  // Get list of unit connections that are part of unit connector
  Q_ASSERT(allUnitConnectionIdList.size() > 0);
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE UnitConnector_Id_FK = " + unitConnectorId.toString();
  sql += " AND ( Id_PK = " + allUnitConnectionIdList.at(0).toString();
  for(i = 1; i < allUnitConnectionIdList.size(); ++i){
    sql += " OR Id_PK = " + allUnitConnectionIdList.at(i).toString();
  }
  sql += " )";
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get unit connection ID list", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtCcTestConnectionCable");
    pvLastError.commit();
    return unitConnectionIdList;
  }
  while(query.next()){
    id = query.value(0);
    if((!id.isNull())&&(!unitConnectionIdList.contains(id))){
      unitConnectionIdList.append(id);
    }
  }

  return unitConnectionIdList;
}

QList<QVariant> mdtCcTestConnectionCable::getToUnitConnectionIdLinkedUnitConnectionIdListPartOfUnitConnectorList(const QVariant & fromUnitConnectionId, const QList<QVariant> & unitConnectorIdList)
{
  Q_ASSERT(!fromUnitConnectionId.isNull());

  QList<QVariant> unitConnectionIdList;
  QList<QVariant> tempIdList;
  QVariant id;
  int i, k;

  for(i = 0; i < unitConnectorIdList.size(); ++i){
    tempIdList = getToUnitConnectionIdLinkedUnitConnectionIdListPartOfUnitConnector(fromUnitConnectionId, unitConnectorIdList.at(i));
    for(k = 0; k < tempIdList.size(); ++k){
      id = tempIdList.at(k);
      if((!id.isNull())&&(!unitConnectionIdList.contains(id))){
        unitConnectionIdList.append(id);
      }
    }
  }

  return unitConnectionIdList;
}

QList<QVariant> mdtCcTestConnectionCable::getToUnitConnectionIdListLinkedUnitConnectionIdListPartOfUnitConnectorList(QList<QVariant> & fromUnitConnectionIdList, const QList<QVariant> & unitConnectorIdList)
{
  QList<QVariant> unitConnectionIdList;
  QList<QVariant> tempIdList;
  QMutableListIterator<QVariant> it(fromUnitConnectionIdList);
  QVariant id;
  int k;

  while(it.hasNext()){
    id = it.next();
    Q_ASSERT(!id.isNull());
    if(unitConnectionIdList.contains(id)){
      it.remove();
    }else{
      tempIdList = getToUnitConnectionIdLinkedUnitConnectionIdListPartOfUnitConnectorList(id, unitConnectorIdList);
      for(k = 0; k < tempIdList.size(); ++k){
        id = tempIdList.at(k);
        if((!id.isNull())&&(!unitConnectionIdList.contains(id))){
          unitConnectionIdList.append(id);
        }
      }
    }
  }

  return unitConnectionIdList;
}

bool mdtCcTestConnectionCable::addLinks(const QVariant & nodeId, const QList<QVariant> & testConnectionIdList, const QVariant & dutVehicleId, const QList<QVariant> & dutConnectionIdList)
{
  Q_ASSERT(dutConnectionIdList.size() <= testConnectionIdList.size());

  mdtClUnit unit(database());
  int i;

  for(i = 0; i < dutConnectionIdList.size(); ++i){
    mdtClLinkData data;
    qDebug() << "Adding link from test connection " << testConnectionIdList.at(i) << " -> dut connection " << dutConnectionIdList.at(i);
    data.setUnitConnectionStartId(testConnectionIdList.at(i));
    data.setUnitConnectionEndId(dutConnectionIdList.at(i));
    data.setIdentification("...");
    data.setLinkDirectionCode("BID");
    data.setLinkTypeCode("TESTLINK");
    data.setValue(0.1);
    data.addVehicleTypeStartId(nodeId);
    data.addVehicleTypeEndId(dutVehicleId);
    if(!data.buildVehicleTypeStartEndIdList()){
      qDebug() << "Cannot build vehicle list";
      return false;
    }
    qDebug() << "VHC lst: " << data.vehicleTypeStartEndIdList();
    if(!unit.addLink(data)){
      pvLastError = unit.lastError();
      return false;
    }
    qDebug() << "->Link added";
  }

  return true;
}
