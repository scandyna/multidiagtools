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
#include "ModificationModel.h"
#include "mdt/sql/error/Error.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QModelIndex>
#include <QComboBox>

namespace Mdt{ namespace CableList{

ModificationModel::ModificationModel(QObject *parent, const QSqlDatabase & db)
 : QSqlQueryModel(parent),
   pvDatabase(db)
{
  setLocale(QLocale::system());
}

ModificationModel::ModificationModel(const QSqlDatabase & db)
 : ModificationModel(nullptr, db)
{
}

void ModificationModel::setLocale(const QLocale & locale)
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

int ModificationModel::row(ModificationType m)
{
  return row(ModificationPk(m));
}

int ModificationModel::row(ModificationPk pk)
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

ModificationPk ModificationModel::modificationPk(int row)
{
  ModificationPk pk;

  if(row < 0){
    return pk;
  }
  if(isInError()){
    return pk;
  }
  QModelIndex idx = index(row, 0);
  pk = ModificationPk::fromQVariant( data(idx) );
  if(pk.isNull()){
    QString msg = QString(tr("Could not find modification for row '%1'.")).arg(row);
    pvLastError = mdtErrorNewQ(msg, mdtError::Error, this);
    pvLastError.commit();
  }

  return pk;
}

ModificationPk ModificationModel::currentModificationPk(QComboBox *cb)
{
  Q_ASSERT(cb != nullptr);

  return modificationPk(cb->currentIndex());
}

void ModificationModel::updateQuery()
{
  Q_ASSERT(!pvNameFieldName.isEmpty());

  QString sql;

  sql = "SELECT Code_PK, " + pvNameFieldName + " FROM Modification_tbl ORDER BY " + pvNameFieldName + " ASC";
  setQuery(sql, pvDatabase);
}

bool ModificationModel::isInError()
{
  QSqlError sqlError = QSqlQueryModel::lastError();

  if(sqlError.isValid()){
    pvLastError = mdtErrorNewQ(tr("A error occured on table 'Modification_tbl'."), mdtError::Error, this);
    pvLastError.stackError(ErrorFromQSqlQuery(query()));
    pvLastError.commit();
    return true;
  }

  return false;
}

}} // namespace Mdt{ namespace CableList{
