/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "mdtClLinkVersionModel.h"
#include <QSqlError>
#include <QModelIndex>
#include <QComboBox>

//#include <QDebug>

mdtClLinkVersionModel::mdtClLinkVersionModel(QObject *parent, QSqlDatabase db)
 : QSqlQueryModel(parent)
{
  QString sql;

  sql = "SELECT Version_PK, Version FROM LinkVersion_tbl ORDER BY Version_PK ASC";
  setQuery(sql, db);
}

mdtClLinkVersionModel::mdtClLinkVersionModel(QSqlDatabase db)
 : mdtClLinkVersionModel(nullptr, db)
{
}

int mdtClLinkVersionModel::row(const LinkVersionPk & key)
{
  int row;

  for(row = 0; row < rowCount(); ++row){
    QModelIndex idx = index(row, 0);
    if(data(idx) == key.version()){
      return row;
    }
  }
  // Update error only if a error occured
  isInError();

  return -1;
}

LinkVersionPk mdtClLinkVersionModel::versionPk(int row)
{
  LinkVersionPk pk;
  QVariant var;

  if(row < 0){
    return pk;
  }
  if(isInError()){
    return pk;
  }
  QModelIndex idx = index(row, 0);
  var = data(idx);
  if(var.isNull()){
    QString msg = QString(tr("Could not find version for row '%1'.")).arg(row);
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLinkVersionModel");
    pvLastError.commit();
  }else{
    pk = LinkVersionPk::fromQVariant(var);
  }

  return pk;
}

LinkVersionPk mdtClLinkVersionModel::currentVersionPk(QComboBox *cb)
{
  Q_ASSERT(cb != nullptr);

  return versionPk(cb->currentIndex());
}

bool mdtClLinkVersionModel::isInError()
{
  QSqlError sqlError = QSqlQueryModel::lastError();

  if(sqlError.isValid()){
    pvLastError.setError(tr("A error occured on table 'LinkVersion_tbl'."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClLinkVersionModel");
    pvLastError.commit();
    return true;
  }

  return false;
}
