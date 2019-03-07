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
#include "RemoveVehicleTypeClass.h"

namespace Mdt{ namespace Railway{

RemoveVehicleTypeClass::RemoveVehicleTypeClass(const std::shared_ptr<VehicleTypeClassRepository> & repository, QObject *parent)
 : QObject(parent),
   mRepository(repository)
{
  Q_ASSERT(mRepository.get() != nullptr);
}

bool RemoveVehicleTypeClass::execute(const RemoveVehicleTypeClassRequest & request)
{
  Q_ASSERT(!request.id.isNull());

  if(!mRepository->remove(request.id)){
    emit failed(request.taskId, mRepository->lastError());
    return false;
  }
  RemoveVehicleTypeClassResponse response;
  response.taskId = request.taskId;
  response.id = request.id;
  emit succeeded(response);

  return true;
}

}} // namespace Mdt{ namespace Railway{
