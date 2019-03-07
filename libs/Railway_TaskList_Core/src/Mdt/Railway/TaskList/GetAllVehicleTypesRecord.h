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
#ifndef MDT_RAILWAY_TASK_LIST_GET_ALL_VEHICLE_TYPES_RECORD_H
#define MDT_RAILWAY_TASK_LIST_GET_ALL_VEHICLE_TYPES_RECORD_H

#include "MdtRailway_TaskList_CoreExport.h"
#include <QVariant>

namespace Mdt{ namespace Railway{ namespace TaskList{

  /*!\brief Record returned from GetAllVehicleTypes
   */
  struct MDT_RAILWAY_TASKLIST_CORE_EXPORT GetAllVehicleTypesRecord
  {
    QVariant name;
    QVariant alias;
    QVariant manufacturerSerie;
  };

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

#endif // MDT_RAILWAY_TASK_LIST_GET_ALL_VEHICLE_TYPES_RECORD_H
