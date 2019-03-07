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
#include "CreateVehicleType.h"
#include "VehicleTypeData.h"
#include "VehicleTypeDataValidator.h"
#include "VehicleTypeClassData.h"
#include "Mdt/ErrorCode.h"

namespace Mdt{ namespace Railway{

CreateVehicleType::CreateVehicleType(const std::shared_ptr<VehicleTypeRepository> & repository, QObject* parent)
 : QObject(parent),
   mRepository(repository)
{
  Q_ASSERT(mRepository.get() != nullptr);
}

bool CreateVehicleType::execute(const CreateVehicleTypeRequest & request)
{
  CreateVehicleTypeResponse response;

  response.transactionId = request.transactionId;

  if(!checkRequest(request)){
    return false;
  }

  VehicleTypeData vehicleType;
  vehicleType.setManufacturerSerie(request.manufacturerSerie);
  vehicleType.setVehicleTypeClassId(request.vehicleTypeClassId);
  const auto vehicleTypeId = mRepository->add(vehicleType);
  if(!vehicleTypeId){
    buildAndNotifyError(request, vehicleTypeId.error());
    return false;
  }

  const auto vehicleTypeClass = mRepository->getVehicleTypeClassById(request.vehicleTypeClassId);
  if(!vehicleTypeClass){
    buildAndNotifyError(request, vehicleTypeClass.error());
    return false;
  }

  response.vehicleTypeId = *vehicleTypeId;
  response.vehicleTypeClassId = vehicleType.vehicleTypeClassId();
  response.manufacturerSerie = vehicleType.manufacturerSerie();
  response.vehicleTypeClassName = vehicleTypeClass->name();
  response.vehicleTypeClassAlias = vehicleTypeClass->alias();

  emit succeeded(response);

  return true;
}

bool CreateVehicleType::checkRequest(const CreateVehicleTypeRequest & request) const
{
  VehicleTypeDataValidator validator;

  if(!validator.validateVehicleTypeClassId(request.vehicleTypeClassId)){
    emit failed(request.transactionId, validator.lastError());
    return false;
  }
  if(!validator.validateManufacturerSerie(request.manufacturerSerie)){
    emit failed(request.transactionId, validator.lastError());
    return false;
  }

  return true;
}

void CreateVehicleType::buildAndNotifyError(const CreateVehicleTypeRequest & request, const Error& error)
{
  if(error.isErrorType<Mdt::ErrorCode>()){
    const auto errorCode = error.error<Mdt::ErrorCode>();
    switch(errorCode){
      case Mdt::ErrorCode::UniqueConstraintError:
        notifyUniqueConstraintError(request, error);
        return;
      default:
        break;
    }
  }

  emit failed(request.transactionId, error);
}

void CreateVehicleType::notifyUniqueConstraintError(const CreateVehicleTypeRequest & request, const Error& inError)
{
  const auto msg = tr("A vehicle type with given data allready exists");
  auto error = mdtErrorNewQ(msg, Mdt::Error::Warning, this);
  error.stackError(inError);
  emit failed(request.transactionId, error);
}

}} // namespace Mdt{ namespace Railway{
