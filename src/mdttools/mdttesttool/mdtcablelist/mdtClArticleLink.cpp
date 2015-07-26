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
#include "mdtClArticleLink.h"
#include "mdtSqlTransaction.h"

//#include <QDebug>

mdtClArticleLink::mdtClArticleLink(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtClArticleLink::mdtClArticleLink(QSqlDatabase db)
 : mdtClArticleLink(nullptr, db)
{
}

bool mdtClArticleLink::addLink(const mdtClArticleLinkData &data)
{
  mdtSqlRecord record;

  if(!record.addAllFields("ArticleLink_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  fillRecord(record, data);

  return addRecord(record, "ArticleLink_tbl");
}

mdtClArticleLinkData mdtClArticleLink::getLinkData(const mdtClArticleLinkPkData & key, bool & ok)
{
  mdtClArticleLinkData data;
  QList<QSqlRecord> dataList;
  QString sql;

  sql = "SELECT * FROM ArticleLink_tbl";
  sql += " WHERE ArticleConnectionStart_Id_FK = " + key.connectionStartId.toString();
  sql += " AND ArticleConnectionEnd_Id_FK = " + key.connectionEndId.toString();
  dataList = getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  fillData(data, dataList.at(0));

  return data;
}

// QList<mdtClArticleLinkData> mdtClArticleLink::getLinkDataList(const mdtClUnitConnectionPkData & ucnxPk, const QVariant & unitId, bool & ok)
// {
//   Q_ASSERT(!ucnxPk.isNull());
//   Q_ASSERT(!unitId.isNull());
// 
//   QList<mdtClArticleLinkData> linkDataList;
//   QList<QSqlRecord> dataList;
//   QString sql;
// 
// //   sql = "SELECT UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK, ArticleConnectionStart_Id_FK, ArticleConnectionEnd_Id_FK,";
// //   sql += " LinkType_Code_FK, LinkDirection_Code_FK, Identification, Resistance";
// //   sql += " FROM ArticleLink_UnitConnection_view ";
// //   sql += " WHERE ( UnitConnectionStart_Id_FK = " + ucnxPk.id.toString();
// //   sql += " OR UnitConnectionEnd_Id_FK = " + ucnxPk.id.toString();
// //   sql += " ) AND ( StartUnit_Id_FK = " + unitId.toString();
// //   sql += " AND EndUnit_Id_FK = " + unitId.toString();
// //   sql += " )";
//   
//   sql = "SELECT ALNK.*\n"\
//         " FROM ArticleLink_tbl ALNK\n"\
//         " JOIN UnitConnection_tbl UCNXS\n"\
//         "  ON UCNXS.ArticleConnection_Id_FK = ALNK.ArticleConnectionStart_Id_FK\n"\
//         " JOIN UnitConnection_tbl UCNXE\n"\
//         "  ON UCNXE.ArticleConnection_Id_FK = ALNK.ArticleConnectionEnd_Id_FK\n"\
//         " WHERE ( UCNXS.Id_PK = " + ucnxPk.id.toString() + \
//         " OR UCNXE.Id_PK = " + ucnxPk.id.toString() + \
//         " ) AND ( UCNXS.Unit_Id_FK = " + unitId.toString() + \
//         " AND UCNXE.Unit_Id_FK = " + unitId.toString() + \
//         " )";
//   dataList = getDataList<QSqlRecord>(sql, ok);
//   if(!ok){
//     return linkDataList;
//   }
//   for(const auto & record : dataList){
//     mdtClArticleLinkData data;
//     fillData(data, record);
//     linkDataList.append(data);
//   }
// 
//   return linkDataList;
// }

// int mdtClArticleLink::relatedLinksToCreateCount(const mdtClUnitConnectionPkData & ucnxPk, const QVariant & unitId)
// {
//   Q_ASSERT(!ucnxPk.isNull());
//   Q_ASSERT(!unitId.isNull());
// 
//   int n = 0;
//   QString sql;
//   bool ok;
// 
//   sql = "SELECT COUNT(*)\n"\
//         " FROM ArticleLink_tbl ALNK\n"\
//         " JOIN UnitConnection_tbl UCNXS\n"\
//         "  ON UCNXS.ArticleConnection_Id_FK = ALNK.ArticleConnectionStart_Id_FK\n"\
//         " JOIN UnitConnection_tbl UCNXE\n"\
//         "  ON UCNXE.ArticleConnection_Id_FK = ALNK.ArticleConnectionEnd_Id_FK\n"\
//         " WHERE ( UCNXS.Id_PK = " + ucnxPk.id.toString() + \
//         " OR UCNXE.Id_PK = " + ucnxPk.id.toString() + \
//         " ) AND ( UCNXS.Unit_Id_FK = " + unitId.toString() + \
//         " AND UCNXE.Unit_Id_FK = " + unitId.toString() + \
//         " )";
//   n = getData<int>(sql, ok);
//   if(!ok){
//     return -1;
//   }
// 
//   return n;
// }

// bool mdtClArticleLink::hasRelatedLinksToCreate(const mdtClUnitConnectionPkData & ucnxPk, const QVariant & unitId, bool & ok)
// {
//   Q_ASSERT(!ucnxPk.isNull());
//   Q_ASSERT(!unitId.isNull());
// 
//   int n = relatedLinksToCreateCount(ucnxPk, unitId);
//   if(n < 0){
//     ok = false;
//     return false;
//   }
//   ok = true;
// 
//   return (n > 0);
// }

int mdtClArticleLink::relatedLinksCount(const mdtClArticleLinkPkData & key)
{
  QString sql;
  QList<QVariant> dataList;
  bool ok;

  sql = "SELECT COUNT(*) FROM Link_tbl WHERE ArticleConnectionStart_Id_FK = " + key.connectionStartId.toString() + " AND ArticleConnectionEnd_Id_FK = " + key.connectionEndId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return -1;
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0).toInt();
}

bool mdtClArticleLink::updateLink(const mdtClArticleLinkPkData & key, const mdtClArticleLinkData & data)
{
  mdtSqlRecord record;

  if(!record.addAllFields("ArticleLink_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  fillRecord(record, data);

  return updateRecord("ArticleLink_tbl", record, "ArticleConnectionStart_Id_FK", key.connectionStartId, "ArticleConnectionEnd_Id_FK", key.connectionEndId);
}

bool mdtClArticleLink::removeLink(const mdtClArticleLinkPkData & key)
{
  return removeData("ArticleLink_tbl", "ArticleConnectionStart_Id_FK", key.connectionStartId, "ArticleConnectionEnd_Id_FK", key.connectionEndId);
}

bool mdtClArticleLink::removeLinks(const mdtSqlTableSelection & s)
{
  mdtSqlTransaction transaction(database());
  int row;

  if(!transaction.begin()){
    pvLastError = transaction.lastError();
    return false;
  }
  for(row = 0; row < s.rowCount(); ++row){
    mdtClArticleLinkPkData key;
    key.connectionStartId = s.data(row, "ArticleConnectionStart_Id_FK");
    key.connectionEndId = s.data(row, "ArticleConnectionEnd_Id_FK");
    if(!removeLink(key)){
      return false;
    }
  }
  if(!transaction.commit()){
    pvLastError = transaction.lastError();
    return false;
  }

  return true;
}

void mdtClArticleLink::fillRecord(mdtSqlRecord &record, const mdtClArticleLinkData &data)
{
  Q_ASSERT(!record.isEmpty());

  auto key = data.keyData();

  record.clearValues();
  record.setValue("ArticleConnectionStart_Id_FK", key.pk.connectionStartId);
  record.setValue("ArticleConnectionEnd_Id_FK", key.pk.connectionEndId);
  record.setValue("LinkType_Code_FK", key.linkTypeFk().code);
  record.setValue("LinkDirection_Code_FK", key.linkDirectionFk().code);
  record.setValue("Identification", data.indetification);
//   record.setValue("SinceVersion", data.sinceVersion);
//   record.setValue("Modification", data.modification);
  record.setValue("Resistance", data.resistance);
}

void mdtClArticleLink::fillData(mdtClArticleLinkData & data, const QSqlRecord & record)
{
  Q_ASSERT(record.contains("ArticleConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("ArticleConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("LinkType_Code_FK"));
  Q_ASSERT(record.contains("LinkDirection_Code_FK"));
  Q_ASSERT(record.contains("Identification"));
//   Q_ASSERT(record.contains("SinceVersion"));
//   Q_ASSERT(record.contains("Modification"));
  Q_ASSERT(record.contains("Resistance"));

  mdtClArticleLinkKeyData key;
  // Fill key
  key.pk.connectionStartId = record.value("ArticleConnectionStart_Id_FK");
  key.pk.connectionEndId = record.value("ArticleConnectionEnd_Id_FK");
  key.setLinkTypeCode(record.value("LinkType_Code_FK"));
  key.setLinkDirectionCode(record.value("LinkDirection_Code_FK"));
  // Fill data
  data.setKeyData(key);
  data.indetification = record.value("Identification");
//   data.sinceVersion = record.value("SinceVersion");
//   data.modification = record.value("Modification");
  data.resistance = record.value("Resistance");
}
