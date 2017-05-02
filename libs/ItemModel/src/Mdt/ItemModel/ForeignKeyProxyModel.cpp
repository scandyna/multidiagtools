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

void ForeignKeyProxyModel::addForeignKey(const QString& foreignEntityName, const ForeignKey& fk)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  Q_ASSERT(!fk.isNull());

  mMap.addForeignKey(foreignEntityName, fk);
//   mMap.insert(foreignEntityName, fk);
}

void ForeignKeyProxyModel::addForeignKey(const QString& foreignEntityName, std::initializer_list< int > fk)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());

  addForeignKey( foreignEntityName, ForeignKey(fk) );
}

ForeignKey ForeignKeyProxyModel::getForeignKeyReferencing(const QString& entityName) const
{
  Q_ASSERT(!entityName.trimmed().isEmpty());

  return mMap.getForeignKeyReferencing(entityName);
//   return mMap.value(entityName).foreignKey();
}

void ForeignKeyProxyModel::setForeignKeyEditable(const QString& foreignEntityName, bool editable)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  ///Q_ASSERT(mMap.contains(foreignEntityName));

  mMap.setForeignKeyEditable(foreignEntityName, editable);
//   mMap[foreignEntityName].setForeignKeyEditable(editable);
}

void ForeignKeyProxyModel::setAllForeignKeysEditable(bool editable)
{
  mMap.setAllForeignKeysEditable(editable);
}

bool ForeignKeyProxyModel::isForeignKeyEditable(const QString& foreignEntityName) const
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  ///Q_ASSERT(mMap.contains(foreignEntityName));

  return mMap.isForeignKeyEditable(foreignEntityName);
//   return mMap.value(foreignEntityName).isForeignKeyEditable();
}

void ForeignKeyProxyModel::setForeignKeyItemsEnabled(const QString& foreignEntityName, bool enable)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  ///Q_ASSERT(mMap.contains(foreignEntityName));

  mMap.setForeignKeyItemsEnabled(foreignEntityName, enable);
//   mMap[foreignEntityName].setForeignKeyItemsEnabled(enable);
}

void ForeignKeyProxyModel::setAllForeignKeysItemsEnabled(bool enable)
{
  mMap.setAllForeignKeysItemsEnabled(enable);
}

bool ForeignKeyProxyModel::isForeignKeyItemsEnabled(const QString& foreignEntityName) const
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  ///Q_ASSERT(mMap.contains(foreignEntityName));

  return mMap.isForeignKeyItemsEnabled(foreignEntityName);
//   return mMap.value(foreignEntityName).isForeignKeyItemsEnabled();
}

Qt::ItemFlags ForeignKeyProxyModel::flags(const QModelIndex& index) const
{
  if(!index.isValid()){
    return PkFkProxyModelBase::flags(index);
  }
  const auto editableEnabledFlags = mMap.getMostRestrictiveFlagsForColumn(index.column());
  if(editableEnabledFlags.isForeignKeyEditable() && editableEnabledFlags.isForeignKeyItemsEnabled()){
    return PkFkProxyModelBase::flags(index);
  }
  auto f = PkFkProxyModelBase::flags(index);
  if(!editableEnabledFlags.isForeignKeyEditable()){
    f &= Qt::ItemFlags(~Qt::ItemIsEditable);
  }
  if(!editableEnabledFlags.isForeignKeyItemsEnabled()){
    f &= Qt::ItemFlags(~Qt::ItemIsEnabled);
  }
  return f;

//   const auto items = getItemsActingOnColumn(index.column());
//   if(items.isEmpty()){
//     return PkFkProxyModelBase::flags(index);
//   }
//   bool isEditable = true;
//   bool isItemsEnabled = true;
//   for(const auto item : items){
//     if(!item.isForeignKeyEditable()){
//       isEditable = false;
//     }
//     if(!item.isForeignKeyItemsEnabled()){
//       isItemsEnabled = false;
//     }
//   }
//   if(isEditable && isItemsEnabled){
//     return PkFkProxyModelBase::flags(index);
//   }
//   auto f = PkFkProxyModelBase::flags(index);
//   if(!isEditable){
//     f &= Qt::ItemFlags(~Qt::ItemIsEditable);
//   }
//   if(!isItemsEnabled){
//     f &= Qt::ItemFlags(~Qt::ItemIsEnabled);
//   }
//   return f;
}

ForeignKeyRecord ForeignKeyProxyModel::getForeignKeyRecord(const QString& foreignEntityName, int row) const
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  const auto fk = getForeignKeyReferencing(foreignEntityName);
  Q_ASSERT(!fk.isNull());

  return ForeignKeyRecord::fromKeyRecord( getKeyRecord(row, fk.toColumnList()) );
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

// QVector<ForeignKeyProxyModelMapItem> ForeignKeyProxyModel::getItemsActingOnColumn(int column) const
// {
//   QVector<ForeignKeyProxyModelMapItem> items;
// 
//   for(const auto item : mMap){
//     if(item.foreignKey().containsColumn(column)){
//       items.append(item);
//     }
//   }
// 
//   return items;
// }

}} // namespace Mdt{ namespace ItemModel{
