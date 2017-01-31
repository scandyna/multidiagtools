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

#include <QDebug>

namespace Mdt{ namespace ItemModel{

SortFilterProxyModel::SortFilterProxyModel(QObject* parent)
 : QSortFilterProxyModel(parent)
{
}

bool SortFilterProxyModel::insertRows(int row, int count, const QModelIndex& parent)
{
  /*
   * QSortFilterProxyModel::insertRows() is implemented something like this:
   * - Create a list of source rows that matches filterAcceptsRow() , if not allready done
   * - Check row on list size (looks like a bug: should check the highest source row in the list, not list size)
   * - Determine source_row:
   *   -> If row >= list size: source_row = liste size (Again, this seems wrong)
   *   -> Else: source_row = row at in list (Ok)
   * - Call sourceModel::insertRows(source_row, count, source_parent)
   *
   * This says that, if row > this proxy model's rowCount(),
   * the insert location will be wrong (= not as expected).
   * As workaround, we proceed this way:
   * - Turn mInsertingRows flag ON to tell filterAcceptsRow() to accept all rows,
   *   so the size of QSortFilterProxyModel's list of source rows will be equal to source model's row count
   * - Invalidate filter to force QSortFilterProxyModel to recreate its mapping (the list of source rows)
   * - Call QSortFilterProxyModel::insertRows()
   * - Turn mInsertingRows flag off to tell filterAcceptsRow() to proceed in normal way
   * - Invalidate filter ? Or not.
   */
  qDebug() << "SFPM::insertRows() - row: " << row << " , row count: " << rowCount() << " , src row count: " << sourceModel()->rowCount();

  return QSortFilterProxyModel::insertRows(row, count, parent);
}

}} // namespace Mdt{ namespace ItemModel{
