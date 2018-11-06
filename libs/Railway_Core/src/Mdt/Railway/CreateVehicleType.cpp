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
#include "Mdt/ErrorCode.h"

namespace Mdt{ namespace Railway{

CreateVehicleType::CreateVehicleType(const std::shared_ptr< VehicleTypeRepository >& repository, QObject* parent)
 : QObject(parent),
   mRepository(repository)
{
  Q_ASSERT(mRepository.get() != nullptr);
}

bool CreateVehicleType::execute(const CreateVehicleTypeRequest & request)
{
  CreateVehicleTypeResponse response;

  if(!checkRequest(request)){
    return false;
  }
  VehicleTypeData vehicleType;
  vehicleType.setManufacturerSerie(request.manufacturerSerie);
  vehicleType.setVehicleTypeClassId(request.vehicleTypeClassId);
  const auto vehicleTypeId = mRepository->add(vehicleType);
  if(!vehicleTypeId){
    buildAndNotifyError(vehicleTypeId.error());
    return false;
  }
  
  response.vehicleTypeId = QString::number( vehicleTypeId.value().value() );
  response.className = request.className;
  response.manufacturerSerie = request.manufacturerSerie;

  emit succeed(response);

  return true;
}

bool CreateVehicleType::checkRequest(const CreateVehicleTypeRequest & request) const
{
  VehicleTypeDataValidator validator;

  if(!validator.validateVehicleTypeClassId(request.vehicleTypeClassId)){
    emit failed(validator.lastError());
    return false;
  }
  if(!validator.validateManufacturerSerie(request.manufacturerSerie)){
    emit failed(validator.lastError());
    return false;
  }
//   if(request.className.trimmed().isEmpty()){
//     const QString msg = tr("Creating vehicle type failed because the vehicle type class is missing.");
//     const auto error = mdtErrorNewQ(msg, Mdt::Error::Warning, this);
//     emit failed(error);
//     return false;
//   }

  return true;
}

void CreateVehicleType::buildAndNotifyError(const Error& error)
{
  if(error.isErrorType<Mdt::ErrorCode>()){
    const auto errorCode = error.error<Mdt::ErrorCode>();
    switch(errorCode){
      case Mdt::ErrorCode::UniqueConstraintError:
        notifyUniqueConstraintError(error);
        return;
      default:
        break;
    }
  }

  emit failed(error);
}

void CreateVehicleType::notifyUniqueConstraintError(const Error& inError)
{
  const auto msg = tr("A vehicle type with given data allready exists");
  auto error = mdtErrorNewQ(msg, Mdt::Error::Warning, this);
  error.stackError(inError);
  emit failed(error);
}

}} // namespace Mdt{ namespace Railway{
