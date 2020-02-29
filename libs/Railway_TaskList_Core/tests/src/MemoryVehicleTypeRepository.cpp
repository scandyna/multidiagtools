/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "Mdt/Railway/TaskList/VehicleType_p.h"
#include "Mdt/ErrorCode.h"

namespace Mdt{ namespace Railway{ namespace TaskList{

Mdt::Expected<VehicleTypeId> MemoryVehicleTypeRepository::add(const VehicleType & vehicle)
{
  const auto id = mVehicleTypeTable.add(vehicle);

  return VehicleTypeId(id);
}

Mdt::Expected<VehicleType> MemoryVehicleTypeRepository::getById(VehicleTypeId id) const
{
  Q_ASSERT(!id.isNull());

  auto vehicle = mVehicleTypeTable.getByPrimaryKey(id.value());
  if(!vehicle){
    return vehicle;
  }
  privateVehicleTypeDataStruct(*vehicle).id = id.value();

  return *vehicle;
}

Mdt::ExpectedResult MemoryVehicleTypeRepository::update(const VehicleType & vehicle)
{
  Q_ASSERT(!vehicle.id().isNull());

  if(!mVehicleTypeTable.update(vehicle, vehicle.id().value())){
    const auto msg = tr("Updating vehicle type with id '' failed bacause it does not exists")
                     .arg(QString::number(vehicle.id().value()));
    auto error = mdtErrorNewTQ(Mdt::ErrorCode::NotFound, msg, Mdt::Error::Critical, this);
    return error;
  }

  return Mdt::ExpectedResultOk();
}

Mdt::ExpectedResult MemoryVehicleTypeRepository::remove(VehicleTypeId id)
{
  mVehicleTypeTable.remove(id.value());

  return Mdt::ExpectedResultOk();
}

Mdt::ExpectedResult MemoryVehicleTypeRepository::removeAll()
{
  mVehicleTypeTable.removeAll();

  return Mdt::ExpectedResultOk();
}

Mdt::ExpectedResult MemoryVehicleTypeRepository::getAllAsync() const
{
  const auto list = mVehicleTypeTable.getAll();
  for(const auto & vehicle : list){
    emit newVehicleTypeFetched(vehicle);
  }

  return Mdt::ExpectedResultOk();
}

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{
