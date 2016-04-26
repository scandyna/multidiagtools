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
#include "CsvFileImportMappingModel.h"

namespace mdt{ namespace sql{ namespace copier{

CsvFileImportMappingModel::CsvFileImportMappingModel(QObject *parent)
 : MappingModel(parent)
{
}

void CsvFileImportMappingModel::setDestinationDatabase(const QSqlDatabase & db)
{
  beginResetModel();
  pvMapping.setDestinationDatabase(db);
  endResetModel();
}

void CsvFileImportMappingModel::appendTableMapping(const std::shared_ptr<CsvFileImportTableMapping> & tm)
{
  Q_ASSERT(tm);

  const int start = pvMapping.tableMappingCount();
  beginInsertRows(QModelIndex(), start, start);
  pvMapping.appendTableMapping(tm);
  endInsertRows();
}

void CsvFileImportMappingModel::removeTableMapping(int index)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvMapping.tableMappingCount());

  beginRemoveRows(QModelIndex(), index, index);
  pvMapping.removeTableMapping(index);
  endRemoveRows();
}

bool CsvFileImportMappingModel::resetMapping()
{
  bool ret;

  beginResetModel();
  ret = pvMapping.resetMapping();
  endResetModel();

  return ret;
}

int CsvFileImportMappingModel::rowCount(const QModelIndex & parent) const
{
  // Check parent validity (case of use with a tree view)
  if(parent.isValid()){
    return 0;
  }
  return pvMapping.tableMappingCount();
}

void CsvFileImportMappingModel::tableMappingUpdated(int row)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  auto firstIdx = index(row, SourceTableNameIndex);
  auto lastIdx = index(row, TableMappingStateIndex);
  emit dataChanged(firstIdx, lastIdx);
}


}}} // namespace mdt{ namespace sql{ namespace copier{
