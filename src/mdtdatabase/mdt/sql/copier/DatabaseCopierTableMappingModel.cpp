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
#include "DatabaseCopierTableMappingModel.h"

//#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

DatabaseCopierTableMappingModel::DatabaseCopierTableMappingModel(QObject* parent)
 : TableMappingModel(parent),
   pvMapping(std::make_shared<DatabaseCopierTableMapping>())
{
}

void DatabaseCopierTableMappingModel::setSourceDatabase(const QSqlDatabase & db)
{
  Q_ASSERT(db.isOpen());

  beginResetModel();
  pvMapping->setSourceDatabase(db);
  endResetModel();
}

bool DatabaseCopierTableMappingModel::setSourceTable(const QString & tableName)
{
  bool ok;

  beginResetModel();
  ok = pvMapping->setSourceTable(tableName);
  endResetModel();
  if(!ok){
    pvLastError = pvMapping->lastError();
    return false;
  }

  return true;
}

bool DatabaseCopierTableMappingModel::setDestinationTable(const QString & tableName, const QSqlDatabase & db)
{
  bool ok;

  beginResetModel();
  ok = pvMapping->setDestinationTable(tableName, db);
  endResetModel();
  if(!ok){
    pvLastError = pvMapping->lastError();
    return false;
  }

  return true;
}

void DatabaseCopierTableMappingModel::setMapping(const std::shared_ptr<DatabaseCopierTableMapping> & m)
{
  Q_ASSERT(m);

  beginResetModel();
  pvMapping = m;
  endResetModel();
}

}}} // namespace mdt{ namespace sql{ namespace copier{
