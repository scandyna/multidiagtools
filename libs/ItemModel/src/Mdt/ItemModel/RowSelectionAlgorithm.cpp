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

#include <QDebug>

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
//   Q_ASSERT(isRangeAdjacent(a, b));

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

//   if(first == last){
//     return last;
//   }
//   bool foundOne = false;
//   auto trailer = first;
//   ++first;
//   while(first != last){
//     qDebug() << "trailer(" << trailer->firstRow() << "-" << trailer->lastRow() << ")  first(" << first->firstRow() << "-" << first->lastRow() << ")";
//     if(isRangeAdjacent(*trailer, *first)){
//       foundOne = true;
//     }else{
//       break;
//     }
//     ++trailer;
//     ++first;
//   }
//   if(foundOne){
//     return trailer;
//   }
//   return trailer;
}

QString rowRangeToString(const RowRange & r)
{
  return "(" + QString::number(r.firstRow()) + "-" + QString::number(r.lastRow()) + ")";
}

void mergeAdjacentRanges(RowRangeList& list)
{
  if(list.size() < 2){
    return;
  }

  RowRangeList tmpList;
//   const auto & constInList = list;

  qDebug() << "---- START-------";
  
  auto first = list.cbegin();
  const auto last = list.cend();
  auto trailer = first;
  ++first;
  while(first != last){
    first = findLastAdjacentRange(trailer, last);
    if(trailer == first){
      qDebug() << "add(" << rowRangeToString(*trailer) << ")";
      tmpList.push_back(*trailer);
    }else{
      qDebug() << "merge( t(" << rowRangeToString(*trailer) << "), f(" << rowRangeToString(*first) << ") )";
      qDebug() << "add(" << rowRangeToString( mergeRanges(*trailer, *first) ) << ")";
      tmpList.push_back( mergeRanges(*trailer, *first) );
    }
    trailer = first + 1;
    ++first;
  }

  
  /*! \todo Must find the last adjacent pair starting from a index
   * 
   */
//   auto first = list.cbegin();
//   const auto last = list.cend();
//   auto trailer = first;
//   while(first != last){
//     trailer = findLastAdjacentRange(first, last);
//     if(trailer != last){
//       qDebug() << "merge(t,f)";
//       tmpList.push_back( mergeRanges(*trailer, *first) );
//     }else{
//       qDebug() << "add a";
//       tmpList.push_back(*first);
//       if(first != last){
//         qDebug() << "add b";
//         ++first;
//         tmpList.push_back(*first);
//       }
//     }
//     ++first;
// //     ++trailer;
//   }
  
//   while(first != last){
//     it = findLastAdjacent(first, ...);
//     if(it != last){
//       merge(first, it);
//     }
//     first = it;
//   }

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

  
  
  
//   while(hasAdjacentRanges(list)){
//     Mdt::Algorithm::forEachAdjacentPair(list.cbegin(), list.cend(), f);
//     list = tmpList;
//   }
  
//   auto it = list.cbegin();
//   while(it != list.cend()){
//     it = findAdjacentRanges(it, list.cend());
//     Mdt::Algorithm::forEachAdjacentPair(it, list.cend(), f);
//   }
  /** \todo Must proceed until list is no longer mergeable
   *
   * ForwardIt findAdjacentRanges(ForwardIt first, ForwardIt last);
   * void mergeAdjacentRanges(ForwardIt first, ForwardIt last);
   *
   * do{
   *   it = findAdjacentRanges(list.cbegin(), list.cend());
   *   mergeAdjacentRanges(it, list.end());
   * }while(it != list.cend());
   */
  
//   Mdt::Algorithm::forEachAdjacentPair(list.cbegin(), list.cend(), f);
  list = tmpList;
  qDebug() << "---- DONE-------";
}

}} // namespace Mdt{ namespace ItemModel{
