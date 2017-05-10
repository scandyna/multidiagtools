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
#include "PkFkProxyModelBase.h"
#include <algorithm>

namespace Mdt{ namespace ItemModel{

PkFkProxyModelBase::PkFkProxyModelBase(QObject* parent)
 : QIdentityProxyModel(parent)
{
}

KeyRecord PkFkProxyModelBase::getKeyRecord(int row, const ColumnList& key) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  KeyRecord record;
  for(int column : key){
    Q_ASSERT(column < columnCount());
    record.append(column, index(row, column).data());
  }
  return record;
}

int PkFkProxyModelBase::findFirstRowForKeyRecord(const KeyRecord & record) const
{
//   Q_ASSERT(record.columnCount() == mKey.size());

  const int n = rowCount();
  for(int row = 0; row < n; ++row){
    if(rowMatchesKeyRecord(row, record)){
      return row;
    }
  }

  return -1;
}

bool PkFkProxyModelBase::rowMatchesKeyRecord(int row, const KeyRecord & record) const
{
  const auto pred = [row, this](const KeyData & kd){
    Q_ASSERT(kd.column() < columnCount());
    return( index(row, kd.column()).data() == kd.data() );
  };
  return std::all_of(record.cbegin(), record.cend(), pred);
}

}} // namespace Mdt{ namespace ItemModel{
