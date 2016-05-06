/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "mdtClAutoConnection.h"
#include "mdtClUnitConnection.h"
#include "mdtClLink.h"
#include "mdtSqlTransaction.h"

#include <QDebug>

mdtClAutoConnection::mdtClAutoConnection(QSqlDatabase db)
 : mdtTtBase(nullptr, db)
{
}

bool mdtClAutoConnection::canConnectConnections(const mdtClUnitConnectionData & a, const mdtClUnitConnectionData & b, const mdtClConnectableCriteria & criteria)
{
  Q_ASSERT(a.connectionType() != mdtClConnectionType_t::Undefined);
  Q_ASSERT(b.connectionType() != mdtClConnectionType_t::Undefined);

  /*
   * Check contact name if required
   */
  if(criteria.checkContactName){
    if(a.name.toString().trimmed() != b.name.toString().trimmed()){
      return false;
    }
  }
  /*
   * Check contact type if required
   */
  if(criteria.checkContactType){
    mdtClConnectionType_t aType = a.connectionType();
    mdtClConnectionType_t bType = b.connectionType();
    // Check case of terminals
    if( (aType == mdtClConnectionType_t::Terminal) && (bType != mdtClConnectionType_t::Terminal) ){
      return false;
    }
    if( (bType == mdtClConnectionType_t::Terminal) && (aType != mdtClConnectionType_t::Terminal) ){
      return false;
    }
    // Here, we have Socket and Pin
    if( (aType == mdtClConnectionType_t::Pin) && (bType == mdtClConnectionType_t::Pin) ){
      return false;
    }
    if( (aType == mdtClConnectionType_t::Socket) && (bType == mdtClConnectionType_t::Socket) ){
      return false;
    }
  }
  /*
   * Check if connection is part of a connector:
   *  - Both can simply be free (not a connector contact)
   *  - Both can be a connector contact, in this case, they must have the same name
   */
  // Check if connections are free (not part of a connector)
  if( (!a.isPartOfUnitConnector()) && (!b.isPartOfUnitConnector()) ){
    return true;
  }
  // If only one connection is part of a connector, they cannot be connected together
  if( (!a.isPartOfUnitConnector()) && b.isPartOfUnitConnector() ){
    return false;
  }
  if( (a.isPartOfUnitConnector()) && (!b.isPartOfUnitConnector()) ){
    return false;
  }

  return true;
}

bool mdtClAutoConnection::canConnectConnections(UnitConnectionPk pkA, UnitConnectionPk pkB, const mdtClConnectableCriteria & criteria, bool & ok)
{
  mdtClUnitConnection ucnx(database());
  mdtClUnitConnectionData a, b;

  // Get A and B connections data
  a = ucnx.getUnitConnectionData(pkA, ok);
  if(!ok){
    pvLastError = ucnx.lastError();
    return false;
  }
  if(a.connectionType() == mdtClConnectionType_t::Undefined){
    QString msg = tr("Connection");
    msg += " " + a.name.toString() + " ";
    msg += tr("has not type (Pin, Socket or Terminal");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClAutoConnection");
    pvLastError.commit();
    ok = false;
    return false;
  }
  b = ucnx.getUnitConnectionData(pkB, ok);
  if(!ok){
    pvLastError = ucnx.lastError();
    return false;
  }
  if(b.connectionType() == mdtClConnectionType_t::Undefined){
    QString msg = tr("Connection");
    msg += " " + b.name.toString() + " ";
    msg += tr("has not type (Pin, Socket or Terminal");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClAutoConnection");
    pvLastError.commit();
    ok = false;
    return false;
  }
  ok = true;

  return canConnectConnections(a, b, criteria);
}

// QList<mdtClUnitConnectionPkData> mdtClAutoConnection::getConnectableConnectionPkList(const mdtClUnitConnectionPkData & toPk,
//                                                                                    const mdtClConnectableCriteria & criteria, bool & ok)
// {
//   return getConnectableConnectionPkList(toPk, QVariant(), criteria, ok);
// }

