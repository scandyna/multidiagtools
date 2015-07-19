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
#include "mdtClLink.h"
#include "mdtClVehicleTypeLink.h"
#include "mdtClVehicleTypeLinkData.h"
#include "mdtSqlTransaction.h"
#include "mdtClUnit.h"
#include "mdtError.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

//#include <QDebug>

mdtClLink::mdtClLink(QObject* parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtClLink::mdtClLink(QSqlDatabase db)
 : mdtTtBase(nullptr, db)
{
}

mdtClLink::~mdtClLink()
{
}

bool mdtClLink::addLink(const mdtClLinkData & linkData, const mdtClLinkModificationKeyData & modification, bool handleTransaction)
{
  mdtSqlTransaction transaction(database());
  mdtSqlRecord record;

  // Setup record for Link_tbl part
  if(!record.addAllFields("Link_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  fillRecord(record, linkData);
  // Init transaction
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  // Add Link_tbl part
  if(!addRecord(record, "Link_tbl")){
    return false;
  }
  // Add LinkModification_tbl part
  if(!addModification(modification)){
    return false;
  }
  // Commit transaction
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      return false;
    }
  }

  return true;
}

bool mdtClLink::addLink(const mdtClLinkData & linkData, const mdtClLinkModificationKeyData & modification, const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction)
{
  mdtClVehicleTypeLink vtl(database());
  mdtSqlTransaction transaction(database());

  // Begin transaction
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  // Add Link_tbl and LinkModification_tbl parts
  if(!addLink(linkData, modification, false)){
    return false;
  }
  // Add VehicleType_Link_tbl part
  if(!vtl.addVehicleTypeLinks(linkData.pk(), vehicleTypeList, false)){
    return false;
  }
  // Commit transaction
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      return false;
    }
  }

  return true;
}

bool mdtClLink::addLink(const mdtClArticleLinkData & articleLinkData, const QVariant & unitId)
{
  Q_ASSERT(!articleLinkData.isNull());
  Q_ASSERT(!unitId.isNull());

  QList<QSqlRecord> dataList;
  QString sql;
  mdtClArticleLinkPkData articleLinkPk = articleLinkData.keyData().pk;
  mdtClLinkPkData linkPk;
  mdtClLinkData linkData;
  mdtSqlRecord record;
  bool ok;

  // Get unit connection start and end
  sql = "SELECT UCNXS.Id_PK AS UnitConnectionStart_Id_FK, UCNXE.Id_PK AS UnitConnectionEnd_Id_FK\n"\
        " FROM ArticleLink_tbl ALNK\n"\
        " JOIN UnitConnection_tbl UCNXS\n"\
        "  ON UCNXS.ArticleConnection_Id_FK = ALNK.ArticleConnectionStart_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXE\n"\
        "  ON UCNXE.ArticleConnection_Id_FK = ALNK.ArticleConnectionEnd_Id_FK\n"\
        " WHERE ALNK.ArticleConnectionStart_Id_FK = " + articleLinkPk.connectionStartId.toString() + \
        " AND ALNK.ArticleConnectionEnd_Id_FK = " + articleLinkPk.connectionEndId.toString() + \
        " AND UCNXS.Unit_Id_FK = " + unitId.toString() + \
        " AND UCNXE.Unit_Id_FK = " + unitId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return false;
  }
  if(dataList.isEmpty()){
    QString msg = tr("No link can be created on base of given article link in given unit because at least on unit connection is missing.\n");
    msg += tr("Article link: ") + articleLinkData.indetification.toString() + tr(", unit ID: ") + unitId.toString();
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    return false;
  }
  Q_ASSERT(dataList.size() == 1);
  // Setup link data
  linkPk.connectionStartId = dataList.at(0).value("UnitConnectionStart_Id_FK");
  linkPk.connectionEndId = dataList.at(0).value("UnitConnectionEnd_Id_FK");
  linkData.setPk(linkPk);
  linkData.setArticleLinkFk(articleLinkData.keyData().pk);
  linkData.setLinkType(articleLinkData.linkType());
  linkData.setLinkDirection(articleLinkData.linkDirection());
  linkData.identification = articleLinkData.indetification;
  if(!articleLinkData.resistance.isNull()){
    linkData.resistance = articleLinkData.resistance.toDouble();
  }
  // Setup record
  if(!record.addAllFields("Link_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  fillRecord(record, linkData);
  // Add to database
  if(!addRecord(record, "Link_tbl")){
    return false;
  }

  return true;
}

bool mdtClLink::linkExists(const mdtClLinkPkData & pk, bool & ok)
{
  Q_ASSERT(!pk.isNull());

  QList<QVariant> dataList;
  QString sql;

  sql = "SELECT COUNT(*) FROM Link_tbl"\
        " WHERE UnitConnectionStart_Id_FK = " + pk.connectionStartId.toString() + \
        " AND UnitConnectionEnd_Id_FK = " + pk.connectionEndId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return false;
  }
  Q_ASSERT(dataList.size() == 1);

  return (dataList.at(0).toInt() > 0);
}

