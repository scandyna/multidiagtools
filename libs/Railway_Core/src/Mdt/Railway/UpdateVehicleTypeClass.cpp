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
#include "UpdateVehicleTypeClass.h"
#include "VehicleTypeClassData.h"
#include "VehicleTypeClassDataValidator.h"

namespace Mdt{ namespace Railway{

UpdateVehicleTypeClass::UpdateVehicleTypeClass(const std::shared_ptr< VehicleTypeClassRepository >& repository, QObject* parent)
 : QObject(parent),
   mRepository(repository)
{
  Q_ASSERT(mRepository.get() != nullptr);
}

bool UpdateVehicleTypeClass::execute(const UpdateVehicleTypeClassRequest& request)
{
  if(!checkRequest(request)){
    return false;
  }

  VehicleTypeClassData data;
  data.setId(request.id);
  data.setName(request.name);
  data.setAlias(request.alias);
  if(!mRepository->update(data)){
    setLastError(mRepository->lastError());
    return false;
  }

  UpdateVehicleTypeClassResponse response;
  response.id = data.id();
  response.name = data.name();
  response.alias = data.alias();
  setResponse(response);

  return true;
}

bool UpdateVehicleTypeClass::checkRequest(const UpdateVehicleTypeClassRequest& request)
{
  VehicleTypeClassDataValidator validator;

  if(!validator.validateName(request.name)){
    setLastError(validator.lastError());
    return false;
  }
  if(!validator.validateAlias(request.alias)){
    setLastError(validator.lastError());
    return false;
  }

  return true;
}

void UpdateVehicleTypeClass::setResponse(const UpdateVehicleTypeClassResponse& response)
{
  mResponse = response;
  emit succeed(response);
}

void UpdateVehicleTypeClass::setLastError(const Error& error)
{
  mLastError = error;
  emit failed(error);
}

}} // namespace Mdt{ namespace Railway{
