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
#include "VehicleTypeClassDataValidator.h"

namespace Mdt{ namespace Railway{

bool VehicleTypeClassDataValidator::validateId(VehicleTypeClassId id)
{
  if( !mFieldDataValidator.validateData(id, def().id()) ){
    return false;
  }
  return true;
}

bool VehicleTypeClassDataValidator::validateName(const QString& name)
{
  if( !mFieldDataValidator.validateData(name, def().name()) ){
    return false;
  }
  return true;
}

bool VehicleTypeClassDataValidator::validateAlias(const QString& alias)
{
  if( !mFieldDataValidator.validateData(alias, def().alias()) ){
    return false;
  }
  return true;
}

}} // namespace Mdt{ namespace Railway{
