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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_MODEL_H
#define MDT_RAILWAY_VEHICLE_TYPE_MODEL_H

#include "VehicleTypeData.h"
#include "Mdt/Entity/RowResizableTableModel.h"
#include "MdtRailway_CoreExport.h"
#include <vector>

namespace Mdt{ namespace Railway{

  /*! \brief Table model for vehicle type
   */
  class MDT_RAILWAY_CORE_EXPORT VehicleTypeModel : public Mdt::Entity::RowResizableTableModel< std::vector<VehicleTypeData> >
  {
   Q_OBJECT

   public:

    using ParentClass = Mdt::Entity::RowResizableTableModel< std::vector<VehicleTypeData> >;
    using ParentClass::ParentClass;

   public slots:

    /*! \brief Add a vehicle type
     */
    void addVehicleType();
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_MODEL_H
