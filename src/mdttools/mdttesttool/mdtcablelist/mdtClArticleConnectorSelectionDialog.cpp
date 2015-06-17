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
#include "mdtClArticleConnectorSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include <QStringList>

mdtClArticleConnectorSelectionDialog::mdtClArticleConnectorSelectionDialog(QWidget *parent)
 : mdtSqlSelectionDialog(parent)
{
  resize(700, 400);
  setWindowTitle(tr("Article connector selection"));
}

bool mdtClArticleConnectorSelectionDialog::select(QSqlDatabase db, const QVariant & articleId)
{
  QString sql;

  sql = "SELECT * FROM ArticleConnector_tbl WHERE Article_Id_FK = " + articleId.toString();
  if(!setQuery(sql, db, false)){
    return false;
  }
  setMessage(tr("Select article connector to use:"));
  setColumnHidden("Id_PK", true);
  setColumnHidden("Article_Id_FK", true);
  setColumnHidden("Connector_Id_FK", true);
  addColumnToSortOrder("Name", Qt::AscendingOrder);
  sort();

  return true;
}

bool mdtClArticleConnectorSelectionDialog::select(QSqlDatabase db, const QVariant & articleId, const QVariant & unitId)
{
  QString sql;

  sql = "SELECT * FROM ArticleConnector_tbl WHERE Article_Id_FK = " + articleId.toString();
  sql += " AND Id_PK NOT IN ("\
         "  SELECT ArticleConnector_Id_FK"\
         "  FROM UnitConnector_tbl"\
         "  WHERE Unit_Id_FK = " + unitId.toString();
  sql += "  AND ArticleConnector_Id_FK IS NOT NULL)";
  if(!setQuery(sql, db, false)){
    return false;
  }
  setMessage(tr("Select article connector to use:"));
  setColumnHidden("Id_PK", true);
  setColumnHidden("Article_Id_FK", true);
  setColumnHidden("Connector_Id_FK", true);
  addColumnToSortOrder("Name", Qt::AscendingOrder);
  sort();

  return true;
}

mdtClArticleConnectorKeyData mdtClArticleConnectorSelectionDialog::selectedArticleConnectorKey() const
{
  mdtClArticleConnectorKeyData key;
  mdtClConnectorKeyData connectorFk;
  QStringList fields;

  if(result() != Accepted){
    return key;
  }
  fields << "Id_PK" << "Article_Id_FK" << "Connector_Id_FK";
  auto s = selection(fields);
  Q_ASSERT(s.rowCount() == 1);
  key.id = s.data(0, "Id_PK");
  key.setArticleId(s.data(0, "Article_Id_FK"));
  connectorFk.id = s.data(0, "Connector_Id_FK");
  key.setConnectorFk(connectorFk);

  return key;
}

QVariant mdtClArticleConnectorSelectionDialog::selectedArticleConnectorName() const
{
  if(result() != Accepted){
    return QVariant();
  }
  auto s = selection("Name");
  Q_ASSERT(s.rowCount() == 1);
  return s.data(0, "Name");
}
