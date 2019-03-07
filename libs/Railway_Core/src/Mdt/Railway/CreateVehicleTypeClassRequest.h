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
#ifndef MDT_RAILWAY_CREATE_VEHICLE_TYPE_CLASS_REQUEST_H
#define MDT_RAILWAY_CREATE_VEHICLE_TYPE_CLASS_REQUEST_H

#include "MdtRailway_CoreExport.h"
#include <QString>

namespace Mdt{ namespace Railway{

  /*! \brief Request data for CreateVehicleTypeClass
   */
  struct MDT_RAILWAY_CORE_EXPORT CreateVehicleTypeClassRequest
  {
    QString name;
    QString alias;
    int transactionId = 0;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_CREATE_VEHICLE_TYPE_CLASS_REQUEST_H
