/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "SortFilterProxyModel.h"

// #include <QDebug>

namespace Mdt{ namespace ItemModel{

SortFilterProxyModel::SortFilterProxyModel(QObject* parent)
 : QSortFilterProxyModel(parent)
{
}

bool SortFilterProxyModel::insertRows(int row, int count, const QModelIndex& parent)
{
  /*
   * When row is >= rowCount() , QSortFilterProxyModel inserts rows in a inexpected way.
   * See https://bugreports.qt.io/browse/QTBUG-58499
   * As workaround, we insert rows at end of the source model if row is > rowCount()
   */
  Q_ASSERT(sourceModel() != nullptr);
  if(row < rowCount(parent)){
    return QSortFilterProxyModel::insertRows(row, count, parent);
  }
  return sourceModel()->insertRows(sourceModel()->rowCount(parent), count, parent);
}

}} // namespace Mdt{ namespace ItemModel{
