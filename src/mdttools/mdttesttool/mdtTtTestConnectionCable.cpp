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
#include "mdtTtTestConnectionCable.h"
#include "mdtClPathGraph.h"
#include "mdtClUnit.h"
#include "mdtSqlRecord.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMutableListIterator>

#include <QDebug>

mdtTtTestConnectionCable::mdtTtTestConnectionCable(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
  pvPathGraph = new mdtClPathGraph(db);
}

mdtTtTestConnectionCable::~mdtTtTestConnectionCable()
{
  delete pvPathGraph;
}

QString mdtTtTestConnectionCable::sqlForTestNodeUnitSelection(const QVariant & testCableId)
{
  QString sql;

  sql = "SELECT\n"\
        " U.SchemaPosition,\n"\
        " U.Alias,\n"\
        " TNU.Unit_Id_FK_PK,\n"\
        /**
        " TNUT.NameEN AS TestNodeUnitTypeEN,\n"\
        " TNUT.NameEN AS TestNodeUnitTypeFR,\n"\
        " TNUT.NameEN AS TestNodeUnitTypeDE,\n"\
        " TNUT.NameEN AS TestNodeUnitTypeIT,\n"\
        */
        " VT.Type,\n"\
        " VT.SubType,\n"\
        " VT.SeriesNumber,\n"\
        " TN.NodeIdentification\n";
  sql += "FROM TestNodeUnit_tbl TNU\n"\
         " JOIN TestNode_tbl TN\n"\
         "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
         " JOIN VehicleType_tbl VT\n"\
         "  ON VT.Id_PK = TN.VehicleType_Id_FK_PK\n"\
         " JOIN Unit_tbl U\n"\
         "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n";
         /**
         " JOIN TestNodeUnitType_tbl TNUT\n"\
         "  ON TNUT.Code_PK = TNU.Type_Code_FK\n";
         */
  sql += "WHERE Type_Code_FK = 'TESTCONNECTOR' ";
  sql += "AND Unit_Id_FK_PK NOT IN (";
  sql += " SELECT TestNodeUnit_Id_FK FROM TestCable_TestNodeUnit_tbl WHERE TestCable_Id_FK = " + testCableId.toString();
  sql += ")";

  return sql;
}

QString mdtTtTestConnectionCable::sqlForDutUnitSelection(const QVariant & testCableId)
{
  QString sql;

  sql = "SELECT * FROM Unit_view ";
  sql += "WHERE Unit_Id_PK NOT IN (";
  sql += "SELECT DutUnit_Id_FK FROM TestCable_DutUnit_tbl WHERE TestCable_Id_FK = " + testCableId.toString();
  sql += ")";

  return sql;
}

