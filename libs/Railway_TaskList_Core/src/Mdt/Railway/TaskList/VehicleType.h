/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_H
#define MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_H

#include "Mdt/Railway/VehicleTypeId.h"
#include "VehicleTypeDataStruct.h"
#include "MdtRailway_TaskList_CoreExport.h"
#include <QString>

namespace Mdt{ namespace Railway{ namespace TaskList{

  /*!\brief Vehicle type
   */
  class MDT_RAILWAY_TASKLIST_CORE_EXPORT VehicleType
  {
   public:

    /*! \brief Construct a vehicle type
     *
     * \pre \a name must not be empty
     * \pre \a alias must not be empty
     * \pre \a manufacturerSerie must not be empty
     */
    explicit VehicleType(const QString & name, const QString & alias, const QString & manufacturerSerie);

    /*! \brief Get id
     */
    VehicleTypeId id() const noexcept
    {
      return VehicleTypeId(mDataStruct.id);
    }

    /*! \brief Set name
     *
     * \pre \a name must not be empty
     */
    void setName(const QString & name);

    /*! \brief Get name
     */
    QString name() const
    {
      return mDataStruct.name;
    }

    /*! \brief Get alias
     */
    QString alias() const
    {
      return mDataStruct.alias;
    }

    /*! \brief Get manufacturer serie
     */
    QString manufacturerSerie() const
    {
      return mDataStruct.manufacturerSerie;
    }

   private:

    VehicleType(const VehicleTypeDataStruct & data)
     : mDataStruct(data)
    {
    }

    friend
    VehicleType vehicleTypeFromDataStruct(const VehicleTypeDataStruct & data);

    friend
    const VehicleTypeDataStruct & privateConstVehicleTypeDataStruct(const VehicleType & vehicleType);

    friend
    VehicleTypeDataStruct & privateVehicleTypeDataStruct(VehicleType & vehicleType);

    VehicleTypeDataStruct mDataStruct;
  };

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

#endif // MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_H
