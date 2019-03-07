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
#include "UpdateVehicleType.h"
#include "VehicleTypeDataValidator.h"
#include "VehicleTypeData.h"

namespace Mdt{ namespace Railway{

UpdateVehicleType::UpdateVehicleType(const std::shared_ptr<VehicleTypeRepository> & repository, QObject *parent)
 : QObject(parent),
   mRepository(repository)
{
  Q_ASSERT(mRepository.get() != nullptr);
}

bool UpdateVehicleType::execute(const UpdateVehicleTypeRequest & request)
{
  UpdateVehicleTypeResponse response;

  response.taskId = request.taskId;

  if(!checkRequest(request)){
    return false;
  }
  VehicleTypeData vehicleType;
  vehicleType.setId(request.vehicleTypeId);
  vehicleType.setVehicleTypeClassId(request.vehicleTypeClassId);
  vehicleType.setManufacturerSerie(request.manufacturerSerie);
  if(!mRepository->update(vehicleType)){
    emit failed(request.taskId, mRepository->lastError());
    return false;
  }
  response.vehicleTypeId = request.vehicleTypeId;
  response.vehicleTypeClassId = request.vehicleTypeClassId;
  response.manufacturerSerie = request.manufacturerSerie;

  emit succeeded(response);

  return true;
}

bool UpdateVehicleType::checkRequest(const UpdateVehicleTypeRequest & request) const
{
  VehicleTypeDataValidator validator;

  if(!validator.validateVehicleTypeId(request.vehicleTypeId)){
    emit failed(request.taskId, validator.lastError());
    return false;
  }
  if(!validator.validateVehicleTypeClassId(request.vehicleTypeClassId)){
    emit failed(request.taskId, validator.lastError());
    return false;
  }
  if(!validator.validateManufacturerSerie(request.manufacturerSerie)){
    emit failed(request.taskId, validator.lastError());
    return false;
  }

  return true;
}

}} // namespace Mdt{ namespace Railway{
