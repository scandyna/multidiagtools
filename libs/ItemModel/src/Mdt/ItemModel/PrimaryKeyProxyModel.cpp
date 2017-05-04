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

  mPk = pk;
//   setKey(pk.toColumnList());
}

void PrimaryKeyProxyModel::setPrimaryKey(std::initializer_list<int> pk)
{
  setPrimaryKey( PrimaryKey(pk) );
}

PrimaryKey PrimaryKeyProxyModel::primaryKey() const
{
  return mPk;
//   return PrimaryKey::fromColumnList(key());
}

void PrimaryKeyProxyModel::setPrimaryKeyEditable(bool editable)
{
  mIsPkEditable = editable;
//   setKeyEditable(editable);
}

void PrimaryKeyProxyModel::setPrimaryKeyItemsEnabled(bool enable)
{
  mIsPkItemsEnabled = enable;
//   setKeyItemsEnabled(enable);
}

Qt::ItemFlags PrimaryKeyProxyModel::flags(const QModelIndex & index) const
{
  if(!index.isValid()){
    return PkFkProxyModelBase::flags(index);
  }
  if(mIsPkEditable && mIsPkItemsEnabled){
    return PkFkProxyModelBase::flags(index);
  }
  if(!mPk.containsColumn(index.column())){
    return PkFkProxyModelBase::flags(index);
  }
  auto f = PkFkProxyModelBase::flags(index);
  if(!mIsPkEditable){
    f &= Qt::ItemFlags(~Qt::ItemIsEditable);
  }
  if(!mIsPkItemsEnabled){
    f &= Qt::ItemFlags(~Qt::ItemIsEnabled);
  }
  return f;
}

PrimaryKeyRecord PrimaryKeyProxyModel::getPrimaryKeyRecord(int row) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  return PrimaryKeyRecord::fromKeyRecord( getKeyRecord(row, mPk.toColumnList()) );
}

int PrimaryKeyProxyModel::findRowForPrimaryKeyRecord(const PrimaryKeyRecord & record) const
{
  Q_ASSERT(record.columnCount() == mPk.columnCount());

  return findFirstRowForKeyRecord(record);
}

}} // namespace Mdt{ namespace ItemModel{
