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
#include "RowSelection.h"
#include "RowSelectionAlgorithm.h"
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QItemSelectionRange>
#include <QModelIndexList>
#include <QModelIndex>
#include <algorithm>

// #include <QDebug>

namespace Mdt{ namespace ItemModel{

void RowSelection::appendRange(const RowRange& range)
{
  Q_ASSERT(range.isValid());

  if(isRangeIncluded(range)){
    return;
  }
  mRangeList.push_back(range);
  sortRangeList();
  mergeAdjacentRanges(mRangeList);
}

void RowSelection::clear()
{
  mRangeList.clear();
}

bool RowSelection::isRangeIncluded(const RowRange& range) const
{
  Q_ASSERT(range.isValid());

  const auto pred = [&range](const RowRange & r){
    return Mdt::ItemModel::isRangeIncluded(range, r);
  };

  return std::any_of(mRangeList.cbegin(), mRangeList.cend(), pred);
}

RowSelection RowSelection::fromSelectionModel(const QItemSelectionModel* selectionModel)
{
  RowSelection rowSelection;

  if(selectionModel == nullptr){
    return rowSelection;
  }
  const auto itemSelection = selectionModel->selection();
  for(const auto & itemSelectionRange : itemSelection){
    RowRange rowRange;
    rowRange.setFirstRow(itemSelectionRange.top());
    rowRange.setLastRow(itemSelectionRange.bottom());
    rowSelection.appendRange(rowRange);
  }

  return rowSelection;
}

void RowSelection::sortRangeList()
{
  sortRanges(mRangeList);
}

}} // namespace Mdt{ namespace ItemModel{