bool mdtTtTestConnectionCable::addTestNodeUnit(const QVariant & testNodeUnitId, const QVariant & testCableId)
{
  mdtSqlRecord record;

  if(!record.addAllFields("TestCable_TestNodeUnit_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("TestNodeUnit_Id_FK", testNodeUnitId);
  record.setValue("TestCable_Id_FK", testCableId);

  return addRecord(record, "TestCable_TestNodeUnit_tbl");
}

bool mdtTtTestConnectionCable::removeTestNodeUnits(const mdtSqlTableSelection & s)
{
  return removeData("TestCable_TestNodeUnit_tbl", s, true);
}

bool mdtTtTestConnectionCable::addDutUnit(const QVariant & unitId, const QVariant & testCableId)
{
  mdtSqlRecord record;

  if(!record.addAllFields("TestCable_DutUnit_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("DutUnit_Id_FK", unitId);
  record.setValue("TestCable_Id_FK", testCableId);

  return addRecord(record, "TestCable_DutUnit_tbl");
}

bool mdtTtTestConnectionCable::removeDutUnits(const mdtSqlTableSelection & s)
{
  return removeData("TestCable_DutUnit_tbl", s, true);
}



QString mdtTtTestConnectionCable::sqlForTestNodeSelection() const
{
  QString sql;

  sql = "SELECT * ";
  sql += " FROM TestNode_tbl ";
  sql += " JOIN VehicleType_tbl ";
  sql += "  ON VehicleType_tbl.Id_PK = TestNode_tbl.VehicleType_Id_FK_PK";

  return sql;
}

QString mdtTtTestConnectionCable::sqlForStartConnectorSelection(const QVariant & dutUnitId) const
{
  QString sql;

  sql = "SELECT * FROM UnitConnector_tbl ";
  sql += " WHERE Unit_Id_FK = " + dutUnitId.toString();

  return sql;
}

QString mdtTtTestConnectionCable::sqlForUnitConnectorSelectionFromUnitConnectorIdList(const QList<QVariant> & connectorIdList) const
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

QString mdtTtTestConnectionCable::sqlForTestCableSelection()
{
  QString sql;

  sql = "SELECT * FROM TestCable_tbl";

  return sql;
}

bool mdtTtTestConnectionCable::loadLinkList()
{
  if(!pvPathGraph->loadLinkList()){
    pvLastError = pvPathGraph->lastError();
    return false;
  }
  return true;
}

QList<QVariant> mdtTtTestConnectionCable::getToUnitConnectorRelatedUnitConnectionIdList(const QVariant & unitConnectorId)
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
    pvLastError.commit();
    return connectionIdList;
  }
  while(query.next()){
    connectionIdList.append(query.value(0));
  }

  return connectionIdList;
}

/**
QList<QVariant> mdtTtTestConnectionCable::getToUnitConnectorRelatedUnitConnectionIdList(const QVariant & unitConnectorId, const QList<QVariant> & unitConnectionIdList)
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

QList<QVariant> mdtTtTestConnectionCable::getToUnitConnectionLinkedUnitConnectorIdList(const QVariant & fromUnitConnectionId)
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
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

QList<QVariant> mdtTtTestConnectionCable::getToUnitConnectionIdLinkedUnitConnectionIdListPartOfUnitConnector(const QVariant & fromUnitConnectionId, const QVariant & unitConnectorId)
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
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
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

QList<QVariant> mdtTtTestConnectionCable::getToUnitConnectionIdLinkedUnitConnectionIdListPartOfUnitConnectorList(const QVariant & fromUnitConnectionId, const QList<QVariant> & unitConnectorIdList)
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

QList<QVariant> mdtTtTestConnectionCable::getToUnitConnectionIdListLinkedUnitConnectionIdListPartOfUnitConnectorList(QList<QVariant> & fromUnitConnectionIdList, const QList<QVariant> & unitConnectorIdList)
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

bool mdtTtTestConnectionCable::createTestCable(const QVariant & nodeId, const QList<QVariant> & busAtestConnectionIdList, const QList<QVariant> & busAdutConnectionIdList, const QList<QVariant> & busBtestConnectionIdList, const QList<QVariant> & busBdutConnectionIdList)
{
  QString sql;
  QSqlQuery query(database());
  QSqlError sqlError;
  QVariant testCableId;

  if(!beginTransaction()){
    return false;
  }
  // Add test cable
  if(!addCable("A test cable beta")){
    rollbackTransaction();
    return false;
  }
  // Get freshly inserted test cable ID
  sql = "SELECT Id_PK FROM TestCable_tbl";
  if(!query.exec(sql)){
    rollbackTransaction();
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query to get test cable ID for test links insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
    pvLastError.commit();
    return false;
  }
  testCableId = query.lastInsertId();
  if(testCableId.isNull()){
    rollbackTransaction();
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot get test cable ID for test links inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
    pvLastError.commit();
    return false;
  }
  // Add BUSA links
  if(!addLinks(nodeId, testCableId, busAtestConnectionIdList, busAdutConnectionIdList)){
    rollbackTransaction();
    return false;
  }
  // Add BUSB links
  if(!addLinks(nodeId, testCableId, busBtestConnectionIdList, busBdutConnectionIdList)){
    rollbackTransaction();
    return false;
  }
  // Commit transaction
  if(!commitTransaction()){
    return false;
  }

  return true;
}

mdtTtTestLinkData mdtTtTestConnectionCable::getLinkData(const QVariant & testConnectionId, const QVariant & dutConnectionId, bool *ok)
{
  Q_ASSERT(ok != 0);

  mdtTtTestLinkData linkData;
  QList<QSqlRecord> dataList;
  QString sql;

  // Get link data part
  sql = "SELECT * FROM TestLink_tbl";
  sql += " WHERE TestConnection_Id_FK = " + testConnectionId.toString();
  sql += " AND DutConnection_Id_FK = " + dutConnectionId.toString();
  dataList = getData(sql, ok);
  if(!*ok){
    return linkData;
  }
  if(dataList.isEmpty()){
    QString msg;
    msg = tr("No link exists from TestConnection_Id_FK ") + testConnectionId.toString();
    msg += tr(" to DutConnection_Id_FK ") + dutConnectionId.toString();
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
    pvLastError.commit();
    *ok = false;
    return linkData;
  }
  Q_ASSERT(dataList.size() == 1);
  linkData = dataList.at(0);
  // Done
  *ok = true;

  return linkData;
}

bool mdtTtTestConnectionCable::addLink(const mdtTtTestLinkData & data)
{
  return addRecord(data, "TestLink_tbl");
}

bool mdtTtTestConnectionCable::addLinks(const QVariant & nodeId, const QVariant & testCableId, const QList<QVariant> & testConnectionIdList, const QList<QVariant> & dutConnectionIdList)
{
  Q_ASSERT(dutConnectionIdList.size() <= testConnectionIdList.size());

  int i;

  // Add test links
  for(i = 0; i < dutConnectionIdList.size(); ++i){
    qDebug() << "Adding link from test connection " << testConnectionIdList.at(i) << " -> dut connection " << dutConnectionIdList.at(i);
    if(!addLink(testConnectionIdList.at(i), dutConnectionIdList.at(i), testCableId, "...", 0.1)){
      return false;
    }
    qDebug() << "->Link added";
  }

  return true;

}

bool mdtTtTestConnectionCable::editLink(const QVariant & testConnectionId, const QVariant & dutConnectionId, const mdtTtTestLinkData & linkData)
{
  if(!beginTransaction()){
    return false;
  }
  if(!removeLink(testConnectionId, dutConnectionId)){
    rollbackTransaction();
    return false;
  }
  if(!addLink(linkData)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTestConnectionCable::removeLink(const QVariant & testConnectionId, const QVariant & dutConnectionId)
{
  return removeData("TestLink_tbl", "TestConnection_Id_FK", testConnectionId, "DutConnection_Id_FK", dutConnectionId);
}

bool mdtTtTestConnectionCable::addCable(const QVariant & identification)
{
  QString sql;
  QSqlQuery query(database());
  QSqlError sqlError;

  // Prepare query for insertion
  sql = "INSERT INTO TestCable_tbl (Identification) "\
        "VALUES (:Identification)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for inertion into TestCable_tbl", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Identification", identification);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for inertion into TestCable_tbl", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTestConnectionCable::addLink(const QVariant & testConnectionId, const QVariant & dutConnectionId, const QVariant & testCableId, const QVariant & identification, const QVariant & value)
{
  QString sql;
  QSqlQuery query(database());
  QSqlError sqlError;

  // Prepare query for insertion
  sql = "INSERT INTO TestLink_tbl (TestConnection_Id_FK, DutConnection_Id_FK, TestCable_Id_FK, Identification, Value) "\
        "VALUES (:TestConnection_Id_FK, :DutConnection_Id_FK, :TestCable_Id_FK, :Identification, :Value)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for inertion into TestLink_tbl", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":TestConnection_Id_FK", testConnectionId);
  query.bindValue(":DutConnection_Id_FK", dutConnectionId);
  query.bindValue(":TestCable_Id_FK", testCableId);
  query.bindValue(":Identification", identification);
  query.bindValue(":Value", value);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for inertion into TestLink_tbl", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
    pvLastError.commit();
    return false;
  }

  return true;
}

/**
mdtClLinkData mdtTtTestConnectionCable::getTestLinkData(const QVariant & testLinkId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  mdtClLinkData data;

  // Setup and run query to get data in test link table
  sql = "SELECT "\
        "TestConnection_Id_FK , DutConnection_Id_FK, TestCable_Id_FK, Identification, Value ";
  sql += " FROM TestLink_tbl ";
  sql += " WHERE Id_PK = " + testLinkId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get test link data", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
    pvLastError.commit();
    return data;
  }
  if(!query.next()){
    return data;
  }
  data.setUnitConnectionStartId(query.value(0));
  data.setUnitConnectionEndId(query.value(1));
  data.setIdentification(query.value(3));
  data.setValue(query.value(4));
  data.setLinkTypeCode("TESTLINK");
  data.setLinkDirectionCode("BID");

  return data;
}
*/


/**
bool mdtTtTestConnectionCable::addLinks(const QVariant & nodeId, const QList<QVariant> & testConnectionIdList, const QVariant & dutVehicleId, const QList<QVariant> & dutConnectionIdList)
{
  Q_ASSERT(dutConnectionIdList.size() <= testConnectionIdList.size());

  mdtClUnit unit(this, database());
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
*/

/**
QList<mdtClLinkData> mdtTtTestConnectionCable::getTestLinkDataByTestCableId(const QVariant & testCableId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QList<mdtClLinkData> dataList;

  // Setup and run query to get data in test link table
  sql = "SELECT "\
        "TestConnection_Id_FK , DutConnection_Id_FK, TestCable_Id_FK, Identification, Value ";
  sql += " FROM TestLink_tbl ";
  sql += " WHERE TestCable_Id_FK = " + testCableId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get test link data", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestConnectionCable");
    pvLastError.commit();
    return dataList;
  }
  while(query.next()){
    mdtClLinkData data;
    data.setUnitConnectionStartId(query.value(0));
    data.setUnitConnectionEndId(query.value(1));
    data.setIdentification(query.value(3));
    data.setValue(query.value(4));
    data.setLinkTypeCode("TESTLINK");
    data.setLinkDirectionCode("BID");
    dataList.append(data);
  }

  return dataList;
}
*/

bool mdtTtTestConnectionCable::connectTestCable(const QVariant & testCableId, const QVariant & testNodeId, const QVariant & dutVehicleTypeId)
{
  qDebug() << "mdtTtTestConnectionCable::connectTestCable() - CURRENTLY NOT IMPLEMENTED !!";
  return false;
  /**
  QList<mdtClLinkData> testLinkDataList;
  mdtClLinkData data;
  mdtClUnit unit(this, database());
  int i;

  // Get link data for given cable
  testLinkDataList = getTestLinkDataByTestCableId(testCableId);
  if(testLinkDataList.isEmpty()){
    return false;
  }
  // Add each link
  for(i = 0; i < testLinkDataList.size(); ++i){
    data = testLinkDataList.at(i);
    data.addVehicleTypeStartId(testNodeId);
    data.addVehicleTypeEndId(dutVehicleTypeId);
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
  */

  return true;
}

bool mdtTtTestConnectionCable::disconnectTestCable(const QVariant & testCableId)
{
  qDebug() << "mdtTtTestConnectionCable::disconnectTestCable() - CURRENTLY NOT IMPLEMENTED !!";
  return false;
  /**
  QList<mdtClLinkData> testLinkDataList;
  mdtClLinkData data;
  mdtClUnit unit(this, database());
  int i;

  // Get link data for given cable
  testLinkDataList = getTestLinkDataByTestCableId(testCableId);
  if(testLinkDataList.isEmpty()){
    return false;
  }
  // Remove each link
  for(i = 0; i < testLinkDataList.size(); ++i){
    data = testLinkDataList.at(i);
    if(!unit.removeLink(data.unitConnectionStartId(), data.unitConnectionEndId())){
      pvLastError = unit.lastError();
      return false;
    }
    qDebug() << "->Link removed";
  }
  */

  return true;
}
