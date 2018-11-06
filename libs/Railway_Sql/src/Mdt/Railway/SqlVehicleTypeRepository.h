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
#ifndef MDT_RAILWAY_SQL_VEHICLE_TYPE_REPOSITORY_H
#define MDT_RAILWAY_SQL_VEHICLE_TYPE_REPOSITORY_H

#include "Mdt/Railway/VehicleTypeRepository.h"

#include "Mdt/Entity/SqlEntityRepository.h"

#include "MdtRailway_SqlExport.h"

namespace Mdt{ namespace Railway{

  /*! \brief SQL implementation of VehicleTypeRepository
   */
  class MDT_RAILWAY_SQL_EXPORT SqlVehicleTypeRepository : public VehicleTypeRepository, public Mdt::Entity::SqlEntityRepository<VehicleTypeData, VehicleTypeId>
  {
   Q_OBJECT

   public:

    
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_SQL_VEHICLE_TYPE_REPOSITORY_H
