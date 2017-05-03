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
#include "ForeignKeyProxyModelMap.h"

namespace Mdt{ namespace ItemModel{

void ForeignKeyProxyModelMap::addForeignKey(const QString& foreignEntityName, const ForeignKey& fk)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  Q_ASSERT(!mMap.contains(foreignEntityName));
  Q_ASSERT(!fk.isNull());

  mMap.insert( foreignEntityName, ForeignKeyProxyModelMapItem(fk) );
}

void ForeignKeyProxyModelMap::removeForeignKey(const QString& foreignEntityName)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());

  mMap.remove(foreignEntityName);
}

bool ForeignKeyProxyModelMap::hasForeignKeyReferencing(const QString& entityName) const
{
  Q_ASSERT(!entityName.trimmed().isEmpty());

  return mMap.contains(entityName);
}

ForeignKey ForeignKeyProxyModelMap::getForeignKeyReferencing(const QString& entityName) const
{
  Q_ASSERT(!entityName.trimmed().isEmpty());

  return mMap.value(entityName).foreignKey();
}

void ForeignKeyProxyModelMap::setForeignKeyEditable(const QString& foreignEntityName, bool editable)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  Q_ASSERT(mMap.contains(foreignEntityName));

  mMap[foreignEntityName].setForeignKeyEditable(editable);
}

void ForeignKeyProxyModelMap::setAllForeignKeysEditable(bool editable)
{
  for(auto & item : mMap){
    item.setForeignKeyEditable(editable);
  }
}

bool ForeignKeyProxyModelMap::isForeignKeyEditable(const QString& foreignEntityName) const
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  Q_ASSERT(mMap.contains(foreignEntityName));

  return mMap.value(foreignEntityName).isForeignKeyEditable();
}

void ForeignKeyProxyModelMap::setForeignKeyItemsEnabled(const QString& foreignEntityName, bool enable)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  Q_ASSERT(mMap.contains(foreignEntityName));

  mMap[foreignEntityName].setForeignKeyItemsEnabled(enable);
}

void ForeignKeyProxyModelMap::setAllForeignKeysItemsEnabled(bool enable)
{
  for(auto & item : mMap){
    item.setForeignKeyItemsEnabled(enable);
  }
}

bool ForeignKeyProxyModelMap::isForeignKeyItemsEnabled(const QString& foreignEntityName) const
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  Q_ASSERT(mMap.contains(foreignEntityName));

  return mMap.value(foreignEntityName).isForeignKeyItemsEnabled();
}

ForeignKeyProxyModelMapItemFlags ForeignKeyProxyModelMap::getMostRestrictiveFlagsForColumn(int column) const
{
  Q_ASSERT(column >= 0);

  ForeignKeyProxyModelMapItemFlags flags;

  for(const auto & item : mMap){
    if(!item.isForeignKeyEditable()){
      flags.setForeignKeyEditable(false);
    }
    if(!item.isForeignKeyItemsEnabled()){
      flags.setForeignKeyItemsEnabled(false);
    }
  }

  return flags;
}

}} // namespace Mdt{ namespace ItemModel{