UnitConnectionPkList mdtClAutoConnection::getConnectableConnectionPkList(UnitConnectionPk toPk, const mdtClConnectableCriteria & criteria, bool & ok)
{
  return getConnectableConnectionPkList(toPk, QVariant(), criteria, ok);
}

// QList<mdtClUnitConnectionPkData> mdtClAutoConnection::getConnectableConnectionPkList(const mdtClUnitConnectionPkData & toPk, const QVariant & unitId,
//                                                                                    const mdtClConnectableCriteria & criteria, bool & ok)
UnitConnectionPkList mdtClAutoConnection::getConnectableConnectionPkList(UnitConnectionPk toPk, const QVariant & unitId,
                                                                         const mdtClConnectableCriteria & criteria, bool & ok)
{
//   QList<mdtClUnitConnectionPkData> pkList;
  UnitConnectionPkList pkList;
  QString sql;
  QList<QVariant> idList;

  /*
   * Get a list of all unit connections
   */
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE ConnectionType_Code_FK IS NOT NULL ";
  if(!unitId.isNull()){
    sql += " AND Unit_Id_FK = " + unitId.toString();
  }
  idList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return pkList;
  }
  /*
   * For each unit connection , that is not the given connection, check if it is connectable to given connection
   */
  for(const auto & id : idList){
    if(id != QVariant(toPk.id())){
      UnitConnectionPk pk(id.toLongLong());
      if(canConnectConnections(pk, toPk, criteria, ok)){
        pkList.append(pk);
      }else{
        if(!ok){
          return pkList;
        }
      }
    }
  }

  return pkList;
}

