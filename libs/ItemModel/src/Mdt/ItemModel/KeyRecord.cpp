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
#include "KeyRecord.h"
#include <algorithm>

namespace Mdt{ namespace ItemModel{

void KeyRecord::append(int column, const QVariant& data)
{
  Q_ASSERT(column >= 0);

  mRecord.emplace_back(column, data);
}

void KeyRecord::clear()
{
  mRecord.clear();
}

QVariant KeyRecord::dataForColumn(int column) const
{
  Q_ASSERT(column >= 0);

  const auto it = iteratorForColumn(column);
  if(it == cend()){
    return QVariant();
  }

  return it->data();
}

Mdt::ItemModel::KeyRecord::const_iterator KeyRecord::iteratorForColumn(int column) const
{
  Q_ASSERT(column >= 0);

  const auto pred = [column](const KeyData & kd){
    return (kd.column() == column);
  };

  return std::find_if(cbegin(), cend(), pred);
}

}} // namespace Mdt{ namespace ItemModel{
