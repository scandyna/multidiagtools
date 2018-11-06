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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_DATA_H
#define MDT_RAILWAY_VEHICLE_TYPE_DATA_H

#include "VehicleTypeId.h"
#include "VehicleTypeClassId.h"
#include "Entity/VehicleType.h"
#include "Mdt/Entity/DataTemplate.h"
#include <QString>

namespace Mdt{ namespace Railway{

  /*! \brief Vehicle type entity data
   */
  class MDT_RAILWAY_CORE_EXPORT VehicleTypeData : public Mdt::Entity::DataTemplate<Entity::VehicleTypeEntity>
  {
   public:

    /*! \brief Set id
     *
     * \pre \a id must not be null
     */
    void setId(VehicleTypeId id)
    {
      Q_ASSERT(!id.isNull());
      dataStruct().id = id.value();
    }

    /*! \brief Get id
     */
    VehicleTypeId id() const
    {
      return constDataStruct().id;
    }

    /*! \brief Set vehicle type class id
     *
     * \pre \a id must not be null
     */
    void setVehicleTypeClassId(VehicleTypeClassId id)
    {
      Q_ASSERT(!id.isNull());
      dataStruct().vehicleTypeClassId = id.value();
    }

    /*! \brief Get vehicle type class id
     */
    VehicleTypeClassId vehicleTypeClassId() const noexcept
    {
      return constDataStruct().vehicleTypeClassId;
    }

    /*! \brief Set manufacturer serie
     *
     * \pre \a serie must not be a empty string
     */
    void setManufacturerSerie(const QString & serie);

    /*! \brief Get manufacturer serie
     */
    QString manufacturerSerie() const
    {
      return constDataStruct().manufacturerSerie;
    }

  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_DATA_H
