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
#include "mdtTtTestModelItem.h"
#include "mdtSqlRecord.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QString>
#include <QLocale>

#include <QDebug>

mdtTtTestModelItem::mdtTtTestModelItem(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

QString mdtTtTestModelItem::sqlForTestLinkSelection(const QVariant & testModelItemId, const QVariant & testModelId) const
{
  QString sql;

  sql = "SELECT\n"\
        " LNK.Id_PK,\n"\
        " LNK.TestCable_Id_FK,\n"\
        " LNK.TestConnection_Id_FK,\n"\
        " LNK.DutConnection_Id_FK,\n"\
        " LNK.Identification AS TestLinkIdentification,\n"\
        " LNK.Value AS TestLinkValue,\n"\
        " UD.SchemaPosition AS DutUnitSchemaPosition,\n"\
        " UD.Alias AS DutUnitAlias,\n"\
        " UCD.Name AS DutConnectorName,\n"\
        " UCNXD.UnitContactName AS DutContactName,\n"\
        " UTNU.SchemaPosition AS TestNodeUnitSchemaPosition,\n"\
        " UCT.Name AS TestConnectorName,\n"\
        " UCNXT.UnitContactName AS TestContactName,\n"\
        " TNB.NameEN AS Bus,\n"\
        /**" TNU.IoPosition,\n"\ */
        " VTN.Type AS TestNodeType,\n"\
        " VTN.SubType AS TestNodeSubType,\n"\
        " VTN.SeriesNumber AS TestNodeSeriesNumber,\n"\
        " TN.NodeIdentification,\n"\
        " UVT.Type,\n"\
        " UVT.SubType,\n"\
        " UVT.SeriesNumber\n"\
        "FROM TestLink_tbl LNK\n"\
        " JOIN TestNodeUnitConnection_tbl TNUCNX\n"\
        "  ON TNUCNX.UnitConnection_Id_FK_PK = LNK.TestConnection_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXT\n"\
        "  ON UCNXT.Id_PK = LNK.TestConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCT\n"\
        "  ON UCT.Id_PK = UCNXT.UnitConnector_Id_FK\n"\
        " JOIN TestNodeUnit_tbl TNU\n"\
        "  ON TNU.Unit_Id_FK_PK = TNUCNX.TestNodeUnit_Id_FK\n"\
        " JOIN Unit_tbl UTNU\n"\
        "  ON UTNU.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
        " JOIN VehicleType_tbl VTN\n"\
        "  ON VTN.Id_PK = TN.VehicleType_Id_FK_PK\n"\
        " LEFT JOIN TestNodeBus_tbl TNB\n"\
        "  ON TNB.Id_PK = TNUCNX.TestNodeBus_Id_FK\n"\
        " JOIN TestModel_TestNode_tbl TMTN\n"\
        "  ON TMTN.TestNode_Id_FK = TN.VehicleType_Id_FK_PK\n"\
        " JOIN UnitConnection_tbl UCNXD\n"\
        "  ON UCNXD.Id_PK = LNK.DutConnection_Id_FK\n"\
        " LEFT JOIN UnitConnector_tbl UCD\n"\
        "  ON UCD.Id_PK = UCNXD.UnitConnector_Id_FK\n"\
        " JOIN Unit_tbl UD\n"\
        "  ON UD.Id_PK = UCNXD.Unit_Id_FK\n"\
        " JOIN VehicleType_Unit_tbl UVTU\n"\
        "  ON UVTU.Unit_Id_FK = UD.Id_PK\n"\
        " JOIN VehicleType_tbl UVT\n"\
        "  ON UVT.Id_PK = UVTU.VehicleType_Id_FK\n";
  ///sql = "SELECT * FROM TestLink_view WHERE Id_PK NOT IN (";
  sql += "WHERE TMTN.TestModel_Id_FK = " + testModelId.toString();
  sql += " AND LNK.Id_PK NOT IN (";
  sql += "  SELECT TestLink_Id_FK FROM TestModelItem_TestLink_tbl WHERE TestModelItem_Id_FK = " + testModelItemId.toString();
  sql += " )";
  /// \todo Ajouter filtre sur une list de test nodes. Voir pour utiliser WHERE TestNode_Id_FK IN (Id1,Id2, ...)

  return sql;
}

QString mdtTtTestModelItem::sqlForTestNodeUnitSelection(const QVariant & testModelItemId, const QVariant & testModelId) const
{
  QString sql;
  QLocale locale;
  QString tnTypeField;

  // Select test node unit type field regarding language
  switch(locale.language()){
    case QLocale::French:
      tnTypeField = "NameFR";
      break;
    case QLocale::German:
      tnTypeField = "NameDE";
      break;
    case QLocale::Italian:
      tnTypeField = "NameIT";
      break;
    default:
      tnTypeField = "NameEN";
  }
  // Generate base of SQL statement
  sql = "SELECT\n"\
        " TNU.Unit_Id_FK_PK,\n"\
        " TNU.TestNode_Id_FK,\n"\
        " TNU.Type_Code_FK,\n"\
        " U.SchemaPosition,\n"\
        " U.Alias,\n";
  sql += " TNUT." + tnTypeField + ",\n"\
        " VT.Type,\n"\
        " VT.SubType,\n"\
        " VT.SeriesNumber\n"\
        "FROM TestNodeUnit_tbl TNU\n"\
        " JOIN Unit_tbl U\n"\
        "  ON U.Id_PK = TNU.Unit_Id_FK_PK\n"\
        " JOIN TestNodeUnitType_tbl TNUT\n"\
        "  ON TNUT.Code_PK = TNU.Type_Code_FK\n"\
        " JOIN TestNode_tbl TN\n"\
        "  ON TN.VehicleType_Id_FK_PK = TNU.TestNode_Id_FK\n"\
        " JOIN VehicleType_tbl VT\n"\
        "  ON VT.Id_PK = TN.VehicleType_Id_FK_PK\n"\
        " JOIN TestModel_TestNode_tbl TMTN\n"\
        "  ON TMTN.TestNode_Id_FK = TN.VehicleType_Id_FK_PK";
  // Limit to test nodes that are affected to given test model
  sql += " WHERE TMTN.TestModel_Id_FK = " + testModelId.toString();
  // Don't show allready used units
  sql += " AND Unit_Id_FK_PK NOT IN(";
  sql += "  SELECT TestNodeUnit_Id_FK FROM TestNodeUnitSetup_tbl WHERE TestModelItem_Id_FK = " + testModelItemId.toString();
  sql += ")";

  return sql;
}

QList<QVariant> mdtTtTestModelItem::getUsedNodeUnitIdList(const QVariant & testItemId, const QVariant & type)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QVariant nodeUnitId;
  QList<QVariant> nodeUnitIdList;

  // Setup and run query to get data in unit link view
  sql = "SELECT Unit_Id_FK_PK FROM TestModelItemNodeUnit_view WHERE Id_PK = " + testItemId.toString();
  if(!type.isNull()){
    sql += " AND Type_Code_FK = '" + type.toString() + "' ";
  }
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of used node units", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTest");
    pvLastError.commit();
    return nodeUnitIdList;
  }
  while(query.next()){
    nodeUnitId = query.value(0);
    if((!nodeUnitId.isNull())&&(!nodeUnitIdList.contains(nodeUnitId))){
      nodeUnitIdList.append(nodeUnitId);
    }
  }

  return nodeUnitIdList;
}

