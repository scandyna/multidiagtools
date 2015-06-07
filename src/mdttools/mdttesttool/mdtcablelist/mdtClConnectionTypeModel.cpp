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
#include "mdtClConnectionTypeModel.h"
#include <QSqlError>
#include <QModelIndex>

//#include <QDebug>

mdtClConnectionTypeModel::mdtClConnectionTypeModel(QObject *parent, QSqlDatabase db, const QLocale & locale)
 : QSqlQueryModel(parent)
{
  QString sql;
  QString fieldName;

  // Select name field regarding language
  switch(locale.language()){
    case QLocale::French:
      fieldName = "NameFR";
      break;
    case QLocale::German:
      fieldName = "NameDE";
      break;
    case QLocale::Italian:
      fieldName = "NameIT";
      break;
    default:
      fieldName = "NameEN";
  }
  // Get text
  sql = "SELECT Code_PK, " + fieldName + " FROM ConnectionType_tbl ORDER BY " + fieldName + " ASC";
  setQuery(sql, db);
}

mdtClConnectionTypeModel::mdtClConnectionTypeModel(QSqlDatabase db, const QLocale & locale)
: mdtClConnectionTypeModel(nullptr, db, locale)
{
}

int mdtClConnectionTypeModel::row(const mdtClConnectionTypeKeyData & key)
{
  int row;

  if(isInError()){
    return -1;
  }
  for(row = 0; row < rowCount(); ++row){
    QModelIndex idx = index(row, 0);
    if(data(idx) == key.code){
      return row;
    }
  }
  // Update error only if a error occured
  isInError();

  return -1;
}

mdtClConnectionTypeKeyData mdtClConnectionTypeModel::keyData(int row)
{
  mdtClConnectionTypeKeyData key;

  if(row < 0){
    return key;
  }
  if(isInError()){
    return key;
  }
  QModelIndex idx = index(row, 0);
  key.code = data(idx);
  if(key.isNull()){
    QString msg = QString(tr("Could not find connection type for row '%1'.")).arg(row);
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClConnectionTypeModel");
    pvLastError.commit();
  }

  return key;
}

bool mdtClConnectionTypeModel::isInError()
{
  QSqlError sqlError = QSqlQueryModel::lastError();

  if(sqlError.isValid()){
    pvLastError.setError(tr("A error occured on table 'ConnectionType_tbl'."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClConnectionTypeModel");
    pvLastError.commit();
    return true;
  }

  return false;
}
