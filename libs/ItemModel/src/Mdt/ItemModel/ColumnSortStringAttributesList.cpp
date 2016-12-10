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
  mList.emplace_back(column, caseSensitivity, numericMode);
}

const ColumnSortStringAttributes & ColumnSortStringAttributesList::attributesForColumn(int column) const
{
  const auto it = std::find_if(mList.cbegin(), mList.cend(), [column](const ColumnSortStringAttributes & csa){
                                                                  return (csa.column() == column);
                                                             });
  Q_ASSERT(it != mList.cend());
  return *it;
}

}} // namespace Mdt{ namespace ItemModel{