mdtClLinkData mdtClLink::getLinkData(const mdtClLinkPkData & pk, bool & ok)
{
  mdtClLinkData linkData;
  QList<QSqlRecord> dataList;
  QString sql;

  // Get link data part
  sql = "SELECT * FROM Link_tbl";
  sql += " WHERE UnitConnectionStart_Id_FK = " + pk.connectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + pk.connectionEndId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return linkData;
  }
  if(dataList.isEmpty()){
    return linkData;
  }
  Q_ASSERT(dataList.size() == 1);
  fillData(linkData, dataList.at(0));

  return linkData;
}

mdtClLinkData mdtClLink::getLinkData(const mdtClArticleLinkPkData articleLinkPk, const QVariant &startUnitId, const QVariant &endUnitId, bool & ok)
{
  mdtClLinkData data;
  mdtClLinkPkData pk;
  QList<QSqlRecord> dataList;
  QString sql;

  // Get unit connection start and end
  sql = "SELECT UCNXS.Id_PK AS UnitConnectionStart_Id_FK, UCNXE.Id_PK AS UnitConnectionEnd_Id_FK\n"\
        " FROM ArticleLink_tbl ALNK\n"\
        " JOIN UnitConnection_tbl UCNXS\n"\
        "  ON UCNXS.ArticleConnection_Id_FK = ALNK.ArticleConnectionStart_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXE\n"\
        "  ON UCNXE.ArticleConnection_Id_FK = ALNK.ArticleConnectionEnd_Id_FK\n"\
        " WHERE ALNK.ArticleConnectionStart_Id_FK = " + articleLinkPk.connectionStartId.toString() + \
        " AND ALNK.ArticleConnectionEnd_Id_FK = " + articleLinkPk.connectionEndId.toString() + \
        " AND UCNXS.Unit_Id_FK = " + startUnitId.toString() + \
        " AND UCNXE.Unit_Id_FK = " + endUnitId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  // Get link data
  pk.connectionStartId = dataList.at(0).value("UnitConnectionStart_Id_FK");
  pk.connectionEndId = dataList.at(0).value("UnitConnectionEnd_Id_FK");
  data = getLinkData(pk, ok);

  return data;
}

