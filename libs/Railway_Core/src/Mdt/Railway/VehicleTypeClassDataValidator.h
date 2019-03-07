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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_DATA_VALIDATOR_H
#define MDT_RAILWAY_VEHICLE_TYPE_CLASS_DATA_VALIDATOR_H

#include "VehicleTypeClassId.h"
#include "Entity/VehicleTypeClass.h"
#include "Mdt/Entity/FieldDataValidator.h"
#include "Mdt/Error.h"
#include "MdtRailway_CoreExport.h"
#include <QObject>
#include <QString>

namespace Mdt{ namespace Railway{

  /*! \brief Validate vehicle type class data
   */
  class MDT_RAILWAY_CORE_EXPORT VehicleTypeClassDataValidator : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Validate vehicle type class id
     */
    bool validateId(VehicleTypeClassId id);

    /*! \brief Validate vehicle type class name
     */
    bool validateName(const QString & name);

    /*! \brief Validate vehicle type class alias
     */
    bool validateAlias(const QString & alias);

    /// \todo In use cases, maybe validate data could be good

    /*! \brief Get last state
     */
    Mdt::Entity::FieldDataValidatorState state() const
    {
      return mFieldDataValidator.state();
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mFieldDataValidator.toGenericError();
    }

   private:

    using def_type = Entity::VehicleTypeClassEntity::def_type;

    static def_type def()
    {
      return Entity::VehicleTypeClassEntity::def();
    }

    Mdt::Entity::FieldDataValidator<> mFieldDataValidator;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_DATA_VALIDATOR_H
