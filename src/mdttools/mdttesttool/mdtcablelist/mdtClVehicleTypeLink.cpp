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
#include "mdtClVehicleTypeLink.h"
#include "mdtSqlRecord.h"
#include "mdtSqlTransaction.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <algorithm>

//#include <QDebug>

mdtClVehicleTypeLink::mdtClVehicleTypeLink(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtClVehicleTypeLink::mdtClVehicleTypeLink(QSqlDatabase db)
 : mdtTtBase(nullptr, db)
{
}

bool mdtClVehicleTypeLink::addVehicleTypeLink(const mdtClVehicleTypeLinkKeyData & key)
{
  mdtSqlRecord record;

  if(!record.addAllFields("VehicleType_Link_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  fillRecord(record, key);
  if(!addRecord(record, "VehicleType_Link_tbl")){
    MDT_ERROR_SET_SRC(pvLastError, "mdtClVehicleTypeLink");
    return false;
  }

  return true;
}

bool mdtClVehicleTypeLink::addVehicleTypeLinks(const mdtClLinkPkData & linkPk, const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction)
{
  Q_ASSERT(!linkPk.isNull());

  mdtSqlTransaction transaction(database());

  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  for(const auto & vtKey : vehicleTypeList){
    mdtClVehicleTypeLinkKeyData key;
    key.setVehicleTypeStartEndFk(vtKey);
    key.setLinkFk(linkPk);
    if(!addVehicleTypeLink(key)){
      return false;
    }
  }
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      return false;
    }
  }

  return true;
}

QList<mdtClVehicleTypeLinkKeyData> mdtClVehicleTypeLink::getVehicleTypeLinkKeyDataList(const mdtClLinkPkData & pk, bool & ok)
{
  Q_ASSERT(!pk.isNull());

  QList<mdtClVehicleTypeLinkKeyData> keyList;
  QList<QSqlRecord> recordList;
  QString sql;

  sql = "SELECT * FROM VehicleType_Link_tbl " \
        " WHERE UnitConnectionStart_Id_FK = " + pk.connectionStartId.toString() + \
        " AND UnitConnectionEnd_Id_FK = " + pk.connectionEndId.toString() + \
        " AND Link_Version_FK = " + QString::number(pk.versionFk.versionPk.value()) + \
        " AND Link_Modification_Code_FK = '" + pk.modificationFk.code + "'";
  recordList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return keyList;
  }
  for(const auto & record : recordList){
    mdtClVehicleTypeLinkKeyData key;
    fillData(key, record);
    keyList.append(key);
  }

  return keyList;
}

QList<mdtClVehicleTypeStartEndKeyData> mdtClVehicleTypeLink::getVehicleTypeStartEndKeyDataList(const mdtClLinkPkData & pk, bool &ok)
{
  Q_ASSERT(!pk.isNull());

  QList<mdtClVehicleTypeStartEndKeyData> keyList;
  QList<QSqlRecord> recordList;
  QString sql;

  sql = "SELECT VehicleTypeStart_Id_FK,VehicleTypeEnd_Id_FK FROM VehicleType_Link_tbl " \
        " WHERE UnitConnectionStart_Id_FK = " + pk.connectionStartId.toString() + \
        " AND UnitConnectionEnd_Id_FK = " + pk.connectionEndId.toString() + \
        " AND Link_Version_FK = " + QString::number(pk.versionFk.versionPk.value()) + \
        " AND Link_Modification_Code_FK = '" + pk.modificationFk.code + "'";
  recordList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return keyList;
  }
  for(const auto & record : recordList){
    mdtClVehicleTypeStartEndKeyData key;
    fillData(key, record);
    keyList.append(key);
  }

  return keyList;
}

