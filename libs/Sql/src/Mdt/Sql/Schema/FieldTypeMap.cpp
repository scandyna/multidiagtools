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
#include "FieldTypeMap.h"
#include "FieldTypeMapImplementation.h"

namespace Mdt{ namespace Sql{ namespace Schema{

FieldTypeMap::~FieldTypeMap()
{
}

void FieldTypeMap::setDefaultImplementation()
{
  setImplementation<FieldTypeMapImplementation>();
}

FieldType FieldTypeMap::fieldTypeFromQMetaType(QMetaType::Type qmetaType, FieldLength fieldLength) const
{
  Q_ASSERT(isValid());

  return mImpl->fieldTypeFromQMetaType(qmetaType, fieldLength);
}

FieldTypeFlags FieldTypeMap::fieldTypeFlagsFromQMetaType(QMetaType::Type qmetaType) const
{
  Q_ASSERT(isValid());

  return mImpl->fieldTypeFlagsFromQMetaType(qmetaType);
}

QMetaType::Type FieldTypeMap::qmetaTypeFromFieldType(FieldType fieldType, FieldLength fieldLength, FieldTypeFlags fieldTypeFlags) const
{
  Q_ASSERT(isValid());

  return mImpl->qmetaTypeFromFieldType(fieldType, fieldLength, fieldTypeFlags);
}

FieldTypeMap FieldTypeMap::make()
{
  return make<FieldTypeMapImplementation>();
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
