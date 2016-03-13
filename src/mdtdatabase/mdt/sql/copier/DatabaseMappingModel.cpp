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
#include "DatabaseMappingModel.h"

namespace mdt{ namespace sql{ namespace copier{

DatabaseMappingModel::DatabaseMappingModel(QObject *parent)
 : MappingModel(parent)
{
}

mdtExpected<bool> DatabaseMappingModel::setSourceDatabase(const QSqlDatabase & db)
{
  mdtExpected<bool> ret;

  beginResetModel();
  ret = pvMapping.setSourceDatabase(db);
  endResetModel();

  return ret;
}

mdtExpected<bool> DatabaseMappingModel::setDestinationDatabase(const QSqlDatabase & db)
{
  mdtExpected<bool> ret;

  beginResetModel();
  ret = pvMapping.setDestinationDatabase(db);
  endResetModel();

  return ret;
}

mdtExpected<bool> DatabaseMappingModel::generateTableMappingByName()
{
  mdtExpected<bool> ret;

  beginResetModel();
  ret = pvMapping.generateTableMappingByName();
  endResetModel();

  return ret;
}

int DatabaseMappingModel::rowCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return pvMapping.tableMappingCount();
}

void DatabaseMappingModel::tableMappingUpdated(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  auto firstIdx = index(row, SourceTableNameIndex);
  auto lastIdx = index(row, TableMappingStateIndex);
  emit dataChanged(firstIdx, lastIdx);
}

}}} // namespace mdt{ namespace sql{ namespace copier{
