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
#include "mdtClArticleSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include <QString>

mdtClArticleSelectionDialog::mdtClArticleSelectionDialog(QWidget *parent, QSqlDatabase db)
 : mdtSqlSelectionDialog(parent)
{
  QString sql;

  sql = "SELECT * FROM Article_tbl";
  setMessage(tr("Select article:"));
  setQuery(sql, db, false);
  setColumnHidden("Id_PK", true);
  setHeaderData("ArticleCode", tr("Article code\n(internal)"));
  ///addColumnToSortOrder("ArticleCode", Qt::AscendingOrder);
  addColumnToSortOrder("DesignationEN", Qt::AscendingOrder);
  sort();
  resize(800, 400);
  setWindowTitle(tr("Article selection"));
}

QVariant mdtClArticleSelectionDialog::selectedArticleId() const
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
