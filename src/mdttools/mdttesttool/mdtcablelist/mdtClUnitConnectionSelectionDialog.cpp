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
#include "mdtClUnitConnectionSelectionDialog.h"
#include "mdtClConnectorKeyData.h"
#include "mdtClUnitConnectorKeyData.h"
#include "mdtClArticleConnectorKeyData.h"
#include "mdtClArticleConnectionKeyData.h"

mdtClUnitConnectionSelectionDialog::mdtClUnitConnectionSelectionDialog(QWidget *parent, const QLocale & locale)
 : mdtSqlSelectionDialog(parent),
   pvLocale(locale)
{
  resize(700, 400);
  setWindowTitle(tr("Unit connection selection"));
}

bool mdtClUnitConnectionSelectionDialog::select(QSqlDatabase db, const QVariant & unitId, mdtClUnitConnectionSelectionDialog::LinkUsage_t lu, bool allowMultiSelection)
{
  QString sql;

  return select(db, unitId, QList<mdtClUnitConnectorPkData>(), lu, allowMultiSelection);
}

bool mdtClUnitConnectionSelectionDialog::select(QSqlDatabase db, const QVariant & unitId, QList<mdtClUnitConnectorPkData> ucnrPkList, mdtClUnitConnectionSelectionDialog::LinkUsage_t lu, bool allowMultiSelection)
{
  QString sql;
  int lastIndex;
  int i;

  sql = baseSqlStatement();
  sql += " WHERE Unit_Id_FK = " + unitId.toString() +  " ";
  sql += linkUsageClause(lu, unitId);
  if(!ucnrPkList.isEmpty()){
    sql += " AND UnitConnector_Id_FK IN(";
    lastIndex = ucnrPkList.size() - 1;
    for(i = 0; i < lastIndex; ++i){
      sql += ucnrPkList.at(i).id.toString();
    }
    sql += ucnrPkList.at(lastIndex).id.toString() + ")";
  }

  return setQuery(sql, db, allowMultiSelection);
}

mdtClUnitConnectionPkData mdtClUnitConnectionSelectionDialog::selectedUnitConnectionPk() const
{
  mdtClUnitConnectionPkData pk;

  if(result() != Accepted){
    return pk;
  }
  auto s = selection("Id_PK");
  if(s.isEmpty()){
    return pk;
  }
  Q_ASSERT(s.rowCount() == 1);
  pk.id = s.data(0, "Id_PK");

  return pk;
}

QList<mdtClUnitConnectionPkData> mdtClUnitConnectionSelectionDialog::selectedUnitConnectionPkList() const
{
  QList<mdtClUnitConnectionPkData> pkList;

  if(result() != Accepted){
    return pkList;
  }
  auto s = selection("Id_PK");
  for(int row = 0; row < s.rowCount(); ++row){
    mdtClUnitConnectionPkData pk;
    pk.id = s.data(row, "Id_PK");
    pkList.append(pk);
  }

  return pkList;
}

mdtClUnitConnectionKeyData mdtClUnitConnectionSelectionDialog::selectedUnitConnectionKey() const
{
  mdtClUnitConnectionKeyData key;
  QList<mdtClUnitConnectionKeyData> keyList;

  if(result() != Accepted){
    return key;
  }
  keyList = selectedUnitConnectionKeyList();
  if(keyList.isEmpty()){
    return key;
  }
  Q_ASSERT(keyList.size() == 1);
  key = keyList.at(0);

  return key;
}

QList<mdtClUnitConnectionKeyData> mdtClUnitConnectionSelectionDialog::selectedUnitConnectionKeyList() const
{
  QList<mdtClUnitConnectionKeyData> keyList;
  QStringList fields;

  if(result() != Accepted){
    return keyList;
  }
  fields << "Id_PK" << "Unit_Id_FK" << "ACNR_Connector_Id_FK" << "ACNR_Article_Id_FK" << "Article_Id_FK" << "ArticleConnector_Id_FK" \
         << "ConnectionType_Code_FK" << "UnitConnector_Id_FK" << "UCNR_Unit_Id_FK" << "UCNR_Connector_Id_FK" \
         << "ArticleConnection_Id_FK" << "ACNX_ConnectionType_Code_FK";
  auto s = selection(fields);
  for(int row = 0; row < s.rowCount(); ++row){
    keyList.append(buildKeyData(s, row));
  }

  return keyList;
}

