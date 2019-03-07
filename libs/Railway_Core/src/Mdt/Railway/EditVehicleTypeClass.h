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
#ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_H
#define MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_H

#include "VehicleTypeClassData.h"
#include "Mdt/Entity/FieldDataValidator.h"
#include "Mdt/Error.h"
#include "MdtRailway_CoreExport.h"
#include <QObject>

namespace Mdt{ namespace Railway{

  /*! \brief Edit vehicle type class
   */
  class MDT_RAILWAY_CORE_EXPORT EditVehicleTypeClass : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Set name
     */
    bool setName(const QString & name);

    /*! \brief Get name
     */
    QString name() const
    {
      return mData.name();
    }

    /*! \brief Set alias
     */
    bool setAlias(const QString & alias);

    /*! \brief Get alias
     */
    QString alias() const
    {
      return mData.alias();
    }


//     /*! \brief Set data
//      */
//     void setData(const VehicleTypeClassData & data);
// 
//     /*! \brief Get data
//      */
//     VehicleTypeClassData data() const
//     {
//       return mData;
//     }

    /*! \brief Validate all attributes of this vehicle type class
     *
     * \note Unique constraint are not checked here
     */
    bool validate();

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

    VehicleTypeClassData mData;
    Mdt::Entity::FieldDataValidator<> mFieldDataValidator;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_H
