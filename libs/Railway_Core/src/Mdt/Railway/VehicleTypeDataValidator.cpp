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
#include "VehicleTypeDataValidator.h"

namespace Mdt{ namespace Railway{

bool VehicleTypeDataValidator::validateVehicleTypeId(VehicleTypeId id)
{
  if( !mFieldDataValidator.validateData(id, def().id()) ){
    setLastError(mFieldDataValidator.toGenericError());
    return false;
  }
  return true;
}

bool VehicleTypeDataValidator::validateVehicleTypeClassId(VehicleTypeClassId id)
{
  if( !mFieldDataValidator.validateData(id.value(), def().vehicleTypeClassId()) ){
    setLastError(mFieldDataValidator.toGenericError());
    return false;
  }
  return true;
}

bool VehicleTypeDataValidator::validateManufacturerSerie(const QString& manufacturerSerie)
{
  if( !mFieldDataValidator.validateData(manufacturerSerie, def().manufacturerSerie()) ){
    setLastError(mFieldDataValidator.toGenericError());
    return false;
  }
  return true;
}

void VehicleTypeDataValidator::setLastError(const Railway::Error& error)
{
  mLastError = error;
}

}} // namespace Mdt{ namespace Railway{
