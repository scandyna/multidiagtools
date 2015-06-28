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
#include "mdtClArticleConnectionSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include <QStringList>

mdtClArticleConnectionSelectionDialog::mdtClArticleConnectionSelectionDialog(QWidget *parent)
 : mdtSqlSelectionDialog(parent)
{
  resize(700, 400);
  setWindowTitle(tr("Article connection selection"));
}

bool mdtClArticleConnectionSelectionDialog::select(QSqlDatabase db, const QVariant & articleId, ArticleConnectorMembership_t acms, ArticleLinkUsage_t alu, bool allowMultiSelection)
{
  QString sql;

  sql = "SELECT * FROM ArticleConnection_view";
  sql += " WHERE Article_Id_FK = " + articleId.toString();
  sql += acmsClause(acms);
  sql += articleLinkUsageClause(alu, articleId);

  return setQuery(sql, db, allowMultiSelection);
}

bool mdtClArticleConnectionSelectionDialog::select(QSqlDatabase db, const QVariant & articleId, const QVariant & unitId, ArticleConnectorMembership_t acms, bool allowMultiSelection)
{
  QString sql;

  sql = "SELECT * FROM ArticleConnection_view";
  sql += " WHERE Article_Id_FK = " + articleId.toString();
  sql += " AND Id_PK NOT IN ("\
         "  SELECT ArticleConnection_Id_FK"\
         "  FROM UnitConnection_tbl"\
         "  WHERE Unit_Id_FK = " + unitId.toString();
  sql += "  AND ArticleConnection_Id_FK IS NOT NULL)";
  sql += acmsClause(acms);

  return setQuery(sql, db, allowMultiSelection);
}

bool mdtClArticleConnectionSelectionDialog::select(QSqlDatabase db, const mdtClArticleConnectorKeyData & articleConnectorKey, const QVariant & unitId, bool allowMultiSelection)
{
  QString sql;

  sql = "SELECT * FROM ArticleConnection_view";
  sql += " WHERE ArticleConnector_Id_FK = " + articleConnectorKey.id.toString();
  sql += " AND Id_PK NOT IN ("\
         "  SELECT ArticleConnection_Id_FK"\
         "  FROM UnitConnection_tbl"\
         "  WHERE Unit_Id_FK = " + unitId.toString();
  sql += "  AND ArticleConnection_Id_FK IS NOT NULL)";

  return setQuery(sql, db, allowMultiSelection);
}

bool mdtClArticleConnectionSelectionDialog::setQuery(const QString & sql, QSqlDatabase & db, bool allowMultiSelection)
{
  if(!mdtSqlSelectionDialog::setQuery(sql, db, allowMultiSelection)){
    return false;
  }
  setMessage(tr("Select article connection to use:"));
  setColumnHidden("Id_PK", true);
  setColumnHidden("Article_Id_FK", true);
  setColumnHidden("Connector_Id_FK", true);
  setColumnHidden("ArticleConnector_Id_FK", true);
  setHeaderData("ArticleConnectorName", tr("Article\nconnector\nname"));
  setHeaderData("ArticleContactName", tr("Article\ncontact\nname"));
  addColumnToSortOrder("ArticleConnectorName", Qt::AscendingOrder);
  addColumnToSortOrder("ArticleContactName", Qt::AscendingOrder);
  sort();

  return true;
}

mdtClArticleConnectionKeyData mdtClArticleConnectionSelectionDialog::selectedArticleConnectionKey() const
{
  mdtClArticleConnectionKeyData key;
  QList<mdtClArticleConnectionKeyData> keyList;

  if(result() != Accepted){
    return key;
  }
  keyList = selectedArticleConnectionKeyList();
  if(keyList.isEmpty()){
    return key;
  }
  Q_ASSERT(keyList.size() == 1);
  key = keyList.at(0);

  return key;
}

QList<mdtClArticleConnectionKeyData> mdtClArticleConnectionSelectionDialog::selectedArticleConnectionKeyList() const
{
  QList<mdtClArticleConnectionKeyData> keyList;
  QStringList fields;

  if(result() != Accepted){
    return keyList;
  }
  fields << "Id_PK" << "Article_Id_FK" << "ArticleConnector_Id_FK" << "ConnectionType_Code_FK" << "Connector_Id_FK";
  auto s = selection(fields);
  for(int row = 0; row < s.rowCount(); ++row){
    keyList.append(buildKeyData(s, row));
  }

  return keyList;
}

