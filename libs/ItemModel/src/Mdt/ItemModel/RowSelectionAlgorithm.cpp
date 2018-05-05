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
#include "RowSelectionAlgorithm.h"
#include "Mdt/Algorithm.h"

// #include <QDebug>

namespace Mdt{ namespace ItemModel{

bool isRangeIncluded(const RowRange& a, const RowRange& b)
{
  Q_ASSERT(a.isValid());
  Q_ASSERT(b.isValid());

  /*
   * a:   |1|2|
   * b: |0|1|2|3|
   */

  return (a.firstRow() >= b.firstRow()) && (a.lastRow() <= b.lastRow());
}

bool compareRanges(const RowRange& a, const RowRange& b)
{
  Q_ASSERT(a.isValid());
  Q_ASSERT(b.isValid());
  Q_ASSERT(!Mdt::ItemModel::isRangeIncluded(a, b));
  Q_ASSERT(!Mdt::ItemModel::isRangeIncluded(b, a));

  /*
   * a: |0|1|2|
   * b:   |1|2|3|
   */

  return a.firstRow() < b.firstRow();
}

void sortRanges(RowRangeList& list)
{
  std::sort(list.begin(), list.end(), compareRanges);
}

bool isRangeAdjacent(const RowRange& a, const RowRange& b)
{
  Q_ASSERT(a.isValid());
  Q_ASSERT(b.isValid());
  Q_ASSERT(!Mdt::ItemModel::isRangeIncluded(a, b));
  Q_ASSERT(!Mdt::ItemModel::isRangeIncluded(b, a));
  Q_ASSERT(compareRanges(a, b));

  /*
   * Adjacent:
   * a: |0|1|
   * b:     |2|3|
   *
   * Adjacent:
   * a: |0|1|2|
   * b:   |1|2|3|
   *
   * Not adjacent:
   * a: |0|1|
   * b:       |3|4|
   */

  return (b.firstRow() - a.lastRow()) <= 1;
}

RowRange mergeRanges(const RowRange& a, const RowRange& b)
{
  Q_ASSERT(a.isValid());
  Q_ASSERT(b.isValid());
  Q_ASSERT(compareRanges(a, b));
  Q_ASSERT(!Mdt::ItemModel::isRangeIncluded(a, b));
  Q_ASSERT(!Mdt::ItemModel::isRangeIncluded(b, a));

  RowRange rowRange;

  rowRange.setFirstRow(a.firstRow());
  rowRange.setLastRow(b.lastRow());

  return rowRange;
}

bool hasAdjacentRanges(const RowRangeList& list)
{
  return findAdjacentRanges(list.cbegin(), list.cend()) != list.cend();
}

RowRangeList::const_iterator findLastAdjacentRange(RowRangeList::const_iterator first, RowRangeList::const_iterator last)
{
  Q_ASSERT(first != last);

  auto trailer = first;
  ++first;
  while(first != last){
    if(!isRangeAdjacent(*trailer, *first)){
      return trailer;
    }
    ++trailer;
    ++first;
  }
  Q_ASSERT(trailer != last);

  return trailer;
}

// QString rowRangeToString(const RowRange & r)
// {
//   return "(" + QString::number(r.firstRow()) + "-" + QString::number(r.lastRow()) + ")";
// }

void mergeAdjacentRanges(RowRangeList& list)
{
  if(list.size() < 2){
    return;
  }

  RowRangeList tmpList;
  auto first = list.cbegin();
  const auto last = list.cend();
  auto trailer = first;
  ++first;
  while(first != last){
    first = findLastAdjacentRange(trailer, last);
    if(trailer == first){
      tmpList.push_back(*trailer);
    }else{
      tmpList.push_back( mergeRanges(*trailer, *first) );
    }
    trailer = first + 1;
    ++first;
  }
  list = tmpList;
}

}} // namespace Mdt{ namespace ItemModel{