bool mdtClUnitConnectionSelectionDialog::setQuery(const QString & sql, QSqlDatabase & db, bool allowMultiSelection)
{
  if(!mdtSqlSelectionDialog::setQuery(sql, db, allowMultiSelection)){
    return false;
  }
  setMessage(tr("Select unit connection to use:"));
  setColumnHidden("Id_PK", true);
  setColumnHidden("Unit_Id_FK", true);
  setColumnHidden("UnitConnector_Id_FK", true);
  setColumnHidden("ConnectionType_Code_FK", true);
  setColumnHidden("ArticleConnection_Id_FK", true);
  setColumnHidden("UCNR_Unit_Id_FK", true);
  setColumnHidden("UCNR_Connector_Id_FK", true);
  setColumnHidden("ACNR_Connector_Id_FK", true);
  setColumnHidden("ACNR_Article_Id_FK", true);
  setColumnHidden("Article_Id_FK", true);
  setColumnHidden("ArticleConnector_Id_FK", true);
  setColumnHidden("ACNX_ConnectionType_Code_FK", true);
  setHeaderData("UnitConnectorName", tr("Unit\nconnector"));
  setHeaderData("UnitContactName", tr("Unit\ncontact"));
  setHeaderData("ConnectionType", tr("Type"));
  setHeaderData("UnitConnectionResistance", tr("R [Ohm]"));
  setHeaderData("SchemaPage", tr("Schema\npage"));
  setHeaderData("UnitFunction", tr("Unit\nfunction"));
  setHeaderData("SignalName", tr("Signal\nname"));
  setHeaderData("SwAddress", tr("SW address"));
  addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  addColumnToSortOrder("UnitContactName", Qt::AscendingOrder);
  sort();

  return true;
}

QString mdtClUnitConnectionSelectionDialog::baseSqlStatement() const
{
  QString sql;
  QString cnxTypeField;
  QString unitFunctionField;
  QString articleFunctionField;

  // Choose some fields regarding locale
  switch(pvLocale.language()){
    case QLocale::French:
      cnxTypeField = "ConnectionTypeFR";
      unitFunctionField = "UnitFunctionFR";
      articleFunctionField = "ArticleFunctionFR";
      break;
    case QLocale::German:
      cnxTypeField = "ConnectionTypeDE";
      unitFunctionField = "UnitFunctionDE";
      articleFunctionField = "ArticleFunctionDE";
      break;
    case QLocale::Italian:
      cnxTypeField = "ConnectionTypeIT";
      unitFunctionField = "UnitFunctionIT";
      articleFunctionField = "ArticleFunctionIT";
      break;
    default:
      cnxTypeField = "ConnectionTypeEN";
      unitFunctionField = "UnitFunctionEN";
      articleFunctionField = "ArticleFunctionEN";
  }
  // Build SQL statement
  sql = "SELECT Id_PK, Unit_Id_FK, UnitConnector_Id_FK, UCNR_Unit_Id_FK, UCNR_Connector_Id_FK, ConnectionType_Code_FK, ArticleConnection_Id_FK,"\
        " ACNR_Connector_Id_FK, ArticleConnector_Id_FK, ACNR_Article_Id_FK, Article_Id_FK, ACNX_ConnectionType_Code_FK, "\
        " UnitConnectorName, UnitContactName, " + cnxTypeField + " AS ConnectionType, UnitConnectionResistance, SchemaPage,"\
        + unitFunctionField + " AS UnitFunction, SignalName, SwAddress,"\
        " ArticleConnectorName, ArticleContactName, IoType, " + articleFunctionField + " AS ArticleFunction"\
        " FROM UnitConnection_view";

  return sql;
}

