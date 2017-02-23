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
#include "PrimaryKeyProxyModel.h"

namespace Mdt{ namespace ItemModel{

PrimaryKeyProxyModel::PrimaryKeyProxyModel(QObject* parent)
 : PkFkProxyModelBase(parent)
{
}

void PrimaryKeyProxyModel::setPrimaryKey(const PrimaryKey & pk)
{
  Q_ASSERT(!pk.isNull());

  setKey(pk.toColumnList());
}

void PrimaryKeyProxyModel::setPrimaryKey(std::initializer_list<int> pk)
{
  setPrimaryKey( PrimaryKey(pk) );
}

PrimaryKey PrimaryKeyProxyModel::primaryKey() const
{
  return PrimaryKey::fromColumnList(key());
}

void PrimaryKeyProxyModel::setPrimaryKeyEditable(bool editable)
{
  setKeyEditable(editable);
}

void PrimaryKeyProxyModel::setPrimaryKeyItemsEnabled(bool enable)
{
  setKeyItemsEnabled(enable);
}

PrimaryKeyRecord PrimaryKeyProxyModel::primaryKeyRecord(int row) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  return PrimaryKeyRecord::fromKeyRecord(keyRecord(row));
}

int PrimaryKeyProxyModel::findRowForPrimaryKeyRecord(const PrimaryKeyRecord & record) const
{
  return findFirstRowForKeyRecord(record);
}

}} // namespace Mdt{ namespace ItemModel{
