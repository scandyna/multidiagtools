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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_DATA_H
#define MDT_RAILWAY_VEHICLE_TYPE_CLASS_DATA_H

#include "Entity/VehicleTypeClass.h"
#include "VehicleTypeClassId.h"
#include "Mdt/Entity/DataTemplate.h"
#include <QString>

namespace Mdt{ namespace Railway{

  class MDT_RAILWAY_CORE_EXPORT VehicleTypeClassData : public Mdt::Entity::DataTemplate<Entity::VehicleTypeClassEntity>
  {
   public:

    /*! \brief Set id
     *
     * \pre id must not be null
     */
    void setId(VehicleTypeClassId id)
    {
      Q_ASSERT(!id.isNull());
      dataStruct().id = id.value();
    }

    /*! \brief Get id
     */
    VehicleTypeClassId id() const
    {
      return VehicleTypeClassId( constDataStruct().id );
    }

    /*! \brief Set name
     *
     * \pre \a name must not be a empty string
     * \pre \a name must not be longer than defined in the entity
     */
    void setName(const QString & name);

    /*! \brief Get name
     */
    QString name() const
    {
      return constDataStruct().name;
    }

    /*! \brief Set alias
     *
     * \pre \a alias must not be a empty string
     * \pre \a alias must not be longer than defined in the entity
     */
    void setAlias(const QString & alias);

    /*! \brief Get alias
     */
    QString alias() const
    {
      return constDataStruct().alias;
    }

  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_DATA_H
