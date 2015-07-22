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
#include "mdtClUnitConnection.h"
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