bool mdtClLink::updateLink(const mdtClLinkPkData & linkPk, const mdtClLinkData & linkData,
                           const mdtClLinkModificationKeyData & oldModification, const mdtClLinkModificationKeyData & modification,
                           const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction)
{
  Q_ASSERT(!linkPk.isNull());
  Q_ASSERT(!linkData.isNull());

  mdtClVehicleTypeLink vtl(database());
  mdtSqlTransaction transaction(database());
  bool pkChanged = false;

  // Check if link PK has changed
  if( (linkData.pk().connectionStartId != linkPk.connectionStartId) || (linkData.pk().connectionEndId != linkPk.connectionEndId) ){
    pkChanged = true;
  }
  // Begin transaction
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  // If link PK has changed, we simply remove and re-add the link
  if(pkChanged){
    if(!removeLink(linkPk, false)){
      return false;
    }
    if(!addLink(linkData, modification, vehicleTypeList, false)){
      return false;
    }
  }else{
    // Update LinkModification_tbl part
    if(!updateModification(oldModification, modification, false)){
      return false;
    }
    // Update VehicleType_Link_tbl part
    if(!vtl.updateVehicleTypeLink(linkPk, vehicleTypeList, false)){
      return false;
    }
    // Update Link_tbl part
    mdtSqlRecord record;
    if(!record.addAllFields("Link_tbl", database())){
      pvLastError = record.lastError();
      return false;
    }
    fillRecord(record, linkData);
    if(!updateRecord("Link_tbl", record, "UnitConnectionStart_Id_FK", linkPk.connectionStartId, "UnitConnectionEnd_Id_FK", linkPk.connectionEndId)){
      return false;
    }
  }
  // Commit transaction
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      return false;
    }
  }

  return true;
}

bool mdtClLink::removeLink(const mdtClLinkPkData & pk, bool handleTransaction)
{
  mdtClVehicleTypeLink vtl(database());
  mdtSqlTransaction transaction(database());

  // Begin transaction
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  // Remove VehicleType_Link_tbl part
  if(!vtl.removeVehicleTypeLinks(pk)){
    return false;
  }
  // Remove LinkModification_tbl part
  if(!removeModifications(pk)){
    return false;
  }
  // Remove Link_tbl part
  if(!removeData("Link_tbl", "UnitConnectionStart_Id_FK", pk.connectionStartId, "UnitConnectionEnd_Id_FK", pk.connectionEndId)){
    return false;
  }
  // Commit transaction
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      return false;
    }
  }

  return true;
}

bool mdtClLink::removeLinks(const mdtSqlTableSelection & s)
{
  mdtSqlTransaction transaction(database());
  int i;

  if(!transaction.begin()){
    pvLastError = transaction.lastError();
    return false;
  }
  for(i = 0; i < s.rowCount(); ++i){
    mdtClLinkPkData pk;
    pk.connectionStartId = s.data(i, "UnitConnectionStart_Id_FK");
    pk.connectionEndId = s.data(i, "UnitConnectionEnd_Id_FK");
    if(!removeLink(pk, false)){
      return false;
    }
  }
  if(!transaction.commit()){
    pvLastError = transaction.lastError();
    return false;
  }

  return true;
}

bool mdtClLink::addModification(const mdtClLinkModificationKeyData & key)
{
  mdtSqlRecord record;

  if(!record.addAllFields("LinkModification_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("UnitConnectionStart_Id_FK", key.linkFk().connectionStartId);
  record.setValue("UnitConnectionEnd_Id_FK", key.linkFk().connectionEndId);
  record.setValue("Version_FK", key.linkVersionFk().versionPk.value());
  record.setValue("Modification_Code_FK", key.modificationFk().code);

  return addRecord(record, "LinkModification_tbl");
}

bool mdtClLink::updateModification(const mdtClLinkModificationKeyData &originalModification, const mdtClLinkModificationKeyData &newModification, bool handleTransaction)
{
  mdtSqlTransaction transaction(database());

  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  if(!removeModification(originalModification)){
    return false;
  }
  if(!addModification(newModification)){
    return false;
  }
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      return false;
    }
  }

  return true;
}