bool mdtClVehicleTypeLink::removeVehicleTypeLink(const mdtClVehicleTypeLinkKeyData & key)
{
  Q_ASSERT(!key.isNull());

  QSqlQuery query(database());
  QString sql;

  sql = "DELETE FROM VehicleType_Link_tbl WHERE VehicleTypeStart_Id_FK = " + key.vehicleTypeStartId().toString() \
      + " AND VehicleTypeEnd_Id_FK = " + key.vehicleTypeEndId().toString() \
      + " AND UnitConnectionStart_Id_FK = " + key.linkFk().connectionStartId.toString() \
      + " AND UnitConnectionEnd_Id_FK = " + key.linkFk().connectionEndId.toString() \
      + " AND Link_Version_FK = " + QString::number(key.linkFk().versionFk.versionPk.value()) \
      + " AND Link_Modification_Code_FK = '" + key.linkFk().modificationFk.code + "'";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Removing vehicle type - link assignation failed. SQL: ") + sql, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClVehicleTypeLink");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClVehicleTypeLink::removeVehicleTypeLinks(const mdtClLinkPkData & linkPk, const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction)
{
  Q_ASSERT(!linkPk.isNull());

  mdtSqlTransaction transaction(database());

  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  for(const auto & vtKey : vehicleTypeList){
    mdtClVehicleTypeLinkKeyData key;
    key.setVehicleTypeStartEndFk(vtKey);
    key.setLinkFk(linkPk);
    if(!removeVehicleTypeLink(key)){
      return false;
    }
  }
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      return false;
    }
  }

  return true;
}

bool mdtClVehicleTypeLink::removeVehicleTypeLinks(const mdtClLinkPkData & linkPk)
{
  Q_ASSERT(!linkPk.isNull());

  QSqlQuery query(database());
  QString sql;

  sql = "DELETE FROM VehicleType_Link_tbl WHERE UnitConnectionStart_Id_FK = " + linkPk.connectionStartId.toString() \
      + " AND UnitConnectionEnd_Id_FK = " + linkPk.connectionEndId.toString() \
      + " AND Link_Version_FK = " + QString::number(linkPk.versionFk.versionPk.value()) \
      + " AND Link_Modification_Code_FK = '" + linkPk.modificationFk.code + "'";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Removing vehicle type - link assignation failed. SQL: ") + sql, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClVehicleTypeLink");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClVehicleTypeLink::removeVehicleTypeLinks(const QVariant & unitId)
{
  Q_ASSERT(!unitId.isNull());

  QSqlQuery query(database());
  QString sql;

  /// \todo + version + modification ?
  sql = "DELETE FROM VehicleType_Link_tbl"\
        " WHERE UnitConnectionStart_Id_FK IN ("\
        "  SELECT L.UnitConnectionStart_Id_FK"\
        "   FROM Link_tbl L JOIN UnitConnection_tbl UCNX ON UCNX.Id_PK = L.UnitConnectionStart_Id_FK"\
        "   WHERE UCNX.Unit_Id_FK = " + unitId.toString() + ")"\
        " AND UnitConnectionEnd_Id_FK IN ("\
        "  SELECT L.UnitConnectionEnd_Id_FK"\
        "   FROM Link_tbl L JOIN UnitConnection_tbl UCNX ON UCNX.Id_PK = L.UnitConnectionEnd_Id_FK"\
        "   WHERE UCNX.Unit_Id_FK = " + unitId.toString() + ")";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Removing vehicle type - link assignation by unit failed. SQL: ") + sql, mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClVehicleTypeLink");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClVehicleTypeLink::updateVehicleTypeLink(const mdtClLinkPkData & linkPk, QList<mdtClVehicleTypeStartEndKeyData> expectedVehicleTypeKeyList, bool handleTransaction)
{
  Q_ASSERT(!linkPk.isNull());

  mdtSqlTransaction transaction(database());
  QList<mdtClVehicleTypeStartEndKeyData> existingVehicleTypeKeyList;
  QList<mdtClVehicleTypeStartEndKeyData> toRemoveVehicleTypeKeyList;
  QList<mdtClVehicleTypeStartEndKeyData> toAddVehicleTypeKeyList;
  mdtClVehicleTypeStartEndLessThan lessThan;
  bool ok;

  // Get actually vehicle types assigned to given link
  existingVehicleTypeKeyList = getVehicleTypeStartEndKeyDataList(linkPk, ok);
  if(!ok){
    return false;
  }
  // We must sort input data sets
  std::sort(expectedVehicleTypeKeyList.begin(), expectedVehicleTypeKeyList.end(), lessThan);
  std::sort(existingVehicleTypeKeyList.begin(), existingVehicleTypeKeyList.end(), lessThan);
  // Determine what assignations must be removed
  std::set_difference(existingVehicleTypeKeyList.constBegin(), existingVehicleTypeKeyList.constEnd(), \
                      expectedVehicleTypeKeyList.constBegin(), expectedVehicleTypeKeyList.constEnd(), \
                      std::inserter(toRemoveVehicleTypeKeyList, toRemoveVehicleTypeKeyList.begin()), lessThan );
  // Determine what assignations must be added
  std::set_difference(expectedVehicleTypeKeyList.constBegin(), expectedVehicleTypeKeyList.constEnd(), \
                      existingVehicleTypeKeyList.constBegin(), existingVehicleTypeKeyList.constEnd(), \
                      std::inserter(toAddVehicleTypeKeyList, toAddVehicleTypeKeyList.begin()), lessThan );
  // Update in database
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  // Remove
  if(!toRemoveVehicleTypeKeyList.isEmpty()){
    if(!removeVehicleTypeLinks(linkPk, toRemoveVehicleTypeKeyList, false)){
      return false;
    }
  }
  // Add
  if(!toAddVehicleTypeKeyList.isEmpty()){
    if(!addVehicleTypeLinks(linkPk, toAddVehicleTypeKeyList, false)){
      return false;
    }
  }
  // Commit
  if(handleTransaction){
    if(!transaction.commit()){
      pvLastError = transaction.lastError();
      return false;
    }
  }

  return true;
}

