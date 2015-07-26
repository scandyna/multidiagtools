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
#include "mdtClArticleLink.h"
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

bool mdtClLink::addLink(const mdtClLinkData & linkData)
{
  mdtSqlRecord record;

  // Setup record
  if(!record.addAllFields("Link_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  fillRecord(record, linkData);
  // Add to Link_tbl
  if(!addRecord(record, "Link_tbl")){
    return false;
  }

  return true;
}

bool mdtClLink::addLink(const mdtClLinkData & linkData, const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction)
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
  // Add Link_tbl part
  if(!addLink(linkData)){
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

bool mdtClLink::addLinkList(const QList<mdtClArticleLinkUnitConnectionKeyData> & keyList,
                            const mdtClLinkVersionPkData & versionPk, const mdtClModificationPkData & modificationPk, bool handleTransaction)
{
  return addLinkList(keyList, versionPk, modificationPk, QList<mdtClVehicleTypeStartEndKeyData>(), handleTransaction);
}

bool mdtClLink::addLinkList(const QList<mdtClArticleLinkUnitConnectionKeyData> & keyList,
                            const mdtClLinkVersionPkData & versionPk, const mdtClModificationPkData & modificationPk,
                            const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction)
{
  mdtSqlTransaction transaction(database());
  mdtClArticleLink alnk(database());
  bool ok;

  // Init transaction
  if(handleTransaction){
    if(!transaction.begin()){
      pvLastError = transaction.lastError();
      return false;
    }
  }
  for(const auto & key : keyList){
    // Get article link data
    mdtClArticleLinkData articleLinkData = alnk.getLinkData(key.articleLinkPk, ok);
    if(!ok){
      return false;
    }
    // Build link PK
    mdtClLinkPkData pk;
    pk.connectionStartId = key.unitConnectionStartPk.id;
    pk.connectionEndId = key.unitConnectionEndPk.id;
    pk.versionFk = versionPk;
    pk.modificationFk = modificationPk;
    // Build link data
    mdtClLinkData linkData;
    linkData.setPk(pk);
    linkData.setLinkType(articleLinkData.linkType());
    linkData.setLinkDirection(articleLinkData.linkDirection());
    linkData.setArticleLinkFk(articleLinkData.keyData().pk);
    linkData.identification = articleLinkData.indetification;
    if(!articleLinkData.resistance.isNull()){
      linkData.resistance = articleLinkData.resistance.toDouble();
    }
    // Add link ro database
    if(vehicleTypeList.isEmpty()){
      ok = addLink(linkData);
    }else{
      ok = addLink(linkData, vehicleTypeList, false);
    }
    if(!ok){
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
  Q_ASSERT(!pk.isNull());

  mdtClLinkData linkData;
  QList<QSqlRecord> dataList;
  QString sql;

  // Get link data part
  sql = "SELECT * FROM Link_tbl";
  sql += " WHERE UnitConnectionStart_Id_FK = " + pk.connectionStartId.toString();
  sql += " AND UnitConnectionEnd_Id_FK = " + pk.connectionEndId.toString();
  sql += " AND Version_FK = " + QString::number(pk.versionFk.versionPk.value());
  sql += " AND Modification_Code_FK = '" + pk.modificationFk.code + "'";
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

bool mdtClLink::updateLink(const mdtClLinkPkData & linkPk, const mdtClLinkData & linkData)
{
  Q_ASSERT(!linkPk.isNull());
  Q_ASSERT(!linkData.isNull());

  QSqlQuery query(database());
  QString sql;

  sql = "UPDATE Link_tbl\n SET"\
        "  UnitConnectionStart_Id_FK = :UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK = :UnitConnectionEnd_Id_FK, "\
        "  Version_FK = :Version_FK, Modification_Code_FK = :Modification_Code_FK, "\
        "  LinkType_Code_FK = :LinkType_Code_FK, LinkDirection_Code_FK = :LinkDirection_Code_FK, "\
        "  ArticleConnectionStart_Id_FK = :ArticleConnectionStart_Id_FK, ArticleConnectionEnd_Id_FK = :ArticleConnectionEnd_Id_FK, "\
        "  Wire_Id_FK = :Wire_Id_FK, LinkBeam_Id_FK = :LinkBeam_Id_FK, "\
        "  Identification = :Identification, Resistance = :Resistance, Length = :Length, Remarks = :Remarks\n"\
        " WHERE UnitConnectionStart_Id_FK = :PK_UnitConnectionStart_Id_FK"\
        " AND UnitConnectionEnd_Id_FK = :PK_UnitConnectionEnd_Id_FK"\
        " AND Version_FK = :PK_Version_FK"\
        " AND Modification_Code_FK = :PK_Modification_Code_FK";
  query.prepare(sql);
  query.bindValue(":PK_UnitConnectionStart_Id_FK", linkPk.connectionStartId);
  query.bindValue(":PK_UnitConnectionEnd_Id_FK", linkPk.connectionEndId);
  query.bindValue(":PK_Version_FK", linkPk.versionFk.versionPk.value());
  query.bindValue(":PK_Modification_Code_FK", linkPk.modificationFk.code);
  auto pk = linkData.pk();
  query.bindValue(":UnitConnectionStart_Id_FK", pk.connectionStartId);
  query.bindValue(":UnitConnectionEnd_Id_FK", pk.connectionEndId);
  query.bindValue(":Version_FK", pk.versionFk.versionPk.value());
  query.bindValue(":Modification_Code_FK", pk.modificationFk.code);
  auto key = linkData.keyData();
  query.bindValue(":LinkType_Code_FK", key.linkTypeFk().code);
  query.bindValue(":LinkDirection_Code_FK", key.linkDirectionFk().code);
  query.bindValue(":ArticleConnectionStart_Id_FK", key.articleLinkFk().connectionStartId);
  query.bindValue(":ArticleConnectionEnd_Id_FK", key.articleLinkFk().connectionEndId);
  query.bindValue(":Wire_Id_FK", key.wireId());
  query.bindValue(":LinkBeam_Id_FK", key.linkBeamId());
  query.bindValue(":Identification", linkData.identification);
  if(!linkData.resistance.isNull()){
    query.bindValue(":Resistance", linkData.resistance.value());
  }
  if(!linkData.length.isNull()){
    query.bindValue(":Length", linkData.length.value());
  }
  query.bindValue(":Remarks", linkData.remarks);
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot update data in 'Link_tbl'. SQL: ") + query.lastQuery(), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClLink::updateLink(const mdtClLinkPkData & linkPk, const mdtClLinkData & linkData, 
                           const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction)
{
  Q_ASSERT(!linkPk.isNull());
  Q_ASSERT(!linkData.isNull());

  mdtClVehicleTypeLink vtl(database());
  mdtSqlTransaction transaction(database());
  bool pkChanged = false;

  // Check if link PK has changed
  if( (linkData.pk().connectionStartId != linkPk.connectionStartId) || (linkData.pk().connectionEndId != linkPk.connectionEndId) || \
      (linkData.pk().versionFk.versionPk != linkPk.versionFk.versionPk) || (linkData.pk().modificationFk.code != linkPk.modificationFk.code) ){
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
    if(!addLink(linkData, vehicleTypeList, false)){
      return false;
    }
  }else{
    // Update VehicleType_Link_tbl part
    if(!vtl.updateVehicleTypeLink(linkPk, vehicleTypeList, false)){
      pvLastError = vtl.lastError();
      return false;
    }
    // Update Link_tbl part
    if(!updateLink(linkPk, linkData)){
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
  Q_ASSERT(!pk.isNull());

  mdtClVehicleTypeLink vtl(database());
  mdtSqlTransaction transaction(database());
  QSqlQuery query(database());
  QString sql;

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
  // Remove Link_tbl part
  sql = "DELETE FROM Link_tbl"\
        " WHERE UnitConnectionStart_Id_FK = :UnitConnectionStart_Id_FK"\
        " AND UnitConnectionEnd_Id_FK = :UnitConnectionEnd_Id_FK"\
        " AND Version_FK = :Version_FK"\
        " AND Modification_Code_FK = :Modification_Code_FK";
  query.prepare(sql);
  query.bindValue(":UnitConnectionStart_Id_FK", pk.connectionStartId);
  query.bindValue(":UnitConnectionEnd_Id_FK", pk.connectionEndId);
  query.bindValue(":Version_FK", pk.versionFk.versionPk.value());
  query.bindValue(":Modification_Code_FK", pk.modificationFk.code);
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError(tr("Cannot remove data from 'Link_tbl'. SQL: ") + query.lastQuery(), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLink");
    pvLastError.commit();
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
    pk.versionFk.versionPk = s.data(i, "Version_FK").toInt();
    pk.modificationFk.code = s.data(i, "Modification_Code_FK").toString();
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

void mdtClLink::fillRecord(mdtSqlRecord & record, const mdtClLinkData & data)
{
  Q_ASSERT(!data.isNull());
  Q_ASSERT(record.contains("UnitConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("UnitConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("Version_FK"));
  Q_ASSERT(record.contains("Modification_Code_FK"));
  Q_ASSERT(record.contains("LinkType_Code_FK"));
  Q_ASSERT(record.contains("LinkDirection_Code_FK"));
  Q_ASSERT(record.contains("ArticleConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("ArticleConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("Wire_Id_FK"));
  Q_ASSERT(record.contains("Identification"));
  Q_ASSERT(record.contains("Resistance"));
  Q_ASSERT(record.contains("Length"));
  Q_ASSERT(record.contains("Remarks"));

  mdtClLinkPkData pk = data.pk();
  mdtClArticleLinkPkData articleLinkFk = data.keyData().articleLinkFk();

  record.setValue("UnitConnectionStart_Id_FK", pk.connectionStartId);
  record.setValue("UnitConnectionEnd_Id_FK", pk.connectionEndId);
  record.setValue("Version_FK", pk.versionFk.versionPk.value());
  record.setValue("Modification_Code_FK", pk.modificationFk.code);
  record.setValue("LinkType_Code_FK", data.keyData().linkTypeFk().code);
  record.setValue("LinkDirection_Code_FK", data.keyData().linkDirectionFk().code);
  record.setValue("ArticleConnectionStart_Id_FK", articleLinkFk.connectionStartId);
  record.setValue("ArticleConnectionEnd_Id_FK", articleLinkFk.connectionEndId);
  record.setValue("Wire_Id_FK", data.keyData().wireId());
  record.setValue("Identification", data.identification);
  record.setValue("Resistance", data.resistance.value());
  record.setValue("Length", data.length.value());
  record.setValue("Remarks", data.remarks);
}

void mdtClLink::fillData(mdtClLinkData & data, const QSqlRecord & record)
{
  Q_ASSERT(record.contains("UnitConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("UnitConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("Version_FK"));
  Q_ASSERT(record.contains("Modification_Code_FK"));
  Q_ASSERT(record.contains("LinkType_Code_FK"));
  Q_ASSERT(record.contains("LinkDirection_Code_FK"));
  Q_ASSERT(record.contains("ArticleConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("ArticleConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("Wire_Id_FK"));
  Q_ASSERT(record.contains("Identification"));
  Q_ASSERT(record.contains("Resistance"));
  Q_ASSERT(record.contains("Length"));
  Q_ASSERT(record.contains("Remarks"));

  mdtClLinkPkData pk;
  mdtClArticleLinkPkData articleLinkFk;

  pk.connectionStartId = record.value("UnitConnectionStart_Id_FK");
  pk.connectionEndId = record.value("UnitConnectionEnd_Id_FK");
  pk.versionFk.versionPk.setValue(record.value("Version_FK").toInt());
  pk.modificationFk.code = record.value("Modification_Code_FK").toString();
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
  data.remarks = record.value("Remarks");
}