bool mdtClLink::removeModification(const mdtClLinkModificationKeyData & key)
{
  QSqlQuery query(database());
  QString sql;

  sql = "DELETE FROM LinkModification_tbl"\
        " WHERE UnitConnectionStart_Id_FK = :UnitConnectionStart_Id_FK"\
        " AND UnitConnectionEnd_Id_FK = :UnitConnectionEnd_Id_FK"\
        " AND Version_FK = :Version_FK"\
        " AND Modification_Code_FK = :Modification_Code_FK";
  query.prepare(sql);
  query.bindValue(":UnitConnectionStart_Id_FK", key.linkFk().connectionStartId);
  query.bindValue(":UnitConnectionEnd_Id_FK", key.linkFk().connectionEndId);
  query.bindValue(":Version_FK", key.linkVersionFk().versionPk.value());
  query.bindValue(":Modification_Code_FK", key.modificationFk().code);
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot remove data from 'LinkModification_tbl'. SQL: ") + query.lastQuery(), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClLink::removeModifications(const mdtClLinkPkData & linkPk)
{
  return removeData("LinkModification_tbl", "UnitConnectionStart_Id_FK", linkPk.connectionStartId, "UnitConnectionEnd_Id_FK", linkPk.connectionEndId);
}

QList<mdtClLinkData> mdtClLink::getConnectionLinkListByName(const QList< mdtClUnitConnectionData >& A, const QList< mdtClUnitConnectionData >& B, const mdtClConnectableCriteria& criteria)
{
  QList<mdtClLinkData> linkDataList;

  if(!checkOrBuildConnectionLinkListByName(A, B, criteria, &linkDataList)){
    linkDataList.clear();
    return linkDataList;
  }

  return linkDataList;
}

bool mdtClLink::canConnectConnections(const mdtClUnitConnectionData& S, const mdtClUnitConnectionData& E, const mdtClConnectableCriteria& criteria)
{
  Q_ASSERT(!S.value("ConnectionType_Code_FK").isNull());
  Q_ASSERT(!E.value("ConnectionType_Code_FK").isNull());

  /*
   * Check contact name if required
   */
  if(criteria.checkContactName){
    if(S.value("UnitContactName").toString().trimmed() != E.value("UnitContactName").toString().trimmed()){
      return false;
    }
  }
  /*
   * Check contact type if required
   */
  if(criteria.checkContactType){
    QString sType = S.value("ConnectionType_Code_FK").toString();
    QString eType = E.value("ConnectionType_Code_FK").toString();
    // Check case of terminals
    if((sType == "T")&&(eType != "T")){
      return false;
    }
    if((eType == "T")&&(sType != "T")){
      return false;
    }
    // Here, we have Socket and Pin
    if((sType == "P")&&(eType == "P")){
      return false;
    }
    if((sType == "S")&&(eType == "S")){
      return false;
    }
  }
  /*
   * Check if connection is part of a connector:
   *  - Both can simply be free (not a connector contact)
   *  - Both can be a connector contact, in this case, they must have the same name
   */
  // Check if connections are free (not part of a connector)
  if((S.value("UnitConnector_Id_FK").isNull())&&(E.value("UnitConnector_Id_FK").isNull())){
    return true;
  }
  // If only one connection is part of a connector, they cannot be connected together
  if((S.value("UnitConnector_Id_FK").isNull())&&(!E.value("UnitConnector_Id_FK").isNull())){
    return false;
  }
  if((!S.value("UnitConnector_Id_FK").isNull())&&(E.value("UnitConnector_Id_FK").isNull())){
    return false;
  }
  
  return true;
  
  /**
  // Here, both connections are part of a connector, check that contact names matches
  Q_ASSERT(!S.value("UnitConnector_Id_FK").isNull());
  Q_ASSERT(!E.value("UnitConnector_Id_FK").isNull());
  if(S.value("UnitContactName").toString().trimmed() != E.value("UnitContactName").toString().trimmed()){
    return true;
  }

  return false;
  */
}

bool mdtClLink::canConnectConnections(const QVariant& unitConnectionIdA, const QVariant& unitConnectionIdB, const mdtClConnectableCriteria& criteria, bool& ok)
{
  mdtClUnit unit(0, database());
  mdtClUnitConnectionData A, B;

  // Get A and B connections data
  A = unit.getConnectionData(unitConnectionIdA, false, &ok);
  if(!ok){
    pvLastError = unit.lastError();
    return false;
  }
  if(A.value("ConnectionType_Code_FK").isNull()){
    QString msg = tr("Connection");
    msg += " " + A.value("UnitContactName").toString() + " ";
    msg += tr("has not type (Pin, Socket or Terminal");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    ok = false;
    return false;
  }
  B = unit.getConnectionData(unitConnectionIdB, false, &ok);
  if(!ok){
    pvLastError = unit.lastError();
    return false;
  }
  if(B.value("ConnectionType_Code_FK").isNull()){
    QString msg = tr("Connection");
    msg += " " + B.value("UnitContactName").toString() + " ";
    msg += tr("has not type (Pin, Socket or Terminal");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    ok = false;
    return false;
  }
  ok = true;

  return canConnectConnections(A, B, criteria);
}

bool mdtClLink::canConnectConnectors(const mdtClUnitConnectorData& S, const mdtClUnitConnectorData& E, const mdtClConnectableCriteria& criteria)
{
  mdtClConnectorData cnrS, cnrE;

  // Get connector data
  cnrS = S.connectorData();
  cnrE = E.connectorData();
  // Check for gender if requested
  /**
   * \todo Currently, gender is a simple free text.
   *   Clarify how to enshure that user can only select 2 strings (f.ex. Male/Female),
   *   or something else that is cleaner that now,
   *   and fix this broken check !
   */
  if(criteria.checkGenderAreOpposite){
    if(cnrS.value("Gender").toString().trimmed() == cnrE.value("Gender").toString().trimmed()){
      return false;
    }
  }
  // Check for contact count if requested
  if(criteria.checkContactCount){
    if(S.connectionDataList().size() != E.connectionDataList().size()){
      return false;
    }
  }
  // Check for form if requested
  if(criteria.checkForm){
    if(cnrS.value("Form").toString().trimmed() != cnrE.value("Form").toString().trimmed()){
      return false;
    }
  }
  // Check for insert if requested
  if(criteria.checkInsert){
    if(cnrS.value("Insert").toString().trimmed() != cnrE.value("Insert").toString().trimmed()){
      return false;
    }
  }
  // Check for insert rotation if requested
  if(criteria.checkInsertRotation){
    if(cnrS.value("InsertRotation").toString().trimmed() != cnrE.value("InsertRotation").toString().trimmed()){
      return false;
    }
  }
  // Check connections
  return checkOrBuildConnectionLinkListByName(S.connectionDataList(), E.connectionDataList(), criteria);
}

bool mdtClLink::canConnectConnectors(const QVariant& startUnitConnectorId, const QVariant& endUnitConnectorId, const mdtClConnectableCriteria& criteria, bool& ok)
{
  mdtClUnit unit(0, database());
  mdtClUnitConnectorData S, E;

  // Get start and end connector data
  S = unit.getConnectorData(startUnitConnectorId, &ok, true, false, true);
  if(!ok){
    pvLastError = unit.lastError();
    return false;
  }
  E = unit.getConnectorData(endUnitConnectorId, &ok, true, false, true);
  if(!ok){
    pvLastError = unit.lastError();
    return false;
  }
  ok = true;

  return canConnectConnectors(S, E, criteria);
}

QString mdtClLink::sqlForConnectableUnitConnectionsSelection(const QVariant& unitConnectionId, const QVariant& unitId, const mdtClConnectableCriteria& criteria)
{
  QString sql;
  QList<QVariant> dataList;
  QVariant id;
  QList<QVariant> idList;
  int i;
  bool ok;

  /*
   * Get a list of all unit connections
   */
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE ConnectionType_Code_FK IS NOT NULL ";
  if(!unitId.isNull()){
    sql += " AND Unit_Id_FK = " + unitId.toString();
  }
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return QString();
  }
  /*
   * For each unit connection ID, that is not the ID of given connection, check if it is connectable to given connection ID
   */
  for(i = 0; i < dataList.size(); ++i){
    id = dataList.at(i);
    if(id != unitConnectionId){
      if(canConnectConnections(unitConnectionId, id, criteria , ok)){
        idList.append(id);
      }else{
        if(!ok){
          return QString();
        }
      }
    }
  }
  /*
   * Generate SQL statement
   */
  if(idList.isEmpty()){
    pvLastError.setError(tr("Could not find connectable connections."), mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    ok = false;
    return QString();
  }
  sql = "SELECT * FROM UnitConnection_view ";
  if(unitId.isNull()){
    sql += " WHERE ";
  }else{
    sql += " WHERE (Unit_Id_FK = " + unitId.toString() + ") AND ";
  }
  Q_ASSERT(idList.size() > 0);
  sql += " UnitConnection_Id_PK IN (";
  for(i = 0; i < idList.size(); ++i){
    sql += idList.at(i).toString();
    if(i < (idList.size() - 1)){
      sql += ",";
    }
  }
  sql += ")";
  ok = true;

  return sql;
}

QString mdtClLink::sqlForConnectableUnitConnectorsSelection(const QVariant& unitConnectorId, const QVariant& unitId, const mdtClConnectableCriteria& criteria, bool& ok)
{
  QList<QVariant> unitIdList;

  if(!unitId.isNull()){
    unitIdList.append(unitId);
  }

  return sqlForConnectableUnitConnectorsSelection(unitConnectorId, unitIdList, criteria, ok);
}

QString mdtClLink::sqlForConnectableUnitConnectorsSelection(const QVariant& unitConnectorId, const QList< QVariant >& unitIdList, const mdtClConnectableCriteria& criteria, bool& ok)
{
  QString sql;
  QList<QVariant> dataList;
  QVariant id;
  QList<QVariant> idList;
  int i;
  int lastIndex;

  /*
   * Get a list of all unit connectors
   */
  sql = "SELECT Id_PK FROM UnitConnector_tbl";
  if(!unitIdList.isEmpty()){
    sql += " WHERE Unit_Id_FK IN(";
    lastIndex = unitIdList.size() - 1;
    for(i = 0; i < lastIndex; ++i){
      sql += unitIdList.at(i).toString() + ",";
    }
    sql += unitIdList.at(lastIndex).toString() + ")";
  }
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return QString();
  }
  /*
   * For each unit connector ID, that is not the ID of given connector, check if it is connectable to given connector ID
   */
  for(i = 0; i < dataList.size(); ++i){
    id = dataList.at(i);
    if(id != unitConnectorId){
      if(canConnectConnectors(unitConnectorId, id, criteria , ok)){
        idList.append(id);
      }else{
        if(!ok){
          return QString();
        }
      }
    }
  }
  /*
   * Generate SQL statement
   */
  if(idList.isEmpty()){
    pvLastError.setError(tr("Could not find connectable connectors."), mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    ok = false;
    return QString();
  }
  Q_ASSERT(idList.size() > 0);
  sql = "SELECT * FROM UnitConnector_view WHERE Id_PK IN(";
  lastIndex = idList.size() - 1;
  for(i = 1; i < idList.size(); ++i){
    sql += idList.at(i).toString() + ",";
  }
  sql += idList.at(lastIndex).toString() + ")";
  ok = true;

  return sql;
}

bool mdtClLink::connectByContactName(const mdtClUnitConnectorData & S, const mdtClUnitConnectorData & E,
                                     const QVariant & startVehicleTypeId, const QVariant & endVehicleTypeId,
                                     const mdtClConnectableCriteria & criteria)
{
  QList<mdtClLinkData> linkDataList;
  QList<QVariant> vtStartIdList, vtEndIdList;
  int i;

  // Get connection links
  if(!checkOrBuildConnectionLinkListByName(S.connectionDataList(), E.connectionDataList(), criteria, &linkDataList)){
    QString msg;
    msg = tr("Could not find connections that can be connected from connector:");
    msg += " '" + S.value("Name").toString() + "' ";
    msg += tr("to");
    msg += " '" + E.value("Name").toString() + "'.";
    pvLastError.setError(msg, mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    return false;
  }
  // Add start/end vehicle types to link data
  vtStartIdList.append(startVehicleTypeId);
  vtEndIdList.append(endVehicleTypeId);
  for(i = 0; i < linkDataList.size(); ++i){
    /**
    if(!buildVehicleTypeLinkDataList(linkDataList[i], vtStartIdList, vtEndIdList)){
      return false;
    }
    */
  }

  ///return addLinks(linkDataList, true);
}

bool mdtClLink::connectByContactName(const QVariant & startUnitConnectorId, const QVariant & endUnitConnectorId,
                                     const QVariant & startVehicleTypeId, const QVariant & endVehicleTypeId,
                                     const mdtClConnectableCriteria & criteria)
{
  mdtClUnit unit(0, database());
  mdtClUnitConnectorData S, E;
  bool ok;

  // Get start and end connector data
  S = unit.getConnectorData(startUnitConnectorId, &ok, true, false, false);
  if(!ok){
    pvLastError = unit.lastError();
    return false;
  }
  E = unit.getConnectorData(endUnitConnectorId, &ok, true, false, false);
  if(!ok){
    pvLastError = unit.lastError();
    return false;
  }

  return connectByContactName(S, E, startVehicleTypeId, endVehicleTypeId, criteria);
}

bool mdtClLink::disconnectConnectors(const QVariant & startUnitConnectorId, const QVariant & endUnitConnectorId, const QList<QVariant> & startVehicleTypeIdList, const QList<QVariant> & endVehicleTypeIdList)
{
  mdtSqlTransaction transaction(database());
  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;
  int i;

  // Create SQL statement to get start and end connection ID list
  sql = "SELECT UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK FROM LinkList_view ";
  sql += " WHERE UnitConnectorStart_Id_FK = " + startUnitConnectorId.toString();
  sql += " AND UnitConnectorEnd_Id_FK = " + endUnitConnectorId.toString();
  // Get list of links
  dataList = getData(sql, &ok);
  if(!ok){
    return false;
  }
  // Begin transaction
  if(!transaction.begin()){
    pvLastError = transaction.lastError();
    return false;
  }
  // Remove links
  for(i = 0; i < dataList.size(); ++i){
    mdtClLinkPkData pk;
    pk.connectionStartId = dataList.at(i).value(0);
    pk.connectionEndId = dataList.at(i).value(1);
    if(!removeLink(pk, false)){
      return false;
    }
  }
  // Commit
  if(!transaction.commit()){
    pvLastError = transaction.lastError();
    return false;
  }

  return true;
}

bool mdtClLink::checkOrBuildConnectionLinkListByName(const QList< mdtClUnitConnectionData >& A, const QList< mdtClUnitConnectionData >& B, const mdtClConnectableCriteria& criteria, QList< mdtClLinkData >* connectionLinkDataList)
{
  mdtClLinkData data;
  mdtClUnitConnectionData c;
  const QList<mdtClUnitConnectionData> *C = 0;
  mdtClUnitConnectionData d;
  const QList<mdtClUnitConnectionData> *D = 0;
  QVariant var;
  int i, j;

  // Assign lists
  if(A.size() >= B.size()){
    C = &A;
    D = &B;
  }else{
    C = &B;
    D = &A;
  }
  Q_ASSERT(C != 0);
  Q_ASSERT(D != 0);
  Q_ASSERT(C->size() >= D->size());
  // Setup link data if required
  if(connectionLinkDataList != 0){
    if(!data.setup(database())){
      pvLastError = data.lastError();
      return false;
    }
  }
  // Search in C
  for(i = 0; i < C->size(); ++i){
    c = C->at(i);
    var = c.value("UnitContactName");
    // Search connection with same name in D
    for(j = 0; j < D->size(); ++j){
      if(D->at(j).value("UnitContactName") == var){
        d = D->at(j);
        // Check if we can connect c and d connections
        if(canConnectConnections(c, d, criteria)){
          // If requested, add to connection link list
          if(connectionLinkDataList != 0){
            data.clearValues();
            data.setStartConnectionData(c);
            data.setEndConnectionData(d);
            data.setValue("LinkType_Code_FK", "CONNECTION");
            data.setValue("LinkDirection_Code_FK", "BID");
            connectionLinkDataList->append(data);
          }
          // DONE for current c connection
          break;
        }
      }
    }
  }

  return true;
}

void mdtClLink::fillRecord(mdtSqlRecord &record, const mdtClLinkData &data)
{
  Q_ASSERT(record.contains("UnitConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("UnitConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("LinkType_Code_FK"));
  Q_ASSERT(record.contains("LinkDirection_Code_FK"));
  Q_ASSERT(record.contains("ArticleConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("ArticleConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("Wire_Id_FK"));
  Q_ASSERT(record.contains("Identification"));
  Q_ASSERT(record.contains("Resistance"));
  Q_ASSERT(record.contains("Length"));

  mdtClLinkPkData pk = data.pk();
  mdtClArticleLinkPkData articleLinkFk = data.keyData().articleLinkFk();

  record.setValue("UnitConnectionStart_Id_FK", pk.connectionStartId);
  record.setValue("UnitConnectionEnd_Id_FK", pk.connectionEndId);
  record.setValue("LinkType_Code_FK", data.keyData().linkTypeFk().code);
  record.setValue("LinkDirection_Code_FK", data.keyData().linkDirectionFk().code);
  record.setValue("ArticleConnectionStart_Id_FK", articleLinkFk.connectionStartId);
  record.setValue("ArticleConnectionEnd_Id_FK", articleLinkFk.connectionEndId);
  record.setValue("Wire_Id_FK", data.keyData().wireId());
  record.setValue("Identification", data.identification);
  record.setValue("Resistance", data.resistance.value());
  record.setValue("Length", data.length.value());
}

void mdtClLink::fillData(mdtClLinkData &data, const QSqlRecord &record)
{
  Q_ASSERT(record.contains("UnitConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("UnitConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("LinkType_Code_FK"));
  Q_ASSERT(record.contains("LinkDirection_Code_FK"));
  Q_ASSERT(record.contains("ArticleConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("ArticleConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("Wire_Id_FK"));
  Q_ASSERT(record.contains("Identification"));
  Q_ASSERT(record.contains("Resistance"));
  Q_ASSERT(record.contains("Length"));

  mdtClLinkPkData pk;
  mdtClArticleLinkPkData articleLinkFk;

  pk.connectionStartId = record.value("UnitConnectionStart_Id_FK");
  pk.connectionEndId = record.value("UnitConnectionEnd_Id_FK");
  articleLinkFk.connectionStartId = record.value("ArticleConnectionStart_Id_FK");
  articleLinkFk.connectionEndId = record.value("ArticleConnectionEnd_Id_FK");
  data.setPk(pk);
  data.setLinkTypeCode(record.value("LinkType_Code_FK"));
  data.setLinkDirectionCode(record.value("LinkDirection_Code_FK"));
  data.setArticleLinkFk(articleLinkFk);
  data.setWireId(record.value("Wire_Id_FK"));
  data.identification = record.value("Identification");
  data.resistance = record.value("Resistance").toDouble();
  data.length = record.value("Length").toDouble();
}
