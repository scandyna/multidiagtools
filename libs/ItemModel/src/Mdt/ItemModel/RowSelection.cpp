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
// #include "Mdt/Algorithm.h"
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QItemSelectionRange>
#include <QModelIndexList>
#include <QModelIndex>
#include <algorithm>

#include <QDebug>

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

  // merge each adjacent ranges
  
  
  
  

  
}

bool RowSelection::isRangeIncluded(const RowRange& range) const
{
  Q_ASSERT(range.isValid());

  const auto pred = [&range](const RowRange & r){
    return Mdt::ItemModel::isRangeIncluded(range, r);
  };

  return std::any_of(mRangeList.cbegin(), mRangeList.cend(), pred);
}

// bool RowSelection::isRangeIncluded(const RowRange& a, const RowRange& b)
// {
//   Q_ASSERT(a.isValid());
//   Q_ASSERT(b.isValid());
// 
//   /*
//    * a:   |1|2|
//    * b: |0|1|2|3|
//    */
// 
//   return (a.firstRow() >= b.firstRow()) && (a.lastRow() <= b.lastRow());
// }

// bool RowSelection::compareRanges(const RowRange& a, const RowRange& b)
// {
//   Q_ASSERT(a.isValid());
//   Q_ASSERT(b.isValid());
//   Q_ASSERT(!Mdt::ItemModel::isRangeIncluded(a, b));
//   Q_ASSERT(!Mdt::ItemModel::isRangeIncluded(b, a));
// 
//   /*
//    * a: |0|1|2|
//    * b:   |1|2|3|
//    */
// 
//   return a.firstRow() < b.firstRow();
// }

// void RowSelection::sortRanges(RowRangeList& list)
// {
//   std::sort(list.begin(), list.end(), compareRanges);
// }

// bool RowSelection::isRangeAdjacent(const RowRange& a, const RowRange& b)
// {
//   Q_ASSERT(a.isValid());
//   Q_ASSERT(b.isValid());
//   Q_ASSERT(!Mdt::ItemModel::isRangeIncluded(a, b));
//   Q_ASSERT(!Mdt::ItemModel::isRangeIncluded(b, a));
//   Q_ASSERT(compareRanges(a, b));
// 
//   /*
//    * Adjacent:
//    * a: |0|1|
//    * b:     |2|3|
//    *
//    * Adjacent:
//    * a: |0|1|2|
//    * b:   |1|2|3|
//    *
//    * Not adjacent:
//    * a: |0|1|
//    * b:       |3|4|
//    */
// 
//   return (b.firstRow() - a.lastRow()) <= 1;
// }

// RowRange RowSelection::mergeRanges(const RowRange& a, const RowRange& b)
// {
//   Q_ASSERT(a.isValid());
//   Q_ASSERT(b.isValid());
//   Q_ASSERT(isRangeAdjacent(a, b));
// 
//   RowRange rowRange;
// 
//   rowRange.setFirstRow(a.firstRow());
//   rowRange.setLastRow(b.lastRow());
// 
//   return rowRange;
// }

// void RowSelection::mergeAdjacentRanges(RowRangeList& list)
// {
//   if(list.size() < 2){
//     return;
//   }
// 
//   RowRangeList tmpList;
// //   const auto & constInList = list;
// 
//   const auto f = [&tmpList](const RowRange & a, const RowRange & b){
//     qDebug() << "a(" << a.firstRow() << "," << a.lastRow() << "), b(" << b.firstRow() << "," << b.lastRow() << ")";
//     if(isRangeAdjacent(a, b)){
//       qDebug() << "merge(a,b)";
//       tmpList.push_back( mergeRanges(a, b) );
//     }else{
//       qDebug() << "add a and b";
//       tmpList.push_back(a);
//       tmpList.push_back(b);
//     }
//   };
//   
//   /** \todo Must proceed until list is no longer mergeable
//    *
//    * ForwardIt findAdjacentRanges(ForwardIt first, ForwardIt last);
//    * void mergeAdjacentRanges(ForwardIt first, ForwardIt last);
//    *
//    * do{
//    *   it = findAdjacentRanges(list.cbegin(), list.cend());
//    *   mergeAdjacentRanges(it, list.end());
//    * }while(it != list.cend());
//    */
//   
//   Mdt::Algorithm::forEachAdjacentPair(list.cbegin(), list.cend(), f);
// 
//   list = tmpList;
// }

// bool RowSelection::rangesAreConsecutive(const RowRange& a, const RowRange& b)
// {
//   Q_ASSERT(a.isValid());
//   Q_ASSERT(b.isValid());
// 
//   return std::abs(a.lastRow() - b.firstRow()) < 1;
// }

RowSelection RowSelection::fromSelectionModel(const QItemSelectionModel* selectionModel)
{
  RowSelection rowSelection;

  if(selectionModel == nullptr){
    return rowSelection;
  }
  const auto itemSelection = selectionModel->selection();
  
//   qDebug() << itemSelection;
  qDebug() << "--------";
  
  for(const auto & itemSelectionRange : itemSelection){
    qDebug() << itemSelectionRange;
    RowRange rowRange;
    rowRange.setFirstRow(itemSelectionRange.top());
    rowRange.setLastRow(itemSelectionRange.bottom());
    rowSelection.appendRange(rowRange);
  }

  return rowSelection;
}

void RowSelection::sortRangeList()
{
//   const auto pred = [](const RowRange & a, const RowRange & b){
//     return a.lastRow() < b.firstRow();
//   };
//   std::sort(mRangeList.begin(), mRangeList.end(), compareRanges);
  sortRanges(mRangeList);
}

}} // namespace Mdt{ namespace ItemModel{
