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
#include "CreateVehicleTypeClass.h"
#include "VehicleTypeClassData.h"
#include "VehicleTypeClassDataValidator.h"

namespace Mdt{ namespace Railway{

CreateVehicleTypeClass::CreateVehicleTypeClass(const std::shared_ptr<VehicleTypeClassRepository> & repository, QObject* parent)
 : QObject(parent),
   mRepository(repository)
{
  Q_ASSERT(repository.get() != nullptr);
}

bool CreateVehicleTypeClass::execute(const CreateVehicleTypeClassRequest& request)
{
  if(!checkRequest(request)){
    return false;
  }

  VehicleTypeClassData data;
  data.setName(request.name);
  data.setAlias(request.alias);
  const auto id = mRepository->add(data);
  if(!id){
    setLastError(id.error());
    return false;
  }

  CreateVehicleTypeClassResponse response;
  response.id = *id;
  response.name = data.name();
  response.alias = data.alias();
  setResponse(response);

  return true;
}

bool CreateVehicleTypeClass::checkRequest(const CreateVehicleTypeClassRequest& request)
{
  VehicleTypeClassDataValidator validator;

  if(!validator.validateName(request.name)){
    setLastError(validator.lastError());
    return false;
  }

  return true;
}

void CreateVehicleTypeClass::setResponse(const CreateVehicleTypeClassResponse& response)
{
  mResponse = response;
  emit succeed(response);
}

void CreateVehicleTypeClass::setLastError(const Error& error)
{
  mLastError = error;
  emit failed(error);
}


// CreateVehicleTypeClassResponseReciever::CreateVehicleTypeClassResponseReciever(CreateVehicleTypeClass* cvtc, QObject *parent)
//  : QObject(parent)
// {
//   Q_ASSERT(cvtc != nullptr);
// 
//   connect(cvtc, &CreateVehicleTypeClass::succeed, this, &CreateVehicleTypeClassResponseReciever::setResponse);
//   connect(cvtc, &CreateVehicleTypeClass::failed, this, &CreateVehicleTypeClassResponseReciever::setError);
// }

}} // namespace Mdt{ namespace Railway{
