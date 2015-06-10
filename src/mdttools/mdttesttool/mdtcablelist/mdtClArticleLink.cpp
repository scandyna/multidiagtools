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

int mdtClArticleLink::relatedLinksCount(const mdtClArticleLinkPkData & pk)
{
  QString sql;
  QList<QVariant> dataList;
  bool ok;

  sql = "SELECT COUNT(*) FROM Link_tbl WHERE ArticleConnectionStart_Id_FK = " + pk.connectionStartId.toString() + " AND ArticleConnectionEnd_Id_FK = " + pk.connectionEndId.toString();
  dataList = getDataList<QVariant>(sql, ok);
  if(!ok){
    return -1;
  }
  Q_ASSERT(dataList.size() == 1);

  return dataList.at(0).toInt();
}

void mdtClArticleLink::fillRecord(mdtSqlRecord &record, const mdtClArticleLinkData &data)
{
  Q_ASSERT(!record.isEmpty());

  auto key = data.keyData();

  record.clearValues();
  record.setValue("ArticleConnectionStart_Id_FK", key.pk.connectionStartId);
  record.setValue("ArticleConnectionEnd_Id_FK", key.pk.connectionEndId);
  record.setValue("LinkType_Code_FK", key.linkTypeFk.code);
  record.setValue("LinkDirection_Code_FK", key.linkDirectionFk.code);
  record.setValue("Identification", data.indetification);
  record.setValue("SinceVersion", data.sinceVersion);
  record.setValue("Modification", data.modification);
  record.setValue("Resistance", data.resistance);
}

void mdtClArticleLink::fillData(mdtClArticleLinkData & data, const QSqlRecord & record)
{
  Q_ASSERT(record.contains("ArticleConnectionStart_Id_FK"));
  Q_ASSERT(record.contains("ArticleConnectionEnd_Id_FK"));
  Q_ASSERT(record.contains("LinkType_Code_FK"));
  Q_ASSERT(record.contains("LinkDirection_Code_FK"));
  Q_ASSERT(record.contains("Identification"));
  Q_ASSERT(record.contains("SinceVersion"));
  Q_ASSERT(record.contains("Modification"));
  Q_ASSERT(record.contains("Resistance"));

  mdtClArticleLinkKeyData key;
  // Fill key
  key.pk.connectionStartId = record.value("ArticleConnectionStart_Id_FK");
  key.pk.connectionEndId = record.value("ArticleConnectionEnd_Id_FK");
  key.linkTypeFk.code = record.value("LinkType_Code_FK");
  key.linkDirectionFk.code = record.value("LinkDirection_Code_FK");
  // Fill data
  data.setKeyData(key);
  data.indetification = record.value("Identification");
  data.sinceVersion = record.value("SinceVersion");
  data.modification = record.value("Modification");
  data.resistance = record.value("Resistance");
}
