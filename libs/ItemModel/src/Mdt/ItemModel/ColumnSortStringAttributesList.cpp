/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "ColumnSortStringAttributesList.h"
#include <QtGlobal>
#include <algorithm>

namespace Mdt{ namespace ItemModel{

void ColumnSortStringAttributesList::addColumn(int column, Qt::CaseSensitivity caseSensitivity, bool numericMode)
{
  Q_ASSERT(column >= 0);
  Q_ASSERT(attributesForColumn(column).isNull());

  mList.emplace_back(column, caseSensitivity, numericMode);
}

void ColumnSortStringAttributesList::setColumn(int column, Qt::CaseSensitivity caseSensitivity, bool numericMode)
{
  Q_ASSERT(column >= 0);

  auto it = std::find_if(mList.begin(), mList.end(), [column](const ColumnSortStringAttributes & csa){
                                                                  return (csa.column() == column);
                                                             });
  if(it == mList.cend()){
    addColumn(column, caseSensitivity, numericMode);
  }else{
    *it = ColumnSortStringAttributes(column, caseSensitivity, numericMode);
  }
}

ColumnSortStringAttributes ColumnSortStringAttributesList::attributesForColumn(int column) const
{
  Q_ASSERT(column >= 0);

  const auto it = std::find_if(mList.cbegin(), mList.cend(), [column](const ColumnSortStringAttributes & csa){
                                                                  return (csa.column() == column);
                                                             });
  if(it == mList.cend()){
    return ColumnSortStringAttributes();
  }
  return *it;
}

void ColumnSortStringAttributesList::clear()
{
  mList.clear();
}

}} // namespace Mdt{ namespace ItemModel{
