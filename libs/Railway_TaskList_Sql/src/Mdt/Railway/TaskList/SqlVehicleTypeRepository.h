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
#ifndef MDT_RAILWAY_TASK_LIST_SQL_VEHICLE_TYPE_REPOSITORY_H
#define MDT_RAILWAY_TASK_LIST_SQL_VEHICLE_TYPE_REPOSITORY_H

#include "Mdt/Railway/TaskList/VehicleTypeRepository.h"
#include "Mdt/Railway/TaskList/VehicleType.h"
#include "MdtRailway_TaskList_SqlExport.h"
#include <QSqlDatabase>

namespace Mdt{ namespace Railway{ namespace TaskList{

  /*!\brief SQL vehicle type repository
   */
  class MDT_RAILWAY_TASKLIST_SQL_EXPORT SqlVehicleTypeRepository : public Mdt::Railway::TaskList::VehicleTypeRepository
  {
   public:

    /*! \brief Set database connection
     *
     * \pre \a db must be valid (must have a driver loaded)
     */
    void setDatabase(const QSqlDatabase & db);

    /*! \brief Add a vehicle type
     */
    Mdt::Expected<VehicleTypeId> add(const VehicleType & vehicle) override;

    /*! \brief Get a vehicle type by its id
     *
     * \pre \a id must not be null
     */
    Mdt::Expected<VehicleType> getById(VehicleTypeId id) const override;

    /*! \brief Fetch all vehicle types in a asynchronous way
     *
     * Each time a vehicle type was fetched,
     *  newVehicleTypeFetched() will be emitted.
     */
    bool getAllAsync() const override;

   private:

    QSqlDatabase mDatabase;
  };

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

#endif // MDT_RAILWAY_TASK_LIST_SQL_VEHICLE_TYPE_REPOSITORY_H
