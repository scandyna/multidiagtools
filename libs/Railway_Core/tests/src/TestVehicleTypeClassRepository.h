/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef TEST_VEHICLE_TYPE_CLASS_REPOSITORY_H
#define TEST_VEHICLE_TYPE_CLASS_REPOSITORY_H

#include "Mdt/Railway/VehicleTypeClassData.h"
#include "Mdt/Railway/VehicleTypeClassRepository.h"
#include "Mdt/Entity/MemoryEntityRepository.h"

class TestVehicleTypeClassRepository : public Mdt::Railway::VehicleTypeClassRepository,
                                       public Mdt::Entity::MemoryEntityRepository<Mdt::Railway::VehicleTypeClassData, Mdt::Railway::VehicleTypeClassId>
{
};

#endif // #ifndef TEST_VEHICLE_TYPE_CLASS_REPOSITORY_H
