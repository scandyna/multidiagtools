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
#include "ForeignKeyList.h"

namespace Mdt{ namespace ItemModel{

void ForeignKeyList::addForeignKey(const QString& foreignEntityName, const ForeignKey & fk)
{
  Q_ASSERT(!foreignEntityName.trimmed().isEmpty());
  Q_ASSERT(!fk.isNull());

  mMap.insert(foreignEntityName, fk);
}

ForeignKey ForeignKeyList::getForeignKeyReferencing(const QString & entityName) const
{
  Q_ASSERT(!entityName.trimmed().isEmpty());

  return mMap.value(entityName);
}

}} // namespace Mdt{ namespace ItemModel{
