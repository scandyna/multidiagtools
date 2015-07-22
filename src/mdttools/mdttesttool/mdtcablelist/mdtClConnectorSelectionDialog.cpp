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
#include "mdtClConnectorSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include <QString>

mdtClConnectorSelectionDialog::mdtClConnectorSelectionDialog(QWidget *parent)
 : mdtSqlSelectionDialog(parent)
{
  resize(1000, 400);
  setWindowTitle(tr("Connector selection"));
}

bool mdtClConnectorSelectionDialog::select(QSqlDatabase db)
{
  QString sql;

  sql = "SELECT * FROM Connector_tbl";
  setMessage(tr("Select connector:"));
  if(!setQuery(sql, db, false)){
    return false;
  }
  setColumnHidden("Id_PK", true);
  setHeaderData("ContactQty", tr("Contact\nQty"));
  setHeaderData("InsertRotation", tr("Insert\nRotation"));
  setHeaderData("ManufacturerConfigCode", tr("Manufacturer\nConfiguration code"));
  setHeaderData("ManufacturerArticleCode", tr("Manufacturer\nArticle code"));
  addColumnToSortOrder("Gender", Qt::AscendingOrder);
  addColumnToSortOrder("ContactQty", Qt::AscendingOrder);
  addColumnToSortOrder("ManufacturerConfigCode", Qt::AscendingOrder);
  sort();

  return true;
}

mdtClConnectorPkData mdtClConnectorSelectionDialog::selectedConnectorPk() const
{
  mdtClConnectorPkData pk;

  if(result() != Accepted){
    return pk;
  }
  auto s = selection("Id_PK");
  Q_ASSERT(s.rowCount() == 1);
  pk.id = s.data(0, "Id_PK");

  return pk;
}

QVariant mdtClConnectorSelectionDialog::selectedConnectorId() const
{
  QVariant id;

  if(result() != Accepted){
    return id;
  }
  auto s = selection("Id_PK");
  Q_ASSERT(s.rowCount() == 1);
  id = s.data(0, "Id_PK");

  return id;
}
