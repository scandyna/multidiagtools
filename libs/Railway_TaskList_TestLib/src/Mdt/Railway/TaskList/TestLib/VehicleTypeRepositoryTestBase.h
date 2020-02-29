/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_RAILWAY_TASK_LIST_TEST_LIB_VEHICLE_TYPE_REPOSITORY_TEST_BASE_H
#define MDT_RAILWAY_TASK_LIST_TEST_LIB_VEHICLE_TYPE_REPOSITORY_TEST_BASE_H

#include "Mdt/Railway/TaskList/VehicleTypeRepository.h"
#include "MdtRailway_TaskList_TestLibExport.h"

namespace Mdt{ namespace Railway{ namespace TaskList{ namespace TestLib{

  /*! \internal
   */
  void MDT_RAILWAY_TASKLIST_CORE_EXPORT addGetUpdateRemoveTest(VehicleTypeRepository & repository);

  /*! \internal
   */
  void MDT_RAILWAY_TASKLIST_CORE_EXPORT getAllAsyncTest(VehicleTypeRepository & repository);

  /*! \internal
   */
  void MDT_RAILWAY_TASKLIST_CORE_EXPORT getAllAsyncTestBuildDataSet();

}}}} // namespace Mdt{ namespace Railway{ namespace TaskList{ namespace TestLib{

#endif // #ifnef MDT_RAILWAY_TASK_LIST_TEST_LIB_VEHICLE_TYPE_REPOSITORY_TEST_BASE_H
