/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "VehicleTypeClassData.h"
#include "Mdt/Entity/FieldAttributes.h"

namespace Mdt{ namespace Railway{

void VehicleTypeClassData::setName(const QString& name)
{
  Q_ASSERT( !name.trimmed().isEmpty() );
  Q_ASSERT( name.trimmed().length() <= def().name().fieldAttributes().maxLength() );

  dataStruct().name = name.trimmed();
}

void VehicleTypeClassData::setAlias(const QString& alias)
{
//   Q_ASSERT( !alias.trimmed().isEmpty() );
  Q_ASSERT( alias.trimmed().length() <= def().alias().fieldAttributes().maxLength() );

  dataStruct().alias = alias.trimmed();
}

}} // namespace Mdt{ namespace Railway{
