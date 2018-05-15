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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_DATA_LIST_H
#define MDT_RAILWAY_VEHICLE_TYPE_CLASS_DATA_LIST_H

#include "VehicleTypeClassData.h"
#include "Mdt/Entity/DataList.h"
#include "MdtRailway_CoreExport.h"
#include <QString>

namespace Mdt{ namespace Railway{

  /*! \brief List of vehicle type class
   */
  class MDT_RAILWAY_CORE_EXPORT VehicleTypeClassDataList : public Mdt::Entity::DataList<VehicleTypeClassData>
  {
   public:

    /*! \brief Get the row for a given vehicle type class name
     *
     * Returns the row of given name or -1 if not found.
     */
    int rowOfVehicleTypeName(const QString & name) const;

    /*! \brief Check if this list contains a vehicle type class designed by \a name
     */
    bool containsVehicleTypeName(const QString & name) const
    {
      return (rowOfVehicleTypeName(name) >= 0);
    }

    /*! \brief Compare vehicle type names
     */
    static bool compareVehicleTypeName(const QString & a, const QString & b);
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_DATA_LIST_H
