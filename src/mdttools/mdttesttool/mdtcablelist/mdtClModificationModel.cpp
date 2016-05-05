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
#include "mdtClModificationModel.h"
#include "mdt/sql/error/Error.h"
#include <QSqlError>
#include <QModelIndex>
#include <QComboBox>

//#include <QDebug>

mdtClModificationModel::mdtClModificationModel(QObject *parent, QSqlDatabase db, const QLocale &locale)
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
  sql = "SELECT Code_PK, " + fieldName + " FROM Modification_tbl ORDER BY " + fieldName + " ASC";
  setQuery(sql, db);
}

mdtClModificationModel::mdtClModificationModel(QSqlDatabase db, const QLocale &locale)
 : mdtClModificationModel(nullptr, db, locale)
{
}

int mdtClModificationModel::row(ModificationType m)
{
  return row(ModificationPk(m));
}

int mdtClModificationModel::row(ModificationPk key)
{
  int row;

  if(isInError()){
    return -1;
  }
  for(row = 0; row < rowCount(); ++row){
    QModelIndex idx = index(row, 0);
    if(data(idx) == key.code()){
      return row;
    }
  }
  // Update error only if a error occured
  isInError();

  return -1;
}

ModificationPk mdtClModificationModel::modificationPk(int row)
{
  ModificationPk pk;

  if(row < 0){
    return pk;
  }
  if(isInError()){
    return pk;
  }
  QModelIndex idx = index(row, 0);
  pk = Mdt::CableList::ModificationPk::fromCode( data(idx).toString().trimmed() );
  if(pk.isNull()){
    QString msg = QString(tr("Could not find modification for row '%1'.")).arg(row);
    pvLastError = mdtErrorNewQ(msg, mdtError::Error, this);
    pvLastError.commit();
  }

  return pk;
}

ModificationPk mdtClModificationModel::currentModificationPk(QComboBox *cb)
{
  Q_ASSERT(cb != nullptr);

  return modificationPk(cb->currentIndex());
}

bool mdtClModificationModel::isInError()
{
  QSqlError sqlError = QSqlQueryModel::lastError();

  if(sqlError.isValid()){
    QString msg = QString(tr("A error occured on table 'Modification_tbl'."));
    pvLastError = mdtErrorNewQ(msg, mdtError::Error, this);
    pvLastError.stackError(mdt::sql::error::fromQSqlError(sqlError));
    pvLastError.commit();
    return true;
  }

  return false;
}
