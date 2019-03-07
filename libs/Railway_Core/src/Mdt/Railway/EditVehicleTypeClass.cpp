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
#include "EditVehicleTypeClass.h"

namespace Mdt{ namespace Railway{

bool EditVehicleTypeClass::setName(const QString& name)
{
  if( !mFieldDataValidator.validateData(name, mData.def().name()) ){
    mLastError = mFieldDataValidator.toGenericError();
    return false;
  }
  mData.setName(name);

  return true;
}

bool EditVehicleTypeClass::setAlias(const QString& alias)
{
  if( !mFieldDataValidator.validateData(alias, mData.def().alias()) ){
    mLastError = mFieldDataValidator.toGenericError();
    return false;
  }
  mData.setAlias(alias);

  return true;
}


// void EditVehicleTypeClass::setData(const VehicleTypeClassData& data)
// {
//   mData = data;
// }

bool EditVehicleTypeClass::validate()
{
  if( !mFieldDataValidator.validateData(mData.name(), mData.def().name()) ){
    mLastError = mFieldDataValidator.toGenericError();
    return false;
  }
  if( !mFieldDataValidator.validateData(mData.alias(), mData.def().alias()) ){
    mLastError = mFieldDataValidator.toGenericError();
    return false;
  }

  return true;
}

}} // namespace Mdt{ namespace Railway{
