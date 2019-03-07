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
#include "MemoryVehicleTypeRepository.h"

namespace Mdt{ namespace Railway{ namespace TaskList{

Mdt::Expected<VehicleTypeId> MemoryVehicleTypeRepository::add(const VehicleType & vehicle)
{
  const auto id = mVehicleTypeTable.add(vehicle);

  return VehicleTypeId(id);
}

Mdt::Expected<VehicleType> MemoryVehicleTypeRepository::getById(VehicleTypeId id) const
{
  Q_ASSERT(!id.isNull());

  return mVehicleTypeTable.getByPrimaryKey(id.value());
}

bool MemoryVehicleTypeRepository::getAllAsync() const
{
  const auto list = mVehicleTypeTable.getAll();
  for(const auto & vehicle : list){
    emit newVehicleTypeFetched(vehicle);
  }

  return true;
}

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{