QList<QVariant> mdtTtTestModelItem::getTestNodeUnitSetupIdList(const QVariant & testItemId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  QList<QVariant> tnusIdList;
  QVariant id;

  // Setup and run query to get data in unit link view
  sql = "SELECT Id_PK FROM TestModelItemNodeUnitSetup_view WHERE TestModelItem_Id_FK = " + testItemId.toString();
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot get list of node unit setup", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelItem");
    pvLastError.commit();
    return tnusIdList;
  }
  while(query.next()){
    id = query.value(0);
    if((!id.isNull())&&(!tnusIdList.contains(id))){
      tnusIdList.append(id);
    }
  }

  return tnusIdList;
}

bool mdtTtTestModelItem::addTestLink(const QVariant & testModelItemId, const QVariant & testLinkId)
{
  mdtSqlRecord record;

  if(!record.addAllFields("TestModelItem_TestLink_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("TestModelItem_Id_FK", testModelItemId);
  record.setValue("TestLink_Id_FK", testLinkId);

  return addRecord(record, "TestModelItem_TestLink_tbl");
}

bool mdtTtTestModelItem::removeTestLinks(const mdtSqlTableSelection & s)
{
  return removeData("TestModelItem_TestLink_tbl", s, true);
}

bool mdtTtTestModelItem::addTestNodeUnitSetup(const mdtTtTestNodeUnitSetupData & data)
{
  return addRecord(data, "TestNodeUnitSetup_tbl");
}

mdtTtTestNodeUnitSetupData mdtTtTestModelItem::getNodeUnitSetupData(const QVariant & testModelItemId, const QVariant & testNodeUnitId, bool *ok)
{
  Q_ASSERT(ok != 0);

  mdtTtTestNodeUnitSetupData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT * FROM TestNodeUnitSetup_tbl ";
  sql += " WHERE TestModelItem_Id_FK = " + testModelItemId.toString();
  sql += " AND TestNodeUnit_Id_FK = " + testNodeUnitId.toString();
  dataList = getData(sql, ok);
  if(!*ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);

  return data;
}

bool mdtTtTestModelItem::updateNodeUnitData(const QVariant & testModelItemId, const QVariant & testNodeUnitId, const mdtTtTestNodeUnitSetupData & data)
{
  mdtSqlRecord matchData;

  // Setup match data
  if(!matchData.addField("TestModelItem_Id_FK", "TestNodeUnitSetup_tbl", database())){
    pvLastError = matchData.lastError();
    return false;
  }
  if(!matchData.addField("TestNodeUnit_Id_FK", "TestNodeUnitSetup_tbl", database())){
    pvLastError = matchData.lastError();
    return false;
  }
  matchData.setValue("TestModelItem_Id_FK", testModelItemId);
  matchData.setValue("TestNodeUnit_Id_FK", testNodeUnitId);

  return updateRecord("TestNodeUnitSetup_tbl", data, matchData);
}

bool mdtTtTestModelItem::removeTestNodeUnitSetups(const mdtSqlTableSelection & s)
{
  return removeData("TestNodeUnitSetup_tbl", s, true);
}

/**
bool mdtTtTestModelItem::setTestLink(const QVariant & testItemId, const QVariant & testLinkBusAId, const QVariant & testLinkBusBId)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "UPDATE TestModelItem_tbl "\
        "SET TestLinkBusA_Id_FK = :TestLinkBusA_Id_FK, "\
        "    TestLinkBusB_Id_FK = :TestLinkBusB_Id_FK "\
        "WHERE Id_PK = " + testItemId.toString();
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test link edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelItem");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":TestLinkBusA_Id_FK", testLinkBusAId);
  query.bindValue(":TestLinkBusB_Id_FK", testLinkBusBId);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test link edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelItem");
    pvLastError.commit();
    return false;
  }

  return true;
}
*/

bool mdtTtTestModelItem::generateTestNodeUnitSetup(const QVariant & testItemId)
{
  Q_ASSERT(!testItemId.isNull());

  QList<QVariant> usedTestNodeUnitIdList;
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());
  ///QVariant testItemId;
  QVariant nodeUnitId;
  int i;

  qDebug() << "TI: begin transaction ...";
  if(!beginTransaction()){
    return false;
  }
  qDebug() << "TI: removing old setup ...";
  // Remove allready existing setups
  if(!removeTestNodeUnitSetupsForTestItemId(testItemId)){
    rollbackTransaction();
    return false;
  }
  qDebug() << "TI: get list of channel relays ...";
  // Get list of used channel relays
  usedTestNodeUnitIdList = getUsedNodeUnitIdList(testItemId, "CHANELREL");
  if(usedTestNodeUnitIdList.isEmpty()){
    rollbackTransaction();
    return false;
  }
  // Add setup for used node units
  for(i = 0; i < usedTestNodeUnitIdList.size(); ++i){
    nodeUnitId = usedTestNodeUnitIdList.at(i);
    qDebug() << "TI: adding setup for unit ID " << nodeUnitId << " ...";
    if(nodeUnitId.isNull()){
      pvLastError.setError("Try to add a setup for a NULL test node ID", mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelItem");
      pvLastError.commit();
      rollbackTransaction();
      return false;
    }
    if(!addTestNodeUnitSetup(testItemId, nodeUnitId, true)){
      rollbackTransaction();
      return false;
    }
  }
  qDebug() << "TI: commit transaction ...";
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTestModelItem::addTestNodeUnitSetup(const QVariant & testItemId, const QVariant & testNodeUnitId, const QVariant & state)
{
  QString sql;
  QSqlError sqlError;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "INSERT INTO TestNodeUnitSetup_tbl (TestModelItem_Id_FK, TestNodeUnit_Id_FK, State) "\
        "VALUES (:TestModelItem_Id_FK, :TestNodeUnit_Id_FK, :State)";
  if(!query.prepare(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for test node setupinsertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelItem");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":TestModelItem_Id_FK", testItemId);
  query.bindValue(":TestNodeUnit_Id_FK", testNodeUnitId);
  query.bindValue(":State", state);
  if(!query.exec()){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test node setup insertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelItem");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTestModelItem::removeTestNodeUnitSetup(const QVariant & tnusId)
{
  QSqlError sqlError;
  QString sql;
  QSqlQuery query(database());

  sql = "DELETE FROM TestNodeUnitSetup_tbl WHERE Id_PK = " + tnusId.toString();
  // Submit query
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test node unit setup deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelItem");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtTtTestModelItem::removeTestNodeUnitSetups(const QModelIndexList & indexListOfSelectedRows)
{
  int row;

  for(row = 0; row < indexListOfSelectedRows.size(); ++row){
    if(!removeTestNodeUnitSetup(indexListOfSelectedRows.at(row).data())){
      return false;
    }
  }

  return true;
}

bool mdtTtTestModelItem::removeTestNodeUnitSetupsForTestItemId(const QVariant & testItemId)
{
  QSqlError sqlError;
  QString sql;
  QSqlQuery query(database());

  sql = "DELETE FROM TestNodeUnitSetup_tbl WHERE TestModelItem_Id_FK = " + testItemId.toString();
  // Submit query
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for test node unit setup deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestModelItem");
    pvLastError.commit();
    return false;
  }

  return true;
}
