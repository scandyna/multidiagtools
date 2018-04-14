/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "PrimaryKeyFieldIndexList.h"
#include "PrimaryKey.h"
#include "AutoIncrementPrimaryKey.h"
#include "FieldList.h"
#include "Field.h"

namespace Mdt{ namespace Sql{ namespace Schema{

AutoIncrementPrimaryKey PrimaryKeyFieldIndexList::toAutoIncrementPrimaryKey(const FieldList & fieldList) const
{
  Q_ASSERT(count() == 1);
  Q_ASSERT(fieldIndexAt(0) < fieldList.size());

  AutoIncrementPrimaryKey pk;
  const auto field = fieldList.at( fieldIndexAt(0) );
  Q_ASSERT(AutoIncrementPrimaryKey::isValidFieldType(field.type()));
  pk.setFieldName(field.name());
  pk.setFieldType(field.type());

  return pk;
}

PrimaryKey PrimaryKeyFieldIndexList::toPrimaryKey(const FieldList& fieldList) const
{
  Q_ASSERT(!isEmpty());

  PrimaryKey pk;

  for(const auto fieldIndex : mList){
    Q_ASSERT(fieldIndex < fieldList.size());
    pk.addField( fieldList.at(fieldIndex) );
  }

  return pk;
}


}}} // namespace Mdt{ namespace Sql{ namespace Schema{