QString mdtClUnitConnectionSelectionDialog::linkUsageClause(mdtClUnitConnectionSelectionDialog::LinkUsage_t lu, const QVariant &unitId) const
{
  QString clause;

  switch(lu){
    case LinkUsage_t::All:
      return clause;
    case LinkUsage_t::NotUsedAsConnectionStart:
      clause = "AND Id_PK NOT IN (SELECT DISTINCT UnitConnectionStart_Id_FK FROM UnitLink_view WHERE StartUnit_Id_FK = " + unitId.toString() \
             + " AND UnitConnectionStart_Id_FK IS NOT NULL)";
      break;
    case LinkUsage_t::NotUsedAsConnectionEnd:
      clause = "AND Id_PK NOT IN (SELECT DISTINCT UnitConnectionEnd_Id_FK FROM UnitLink_view WHERE EndUnit_Id_FK = " + unitId.toString() \
             + " AND UnitConnectionEnd_Id_FK IS NOT NULL)";
      break;
    case LinkUsage_t::NotUsed:
      clause = "AND Id_PK NOT IN (SELECT DISTINCT UnitConnectionStart_Id_FK FROM UnitLink_view WHERE StartUnit_Id_FK = " + unitId.toString() \
             + " AND UnitConnectionStart_Id_FK IS NOT NULL)" \
             + "AND Id_PK NOT IN (SELECT DISTINCT UnitConnectionEnd_Id_FK FROM UnitLink_view WHERE EndUnit_Id_FK = " + unitId.toString() \
             + " AND UnitConnectionStart_Id_FK IS NOT NULL)";
      break;
  }

  return clause;
}

mdtClUnitConnectionKeyData mdtClUnitConnectionSelectionDialog::buildKeyData(const mdtSqlTableSelection &s, int row) const
{
  Q_ASSERT( (row >= 0) && (row < s.rowCount()) );

  mdtClUnitConnectionKeyData key;
  mdtClConnectorKeyData connectorFk;
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClUnitConnectorKeyData unitConnectorFk;
  mdtClArticleConnectionKeyData articleConnectionFk;

  // Setup article connector FK
  connectorFk.id = s.data(row, "ACNR_Connector_Id_FK");
  articleConnectorFk.id = s.data(row, "ArticleConnector_Id_FK");
  articleConnectorFk.setArticleId(s.data(row, "ACNR_Article_Id_FK"));
  if( (!articleConnectorFk.isNull())&&(!connectorFk.isNull()) ){
    articleConnectorFk.setConnectorFk(connectorFk);
  }
  // Setup unit connector FK
  unitConnectorFk.pk.id = s.data(row, "UnitConnector_Id_FK");
  unitConnectorFk.setUnitId(s.data(row, "UCNR_Unit_Id_FK"));
  if(!articleConnectorFk.isNull()){
    unitConnectorFk.setArticleConnectorFk(articleConnectorFk);
  }else{
    connectorFk.id = s.data(row, "UCNR_Connector_Id_FK");
    if(!connectorFk.isNull()){
      unitConnectorFk.setConnectorFk(connectorFk);
    }
  }
  // Setup article connection FK
  articleConnectionFk.id = s.data(row, "ArticleConnection_Id_FK");
  articleConnectionFk.setArticleId(s.data(row, "Article_Id_FK"));
  if(!articleConnectorFk.isNull()){
    articleConnectionFk.setArticleConnectorFk(articleConnectorFk);
  }
  articleConnectionFk.setConnectionTypeCode(s.data(row, "ACNX_ConnectionType_Code_FK"));
  // Setup unit connection key
  key.pk.id = s.data(row, "Id_PK");
  key.setUnitId(s.data(row, "Unit_Id_FK"));
  if(!unitConnectorFk.isNull()){
    key.setUnitConnectorFk(unitConnectorFk);
  }
  if(articleConnectionFk.isNull()){
    key.setConnectionTypeCode(s.data(row, "ConnectionType_Code_FK"));
  }else{
    key.setArticleConnectionFk(articleConnectionFk);
  }

  return key;
}
