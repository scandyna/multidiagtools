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
#include "ColumnFormatMap.h"
#include <algorithm>

// #include <QDebug>

namespace Mdt{ namespace ItemModel{

void ColumnFormatMap::clearFormatForColumn(int column)
{
  Q_ASSERT(column >= 0);

  const auto it = iteratorForColumn(column);
  if(it != mItems.cend()){
    mItems.erase(it);
  }
}

QVariant ColumnFormatMap::formatForColumn(int column) const
{
  Q_ASSERT(column >= 0);

  const auto it = iteratorForColumn(column);
  if(it != mItems.cend()){
    return it->value();
  }

  return QVariant();
}

ColumnFormatMap::const_iterator ColumnFormatMap::iteratorForColumn(int column) const
{
  Q_ASSERT(column >= 0);

  const auto pred = [column](const ColumnFormatMapItem & item){
    return item.column() == column;
  };
  return std::find_if(mItems.cbegin(), mItems.cend(), pred);
}

ColumnFormatMap::iterator ColumnFormatMap::iteratorForColumnW(int column)
{
  Q_ASSERT(column >= 0);

  const auto pred = [column](const ColumnFormatMapItem & item){
    return item.column() == column;
  };
  return std::find_if(mItems.begin(), mItems.end(), pred);
}

// int ColumnFormatMap::itemIndexForColumn(int column) const
// {
//   Q_ASSERT(column >= 0);
// 
//   const auto it = iteratorForColumn(column);
//   if(it == mItems.cend()){
//     return -1;
//   }
//   Q_ASSERT((it - mItems.cbegin()) >= 0);
//   Q_ASSERT((it - mItems.cbegin()) < (int)mItems.size());
// 
//   return it - mItems.cbegin();
// }

void ColumnFormatMap::setFormatVariantForColumn(int column, const QVariant & format)
{
  Q_ASSERT(column >= 0);

  auto it = iteratorForColumnW(column);
  if(it == mItems.end()){
    mItems.emplace_back(column, format);
  }else{
    *it = ColumnFormatMapItem(column, format);
  }

//   const int index = itemIndexForColumn(column);
//   if(index >= 0){
//     mItems[index] = ColumnFormatMapItem(column, format);
//   }else{
//     mItems.emplace_back(column, format);
//   }
}

}} // namespace Mdt{ namespace ItemModel{
