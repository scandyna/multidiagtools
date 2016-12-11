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
#include "SortProxyModel.h"
#include "ColumnSortOrder.h"
#include <algorithm>

#include <QDebug>

namespace Mdt{ namespace ItemModel{

SortProxyModel::SortProxyModel(QObject* parent)
 : QSortFilterProxyModel(parent)
{
}

void SortProxyModel::addColumnToSortOrder(int column, Qt::SortOrder sortOrder)
{
//   qDebug() << "add(" << column << ")";
  mColumnSortOrderList.addColumn(column, sortOrder);
}

void SortProxyModel::addColumnToSortOrder(int column, Qt::CaseSensitivity caseSensitivity, bool numericMode, Qt::SortOrder sortOrder)
{

}

void SortProxyModel::clearColumnsSortOrder()
{
  mColumnSortOrderList.clear();
}

void SortProxyModel::sort(int column, Qt::SortOrder order)
{
  Q_ASSERT(column >= -1);
  Q_ASSERT(column < columnCount());

//   qDebug() << "sort(" << column << ")";
  QSortFilterProxyModel::sort(column, order);
}

void SortProxyModel::sort()
{
//   qDebug() << "sort()";
  std::for_each(mColumnSortOrderList.crbegin(), mColumnSortOrderList.crend(), [this](const ColumnSortOrder & cso){
                                                                  sort(cso.column(), cso.sortOrder());
                                                                });
}

bool SortProxyModel::lessThan(const QModelIndex & source_left, const QModelIndex & source_right) const
{
//   qDebug() << "lessThan, left: " << sourceModel()->data(source_left) << " , right: " << sourceModel()->data(source_right);
  return QSortFilterProxyModel::lessThan(source_left, source_right);
}

}} // namespace Mdt{ namespace ItemModel{
