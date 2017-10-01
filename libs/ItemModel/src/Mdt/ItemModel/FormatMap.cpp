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
#include "FormatMap.h"
#include <algorithm>

namespace Mdt{ namespace ItemModel{

FormatMap::FormatMap()
 : mPriority{FormatMapPriority::Index, FormatMapPriority::Row, FormatMapPriority::Column}
{
}

void FormatMap::setPriority(const std::array< FormatMapPriority, int(3) >& priority)
{
  mPriority = priority;
}

void FormatMap::clearFormatForIndex(int row, int column)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  mIndexMap.clearFormatForIndex(row, column);
}

void FormatMap::clearFormatForRow(int row)
{
  Q_ASSERT(row >= 0);

  mRowMap.clearFormatForRow(row);
}

void FormatMap::clearFormatForColumn(int column)
{
  Q_ASSERT(column >= 0);

  mColumnMap.clearFormatForColumn(column);
}


QVariant FormatMap::formatForIndex(int row, int column) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  QVariant format;
  
  const auto pred = [&format, this, row, column](FormatMapPriority priority){
    switch(priority){
      case FormatMapPriority::Index:
        format = mIndexMap.formatForIndex(row, column);
        break;
      case FormatMapPriority::Row:
        format = mRowMap.formatForRow(row);
        break;
      case FormatMapPriority::Column:
        format = mColumnMap.formatForColumn(column);
        break;
    }
    return !format.isNull();
  };
  std::find_if(mPriority.cbegin(), mPriority.cend(), pred);

  return format;
}

}} // namespace Mdt{ namespace ItemModel{
