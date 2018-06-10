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
#include "RemoveRows.h"
#include "RowRange.h"
#include <iterator>

// #include <QDebug>

namespace Mdt{ namespace ItemModel{

// QString rangeToString(const RowRange & r)
// {
//   return "(" + QString::number(r.firstRow()) + "-" + QString::number(r.lastRow()) + ")";
// }

bool removeRows(QAbstractItemModel* model, const RowRange& rowRange)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(rowRange.isValid());
  Q_ASSERT(rowRange.lastRow() < model->rowCount());

  return model->removeRows(rowRange.firstRow(), rowRange.rowCount());
}

bool removeRows(QAbstractItemModel* model, const RowSelection& selection)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(!selection.isEmpty());

  auto rFirst = std::make_reverse_iterator(selection.cend());
  const auto rLast = std::make_reverse_iterator(selection.cbegin());
  while(rFirst != rLast){
    if( !removeRows(model, *rFirst) ){
      return false;
    }
    ++rFirst;
  }

  return true;
}

bool removeRows(QAbstractItemModel* model, const QItemSelectionModel* selectionModel)
{
  Q_ASSERT(model != nullptr);
  Q_ASSERT(selectionModel != nullptr);

  if(!selectionModel->hasSelection()){
    return true;
  }

  return removeRows(model, RowSelection::fromSelectionModel(selectionModel) );
}

}} // namespace Mdt{ namespace ItemModel{
