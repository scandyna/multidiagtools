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
#include "IndexFormatMap.h"
#include <algorithm>

namespace Mdt{ namespace ItemModel{

void IndexFormatMap::clearFormatForIndex(int row, int column)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  const auto it = iteratorForIndex(row, column);
  if(it != mItems.cend()){
    mItems.erase(it);
  }
}

QVariant IndexFormatMap::formatForIndex(int row, int column) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  const auto it = iteratorForIndex(row, column);
  if(it != mItems.cend()){
    return it->value();
  }

  return QVariant();
}

void IndexFormatMap::setFormatVariantForIndex(int row, int column, const QVariant & format)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  auto it = iteratorForIndexW(row, column);
  if(it == mItems.end()){
    mItems.emplace_back(row, column, format);
  }else{
    *it = IndexFormatMapItem(row, column, format);
  }
}

IndexFormatMap::const_iterator IndexFormatMap::iteratorForIndex(int row, int column) const
{
  Q_ASSERT(column >= 0);

  const auto pred = [row, column](const IndexFormatMapItem & item){
    return ( item.row() == row && item.column() == column );
  };
  return std::find_if(mItems.cbegin(), mItems.cend(), pred);
}

IndexFormatMap::iterator IndexFormatMap::iteratorForIndexW(int row, int column)
{
  Q_ASSERT(column >= 0);

  const auto pred = [row, column](const IndexFormatMapItem & item){
    return ( item.row() == row && item.column() == column );
  };
  return std::find_if(mItems.begin(), mItems.end(), pred);
}

}} //namespace Mdt{ namespace ItemModel{
