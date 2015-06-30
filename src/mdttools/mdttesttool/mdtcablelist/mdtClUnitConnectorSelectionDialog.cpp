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
#include "mdtClUnitConnectorSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtClConnectorKeyData.h"
#include "mdtClArticleConnectorKeyData.h"
#include <QStringList>

mdtClUnitConnectorSelectionDialog::mdtClUnitConnectorSelectionDialog(QWidget *parent)
 : mdtSqlSelectionDialog(parent)
{
  resize(700, 300);
  setWindowTitle(tr("Unit connector selection"));
}

bool mdtClUnitConnectorSelectionDialog::select(QSqlDatabase db, const QVariant & unitId)
{
  Q_ASSERT(!unitId.isNull());

  QString sql;

  sql = "SELECT UCNR.* ,"\
        " ACNR.Article_Id_FK, ACNR.Name AS ArticleConnectorName,"\
        " CNR.Gender, CNR.Form, CNR.Manufacturer, CNR.Series, CNR.'Insert', CNR.InsertRotation, CNR.ManufacturerConfigCode, CNR.ManufacturerArticleCode\n"\
        "FROM UnitConnector_tbl UCNR\n"\
        " LEFT JOIN ArticleConnector_tbl ACNR ON ACNR.Id_PK = UCNR.ArticleConnector_Id_FK\n"\
        " LEFT JOIN Connector_tbl CNR ON CNR.Id_PK = UCNR.Connector_Id_FK\n";
  sql += "WHERE UCNR.Unit_Id_FK = " + unitId.toString();
  if(!setQuery(sql, db, false)){
    return false;
  }
  setMessage(tr("Select unit connector to use:"));
  setColumnHidden("Id_PK", true);
  setColumnHidden("Unit_Id_FK", true);
  setColumnHidden("Connector_Id_FK", true);
  setColumnHidden("ArticleConnector_Id_FK", true);
  setColumnHidden("Article_Id_FK", true);
  setHeaderData("Name", tr("Unit\nconnector\nname"));
  setHeaderData("ArticleConnectorName", tr("Article\nconnector\nname"));
//   setHeaderData("ContactQty", tr("Contact\nQty"));
  setHeaderData("InsertRotation", tr("Insert\nRotation"));
  setHeaderData("ManufacturerConfigCode", tr("Manufacturer\nConfiguration code"));
  setHeaderData("ManufacturerArticleCode", tr("Manufacturer\nArticle code"));

  
  addColumnToSortOrder("Name", Qt::AscendingOrder);
  sort();

  return true;
}

mdtClUnitConnectorKeyData mdtClUnitConnectorSelectionDialog::selectedUnitConnectorKey() const
{
  mdtClUnitConnectorKeyData key;
  mdtClConnectorKeyData connectorFk;
  mdtClArticleConnectorKeyData articleConnectorFk;
  QStringList fields;

  if(result() != Accepted){
    return key;
  }
  fields << "Id_PK" << "Unit_Id_FK" << "Connector_Id_FK" << "ArticleConnector_Id_FK" << "Article_Id_FK";
  auto s = selection(fields);
  Q_ASSERT(s.rowCount() == 1);
  // Setup connector FK
  connectorFk.id = s.data(0, "Connector_Id_FK");
  // Setup article connector FK
  articleConnectorFk.id = s.data(0, "ArticleConnector_Id_FK");
  articleConnectorFk.setArticleId(s.data(0, "Article_Id_FK"));
  if(!connectorFk.isNull()){
    articleConnectorFk.setConnectorFk(connectorFk);
  }
  // Setup unit connector FK
  key.pk.id = s.data(0, "Id_PK");
  key.setUnitId(s.data(0, "Unit_Id_FK"));
  if(!articleConnectorFk.isNull()){
    key.setArticleConnectorFk(articleConnectorFk);
  }else{
    if(!connectorFk.isNull()){
      key.setConnectorFk(connectorFk);
    }
  }

  return key;
}

mdtClUnitConnectorData mdtClUnitConnectorSelectionDialog::selectedUnitConnectorData() const
{
  mdtClUnitConnectorData data;

  if(result() != Accepted){
    return data;
  }
  auto s = selection("Name");
  Q_ASSERT(s.rowCount() == 1);
  data.setKeyData(selectedUnitConnectorKey());
  data.name = s.data(0, "Name");

  return data;
}
