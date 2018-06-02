/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "AbstractCachedTableModel.h"
#include "Mdt/IndexRange/RowRange.h"
#include <QLatin1String>

#include <QDebug>

using namespace Mdt::Container;

namespace Mdt{ namespace ItemModel{

AbstractCachedTableModel::AbstractCachedTableModel(QObject* parent)
 : AbstractTableModel(parent)
{
}

QVariant AbstractCachedTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if( (orientation != Qt::Vertical) || (role != Qt::DisplayRole) ){
    return ParentClass::headerData(section, orientation, role);
  }
  Q_ASSERT(section >= 0);
  Q_ASSERT(section < rowCount());

  const auto operation = operationAtRow(section);
  switch(operation){
    case TableCacheOperation::Insert:
      return QLatin1String("*");
    case TableCacheOperation::Update:
      return QLatin1String("e");
    case TableCacheOperation::Delete:
      return QLatin1String("x");
    case TableCacheOperation::None:
      break;
  }

  return ParentClass::headerData(section, orientation, role);
}

Qt::ItemFlags AbstractCachedTableModel::flags(const QModelIndex& index) const
{
  if(index.isValid()){
    Q_ASSERT(index.row() < rowCountImpl());
    if(operationAtRow(index.row()) == TableCacheOperation::Delete){
      return Qt::ItemIsSelectable;
    }
  }

  return ParentClass::flags(index);
}

bool AbstractCachedTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
  if(parent.isValid()){
    return false;
  }
  Q_ASSERT(row >= 0);
  Q_ASSERT(count >= 1);
  Q_ASSERT( (row+count) <= rowCountImpl() );

  if(!removeRowsFromCache(row, count)){
    return false;
  }
  Mdt::IndexRange::RowRange rowRange;
  rowRange.setFirstRow(row);
  rowRange.setRowCount(count);
  emitVerticalHeaderDataChanged(rowRange);

  return true;
}

// bool AbstractCachedTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
// {
//   if(!ParentClass::setData(index, value, role)){
//     return false;
//   }
//   Q_ASSERT(index.isValid());
//   Q_ASSERT(index.row() < rowCountImpl());
// 
//   emitVerticalHeaderDataChanged(index.row());
// 
//   return true;
// }

}} // namespace Mdt{ namespace ItemModel{