void mdtClVehicleTypeLink::fillRecord(mdtSqlRecord & record, const mdtClVehicleTypeLinkKeyData & key)
{
  Q_ASSERT(!key.isNull());
  Q_ASSERT(record.contains("VehicleTypeStart_Id_FK"));
  Q_ASSERT(record.contains("VehicleTypeEnd_Id_FK"));
  Q_ASSERT(record.contains("UnitConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("UnitConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("Link_Version_FK"));
  Q_ASSERT(record.contains("Link_Modification_Code_FK"));

  record.setValue("VehicleTypeStart_Id_FK", key.vehicleTypeStartId());
  record.setValue("VehicleTypeEnd_Id_FK", key.vehicleTypeEndId());
  record.setValue("UnitConnectionStart_Id_FK", key.linkFk().connectionStartId);
  record.setValue("UnitConnectionEnd_Id_FK", key.linkFk().connectionEndId);
  record.setValue("Link_Version_FK", key.linkFk().versionFk.versionPk.value());
  record.setValue("Link_Modification_Code_FK", key.linkFk().modificationFk.code);
}

void mdtClVehicleTypeLink::fillData(mdtClVehicleTypeLinkKeyData & key, const QSqlRecord & record)
{
  Q_ASSERT(record.contains("VehicleTypeStart_Id_FK"));
  Q_ASSERT(record.contains("VehicleTypeEnd_Id_FK"));
  Q_ASSERT(record.contains("UnitConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("UnitConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("Link_Version_FK"));
  Q_ASSERT(record.contains("Link_Modification_Code_FK"));

  mdtClLinkPkData linkFk;

  key.setVehicleTypeStartId(record.value("VehicleTypeStart_Id_FK"));
  key.setVehicleTypeEndId(record.value("VehicleTypeEnd_Id_FK"));
  linkFk.connectionStartId = record.value("UnitConnectionStart_Id_FK");
  linkFk.connectionEndId = record.value("UnitConnectionEnd_Id_FK");
  linkFk.versionFk.versionPk.setValue(record.value("Link_Version_FK").toInt());
  linkFk.modificationFk.code = record.value("Link_Modification_Code_FK").toString();
  key.setLinkFk(linkFk);
}

void mdtClVehicleTypeLink::fillData(mdtClVehicleTypeStartEndKeyData & key, const QSqlRecord & record)
{
  Q_ASSERT(record.contains("VehicleTypeStart_Id_FK"));
  Q_ASSERT(record.contains("VehicleTypeEnd_Id_FK"));

  key.setVehicleTypeStartId(record.value("VehicleTypeStart_Id_FK"));
  key.setVehicleTypeEndId(record.value("VehicleTypeEnd_Id_FK"));
}
