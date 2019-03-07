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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_REPOSITORY_H
#define MDT_RAILWAY_VEHICLE_TYPE_REPOSITORY_H

#include "VehicleTypeId.h"
#include "VehicleTypeData.h"
#include "VehicleTypeClassId.h"
#include "VehicleTypeClassData.h"
#include "AbstractEntityRepository.h"
#include "MdtRailway_CoreExport.h"

namespace Mdt{ namespace Railway{

  /*! \brief Vehicle type reprository interface
   */
  class MDT_RAILWAY_CORE_EXPORT VehicleTypeRepository : public virtual AbstractEntityRepository<VehicleTypeData, VehicleTypeId>
  {
   public:

    /*! \brief Get vehicle type class by id
     *
     * \pre \a id must not be null
     */
    virtual Mdt::Expected<VehicleTypeClassData> getVehicleTypeClassById(VehicleTypeClassId id) const = 0;

//    public:
// 
//     /*! \brief Constructor
//      */
//     explicit VehicleTypeRepository(QObject *parent = nullptr);
// 
//     virtual ~VehicleTypeRepository() = default;
// 
//     /*! \brief Add a new vehicle type
//      *
//      * \todo Use Mdt::Expected
//      */
//     virtual Mdt::Expected<VehicleTypeId> add(const VehicleTypeData & data) = 0;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_REPOSITORY_H