bool mdtClAutoConnection::canConnectConnectors(const mdtClConnectorData & a, const mdtClConnectorData & b, const mdtClConnectableCriteria & criteria)
{
  // Check for gender if requested
  /**
   * \todo Currently, gender is a simple free text.
   *   Clarify how to enshure that user can only select 2 strings (f.ex. Male/Female),
   *   or something else that is cleaner that now,
   *   and fix this broken check !
   */
  if(criteria.checkGenderAreOpposite){
    if(a.gender.toString().trimmed() == b.gender.toString().trimmed()){
      return false;
    }
  }
  // Check for contact count if requested
  /// \todo Implement when a solution was found
  // Check for form if requested
  if(criteria.checkForm){
    if(a.form.toString().trimmed() != b.form.toString().trimmed()){
      return false;
    }
  }
  // Check for insert if requested
  if(criteria.checkInsert){
    pvLastError.setError("Insert checking currently not implemented..", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClAutoConnection");
    pvLastError.commit();
    return false;
  }
  // Check for insert rotation if requested
  if(criteria.checkInsertRotation){
    pvLastError.setError("Insert rotation checking currently not implemented..", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClAutoConnection");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClAutoConnection::canConnectConnectors(const mdtClConnectorPkData & pkA, const mdtClConnectorPkData & pkB,
                                               const mdtClConnectableCriteria & criteria, bool & ok)
{
  mdtClConnector cnr(database());
  mdtClConnectorData cnrA, cnrB;

  cnrA = cnr.getConnectorData(pkA, false, ok);
  if(!ok){
    pvLastError = cnr.lastError();
    return false;
  }
  cnrB = cnr.getConnectorData(pkB, false, ok);
  if(!ok){
    pvLastError = cnr.lastError();
    return false;
  }

  return canConnectConnectors(cnrA, cnrB, criteria);
}

bool mdtClAutoConnection::canConnectConnectors(const mdtClUnitConnectorData & a, const mdtClUnitConnectorData & b,
                                               const mdtClConnectableCriteria & criteria, bool & ok)
{
  // Check for contact count if requested
  if(criteria.checkContactCount){
    if(a.connectionDataList().size() != b.connectionDataList().size()){ /// \todo should be attribute of the connector
      return false;
    }
  }
  // If checks of base connectors are requested, a and b must be based on a base connector
  if( (criteria.checkGenderAreOpposite) || (criteria.checkForm) || (criteria.checkInsert) || (criteria.checkInsertRotation) ){
    if(!a.isBasedOnConnector() || !b.isBasedOnConnector()){
      QString msg;
      msg = tr("It was requested to check if connectors");
      msg += " '" + a.name.toString() + "' " + tr("and") + " '" + b.name.toString() + "' ";
      msg += tr("can be connected, with criteria that depends on connector model.\n");
      msg += tr("At least one of the two connectors is not based on a model, so those checks could not be done.");
      pvLastError.setError(msg, mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtClAutoConnection");
      pvLastError.commit();
      ok = false;
      return false;
    }
    Q_ASSERT(!a.keyData().connectorFk().isNull());
    Q_ASSERT(!b.keyData().connectorFk().isNull());
    if(!canConnectConnectors(a.keyData().connectorFk(), b.keyData().connectorFk(), criteria, ok)){
      return false;
    }
    Q_ASSERT(ok);
  }
  // Check connections
  return checkOrBuildConnectionLinkListByName(a.connectionDataList(), b.connectionDataList(), criteria);
}

bool mdtClAutoConnection::canConnectConnectors(const mdtClUnitConnectorPkData & pkA, const mdtClUnitConnectorPkData & pkB,
                                               const mdtClConnectableCriteria & criteria, bool & ok)
{
  mdtClUnitConnection ucnx(database());
  mdtClUnitConnectorData a, b;

  a = ucnx.getUnitConnectorData(pkA, true, ok);
  if(!ok){
    pvLastError = ucnx.lastError();
    return false;
  }
  b = ucnx.getUnitConnectorData(pkB, true, ok);
  if(!ok){
    pvLastError = ucnx.lastError();
    return false;
  }

  return canConnectConnectors(a, b, criteria, ok);
}

QList<mdtClUnitConnectorPkData> mdtClAutoConnection::getConnectableConnectorPkList(const mdtClUnitConnectorPkData & toPk, const QList<QVariant> & unitIdList,
                                                                                   const mdtClConnectableCriteria & criteria, bool & ok)
{
  QList<mdtClUnitConnectorPkData> pkList;
  QString sql;
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
  idList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return pkList;
  }
  /*
   * For each unit connector, that is not the given connector, check if it is connectable to given connector
   */
  for(const auto & id : idList){
    if(id != toPk.id){
      mdtClUnitConnectorPkData pk;
      pk.id = id;
      if(canConnectConnectors(pk, toPk, criteria, ok)){
        Q_ASSERT(ok);
        pkList.append(pk);
      }else{
        if(!ok){
          pkList.clear();
          return pkList;
        }
      }
    }
  }

  return pkList;
}

bool mdtClAutoConnection::connectByContactName(const mdtClUnitConnectorData & a, const mdtClUnitConnectorData & b,
                                               const LinkVersionPk & versionPk, const ModificationPk & modificationPk,
                                               const QList<mdtClVehicleTypeStartEndKeyData> & vtList,
                                               const mdtClConnectableCriteria & criteria, bool handleTransaction)
{
  mdtClLink lnk(database());
  mdtSqlTransaction transaction(database());
  QList<mdtClLinkData> linkDataList;

  // Build list of links
  if(!checkOrBuildConnectionLinkListByName(a.connectionDataList(), b.connectionDataList(), criteria, & linkDataList, &versionPk, &modificationPk)){
    QString msg;
    msg = tr("Could not find connections that can be connected from connector:");
    msg += " '" + a.name.toString() + "' ";
    msg += tr("to");
    msg += " '" + b.name.toString() + "'.";
    pvLastError.setError(msg, mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClAutoConnection");
    pvLastError.commit();
    return false;
  }
  // Begin transaction
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  // Add each link
  for(const auto & linkData : linkDataList){
    if(!lnk.addLink(linkData, vtList, false)){
      pvLastError = lnk.lastError();
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

bool mdtClAutoConnection::disconnectConnectors(const mdtClUnitConnectorPkData & pkA, const mdtClUnitConnectorPkData & pkB)
{
  mdtClLink lnk(database());
  mdtSqlTransaction transaction(database());
  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;

  // Create SQL statement to get start and end connection ID list
  sql = "SELECT UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK\n"\
        " FROM Link_tbl LNK\n"\
        " JOIN UnitConnection_tbl UCNXS ON UCNXS.Id_PK = LNK.UnitConnectionStart_Id_FK\n"\
        " JOIN UnitConnection_tbl UCNXE ON UCNXE.Id_PK = LNK.UnitConnectionEnd_Id_FK\n";
  sql += " WHERE UnitConnectorStart_Id_FK = " + pkA.id.toString();
  sql += " AND UnitConnectorEnd_Id_FK = " + pkB.id.toString();
  // Get list of links
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return false;
  }
  // Begin transaction
  if(!transaction.begin()){
    pvLastError = transaction.lastError();
    return false;
  }
  // Remove links
  for(const auto & record : dataList){
    LinkPk pk;
    pk.setConnectionStart( UnitConnectionPk::fromQVariant( record.value(0) ) );
    pk.setConnectionEnd( UnitConnectionPk::fromQVariant( record.value(1) ) );
    /// \todo Version ?
//     pk.connectionStartId = record.value(0);
//     pk.connectionEndId = record.value(1);
    if(!lnk.removeLink(pk, false)){
      pvLastError = lnk.lastError();
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

bool mdtClAutoConnection::checkOrBuildConnectionLinkListByName(const QList<mdtClUnitConnectionData> & A, const QList<mdtClUnitConnectionData> & B,
                                                               const mdtClConnectableCriteria & criteria, QList<mdtClLinkData> *connectionLinkDataList,
                                                               const LinkVersionPk *versionPk, const ModificationPk *modificationPk)
{
  mdtClUnitConnectionData c;
  const QList<mdtClUnitConnectionData> *C = nullptr;
  mdtClUnitConnectionData d;
  const QList<mdtClUnitConnectionData> *D = nullptr;
  QVariant var;

  // Assign lists
  if(A.size() >= B.size()){
    C = &A;
    D = &B;
  }else{
    C = &B;
    D = &A;
  }
  Q_ASSERT(C != nullptr);
  Q_ASSERT(D != nullptr);
  Q_ASSERT(C->size() >= D->size());

  if(connectionLinkDataList != nullptr){
    connectionLinkDataList->clear();
    Q_ASSERT(versionPk != nullptr);
    Q_ASSERT(modificationPk != nullptr);
  }
  // Search in C
  for(const auto & c : *C){
    var = c.name;
    // Search connection with same name in D
    for(const auto & d : *D){
      // Check if we can connect c and d connections
      if(canConnectConnections(c, d, criteria)){
        // If requested, add to connection link list
        if(connectionLinkDataList != nullptr){
          LinkPk pk;
          pk.setConnectionStart(c.keyData().pk());
          pk.setConnectionEnd(d.keyData().pk());
          pk.setVersion(*versionPk);
          pk.setModification(*modificationPk);
//           pk.connectionStartId = c.keyData().pk().id();
//           pk.connectionEndId = d.keyData().pk().id();
//           pk.versionFk = *versionPk;
//           pk.modificationFk = *modificationPk;
          mdtClLinkData data;
          data.setPk(pk);
          data.setLinkType(LinkType::Connection);
          data.setLinkDirection(LinkDirectionType::Bidirectional);
          connectionLinkDataList->append(data);
        }
        // DONE for current c connection
        break;
      }
    }
  }

  return true;
}
