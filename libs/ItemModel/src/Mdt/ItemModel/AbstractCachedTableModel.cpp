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

// #include <QDebug>

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
    case TableCacheOperation::InsertDelete:
      return QLatin1String("x");
    case TableCacheOperation::None:
      break;
  }

  return ParentClass::headerData(section, orientation, role);
}

Qt::ItemFlags AbstractCachedTableModel::flags(const QModelIndex& index) const
{
  /*
   * After removing rows, some views seems to query flags on out of range row
   */
  if(index.isValid() && (index.row() < rowCountImpl()) ){
    const auto op = operationAtRow(index.row());
    if( (op == TableCacheOperation::Delete) || (op == TableCacheOperation::InsertDelete) ){
      return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
  }

  return ParentClass::flags(index);
}

void AbstractCachedTableModel::cancelRemoveRows(int, int)
{
}

void AbstractCachedTableModel::beginInsertRows(int firstRow, int lastRow)
{
  ParentClass::beginInsertRows(QModelIndex(), firstRow, lastRow);
}

void AbstractCachedTableModel::endInsertRows()
{
  ParentClass::endInsertRows();
}

void AbstractCachedTableModel::beginRemoveRows(int firstRow, int lastRow)
{
  ParentClass::beginRemoveRows(QModelIndex(), firstRow, lastRow);
}

void AbstractCachedTableModel::endRemoveRows()
{
  ParentClass::endRemoveRows();
}

}} // namespace Mdt{ namespace ItemModel{
