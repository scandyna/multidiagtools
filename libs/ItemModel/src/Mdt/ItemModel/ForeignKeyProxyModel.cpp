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
#include "ForeignKeyProxyModel.h"

namespace Mdt{ namespace ItemModel{

ForeignKeyProxyModel::ForeignKeyProxyModel(QObject* parent)
 : PkFkProxyModelBase(parent)
{
}

void ForeignKeyProxyModel::setForeignKey(const ForeignKey & fk)
{
  Q_ASSERT(!fk.isNull());

  setKey(fk.toColumnList());
}

void ForeignKeyProxyModel::setForeignKey(std::initializer_list<int> fk)
{
  setForeignKey( ForeignKey(fk) );
}

ForeignKey ForeignKeyProxyModel::foreignKey() const
{
  return ForeignKey::fromColumnList(key());
}

void ForeignKeyProxyModel::setForeignKeyEditable(bool editable)
{
  setKeyEditable(editable);
}

void ForeignKeyProxyModel::setForeignKeyItemsEnabled(bool enable)
{
  setKeyItemsEnabled(enable);
}

ForeignKeyRecord ForeignKeyProxyModel::foreignKeyRecord(int row) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  return ForeignKeyRecord::fromKeyRecord(keyRecord(row));
}

}} // namespace Mdt{ namespace ItemModel{