mdtClArticleConnectionData mdtClArticleConnectionSelectionDialog::selectedArticleConnectionData() const
{
  mdtClArticleConnectionData data;
  QList<mdtClArticleConnectionData> dataList;

  if(result() != Accepted){
    return data;
  }
  dataList = selectedArticleConnectionDataList();
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);

  return data;
}

QList<mdtClArticleConnectionData> mdtClArticleConnectionSelectionDialog::selectedArticleConnectionDataList() const
{
  QList<mdtClArticleConnectionData> dataList;
  QStringList fields;

  if(result() != Accepted){
    return dataList;
  }
  fields << "Id_PK" << "Article_Id_FK" << "ArticleConnector_Id_FK" << "ConnectionType_Code_FK" << "Connector_Id_FK"\
         << "ArticleContactName" << "IoType" << "FunctionEN" << "FunctionFR" << "FunctionDE" << "FunctionIT";
  auto s = selection(fields);
  for(int row = 0; row < s.rowCount(); ++row){
    auto key = buildKeyData(s, row);
    mdtClArticleConnectionData data;
    data.setKeyData(key);
    data.name = s.data(row, "ArticleContactName");
    data.ioType = s.data(row, "IoType");
    data.functionEN = s.data(row, "FunctionEN");
    data.functionFR = s.data(row, "FunctionFR");
    data.functionDE = s.data(row, "FunctionDE");
    data.functionIT = s.data(row, "FunctionIT");
    dataList.append(data);
  }

  return dataList;
}

QString mdtClArticleConnectionSelectionDialog::acmsClause(ArticleConnectorMembership_t acms) const
{
  switch(acms){
    case ArticleConnectorMembership_t::PartOfArticleConnector:
      return "AND ArticleConnector_Id_FK IS NOT NULL ";
    case ArticleConnectorMembership_t::NotPartOfArticleConnector:
      return "AND ArticleConnector_Id_FK IS NULL ";
    case ArticleConnectorMembership_t::All:
      return "";
  }
  return "";
}

QString mdtClArticleConnectionSelectionDialog::articleLinkUsageClause(ArticleLinkUsage_t alu, const QVariant & articleId) const
{
  QString clause;
  QString cnxField, articleField;

  switch(alu){
    case ArticleLinkUsage_t::All:
      return clause;
    case ArticleLinkUsage_t::NotUsedAsConnectionStart:
      cnxField = "ArticleConnectionStart_Id_FK";
      articleField = "StartArticle_Id_FK";
      break;
    case ArticleLinkUsage_t::NotUsedAsConnectionEnd:
      cnxField = "ArticleConnectionEnd_Id_FK";
      articleField = "EndArticle_Id_FK";
      break;
  }
  clause = " AND Id_PK NOT IN (";
  clause += " SELECT " + cnxField + " FROM ArticleLink_view WHERE " + articleField + " = " + articleId.toString();
  clause += " AND " + cnxField + " IS NOT NULL)";

  return clause;
}

mdtClArticleConnectionKeyData mdtClArticleConnectionSelectionDialog::buildKeyData(const mdtSqlTableSelection &s, int row) const
{
  Q_ASSERT( (row >= 0) && (row < s.rowCount()) );

  mdtClArticleConnectionKeyData key;
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClConnectorKeyData connectorFk;

  key.id = s.data(row, "Id_PK");
  key.setArticleId(s.data(row, "Article_Id_FK"));
  articleConnectorFk.id = s.data(row, "ArticleConnector_Id_FK");
  articleConnectorFk.setArticleId(key.articleId());
  connectorFk.id = s.data(row, "Connector_Id_FK");
  articleConnectorFk.setConnectorFk(connectorFk);
  if(!articleConnectorFk.isNull()){
    key.setArticleConnectorFk(articleConnectorFk);
  }
  key.setConnectionTypeCode(s.data(row, "ConnectionType_Code_FK"));

  return key;
}

// QString mdtClArticleConnectionSelectionDialog::tableNameToUse(mdtClArticleConnectionSelectionDialog::ArticleConnectorMembership_t acms) const
// {
//   switch(acms){
//     case PartOfArticleConnector:
//     case All:
//       return "ArticleConnection_view";
//     case NotPartOfArticleConnector:
//       return "ArticleConnection_tbl";
//   }
// }
