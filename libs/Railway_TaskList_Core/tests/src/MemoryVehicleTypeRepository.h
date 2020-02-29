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
#ifndef MDT_RAILWAY_TASK_LIST_MEMORY_VEHICLE_TYPE_REPOSITORY_H
#define MDT_RAILWAY_TASK_LIST_MEMORY_VEHICLE_TYPE_REPOSITORY_H

#include "Mdt/Railway/TaskList/VehicleTypeRepository.h"
#include "Mdt/Railway/TaskList/VehicleType.h"
#include "MdtRailway_TaskList_CoreExport.h"
#include "Mdt/MemoryStore/TableWithAutoId.h"

namespace Mdt{ namespace Railway{ namespace TaskList{

  /*!\brief Memory vehicle type repository for tests
   */
  class MDT_RAILWAY_TASKLIST_CORE_EXPORT MemoryVehicleTypeRepository : public Mdt::Railway::TaskList::VehicleTypeRepository
  {
   public:

    /*! \override
     */
    Mdt::Expected<VehicleTypeId> add(const VehicleType & vehicle) override;

    /*! \override
     */
    Mdt::Expected<VehicleType> getById(VehicleTypeId id) const override;

    /*! \override
     */
    Mdt::ExpectedResult update(const VehicleType & vehicle) override;

    /*! \override
     */
    Mdt::ExpectedResult remove(VehicleTypeId id) override;

    /*! \override
     */
    Mdt::ExpectedResult removeAll() override;

    /*! \override
     */
    Mdt::ExpectedResult getAllAsync() const override;

   private:

    Mdt::MemoryStore::TableWithAutoId<VehicleType> mVehicleTypeTable;
  };

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

#endif // MDT_RAILWAY_TASK_LIST_MEMORY_VEHICLE_TYPE_REPOSITORY_H
