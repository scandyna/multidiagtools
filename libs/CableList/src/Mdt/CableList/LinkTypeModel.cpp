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
#include "LinkTypeModel.h"
#include "mdt/sql/error/Error.h"
#include <QSqlError>
#include <QModelIndex>
#include <QComboBox>
#include <QSqlQuery>

namespace Mdt{ namespace CableList{

LinkTypeModel::LinkTypeModel(QObject *parent, const QSqlDatabase & db)
 : QSqlQueryModel(parent),
   pvDatabase(db)
{
  setLocale(QLocale::system());
}

LinkTypeModel::LinkTypeModel(const QSqlDatabase & db)
 : LinkTypeModel(nullptr, db)
{
}

void LinkTypeModel::setLocale(const QLocale & locale)
{
  switch(locale.language()){
    case QLocale::French:
      pvNameFieldName = "NameFR";
      break;
    case QLocale::German:
      pvNameFieldName = "NameDE";
      break;
    case QLocale::Italian:
      pvNameFieldName = "NameIT";
      break;
    default:
      pvNameFieldName = "NameEN";
  }
  updateQuery();
}

int LinkTypeModel::row(LinkType type)
{
  return row(LinkTypePk(type));
}

int LinkTypeModel::row(LinkTypePk pk)
{
  if(isInError()){
    return -1;
  }
  const int n = rowCount();
  for(int row = 0; row < n; ++row){
    QModelIndex idx = index(row, 0);
    if(data(idx) == pk.code()){
      return row;
    }
  }
  // Update error only if a error occured
  isInError();

  return -1;
}

LinkTypePk LinkTypeModel::primaryKey(int row)
{
  LinkTypePk pk;

  if(row < 0){
    return pk;
  }
  if(isInError()){
    return pk;
  }
  QModelIndex idx = index(row, 0);
  pk = LinkTypePk::fromQVariant( data(idx) );
  if(pk.isNull()){
    QString msg = QString(tr("Could not find link type for row '%1'.")).arg(row);
    pvLastError = mdtErrorNewQ(msg, mdtError::Error, this);
    pvLastError.commit();
  }

  return pk;
}

LinkTypePk LinkTypeModel::currentPrimaryKey(const QComboBox * const cb)
{
  Q_ASSERT(cb != nullptr);

  return primaryKey(cb->currentIndex());
}

QString LinkTypeModel::unit(int row)
{
  QString str;

  if(row < 0){
    return str;
  }
  if(isInError()){
    str = tr("<Error>");
    return str;
  }
  QModelIndex idx = index(row, 2);
  str = data(idx).toString();
  if(str.isEmpty()){
    QString msg = QString(tr("Could not find unit for row '%1'.")).arg(row);
    pvLastError = mdtErrorNewQ(msg, mdtError::Error, this);
    pvLastError.commit();
  }

  return str;
}

void LinkTypeModel::updateQuery()
{
  Q_ASSERT(!pvNameFieldName.isEmpty());

  QString sql;

  sql = "SELECT Code_PK, " + pvNameFieldName + ", ValueUnit FROM LinkType_tbl ORDER BY " + pvNameFieldName + " ASC";
  setQuery(sql, pvDatabase);
}

bool LinkTypeModel::isInError()
{
  QSqlError sqlError = QSqlQueryModel::lastError();

  if(sqlError.isValid()){
    pvLastError = mdtErrorNewQ(tr("A error occured on table 'LinkType_tbl'."), mdtError::Error, this);
    pvLastError.stackError(ErrorFromQSqlQuery(query()));
    pvLastError.commit();
    return true;
  }

  return false;
}

}} // namespace Mdt{ namespace CableList{

