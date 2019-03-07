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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_COMMON_ERROR_H
#define MDT_RAILWAY_VEHICLE_TYPE_CLASS_COMMON_ERROR_H

#include "VehicleTypeClassData.h"
#include "Mdt/Error.h"
#include "MdtRailway_CoreExport.h"
#include <QObject>

namespace Mdt{ namespace Railway{

  /*! \brief Common errors for vehicle type class
   */
  class MDT_RAILWAY_CORE_EXPORT VehicleTypeClassCommonError : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Check if \a error is known
     */
    bool isKnownError(const Mdt::Error & error) const noexcept;

    /*! \brief Create a common error
     *
     * \pre \a error must be a known error
     */
    Mdt::Error createError(const VehicleTypeClassData & data, const Mdt::Error & error);

   private:

    Mdt::Error createUniqueConstraintError(const VehicleTypeClassData & data, const Mdt::Error & error);
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_COMMON_ERROR_H
