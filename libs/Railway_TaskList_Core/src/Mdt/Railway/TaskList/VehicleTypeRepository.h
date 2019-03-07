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
#ifndef MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_REPOSITORY_H
#define MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_REPOSITORY_H

#include "VehicleType.h"
#include "Mdt/Railway/VehicleTypeId.h"
#include "Mdt/Expected.h"
#include "MdtRailway_TaskList_CoreExport.h"
#include <QObject>

namespace Mdt{ namespace Railway{ namespace TaskList{

  /*!\brief Vehicle type repository interface
   */
  class MDT_RAILWAY_TASKLIST_CORE_EXPORT VehicleTypeRepository : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    VehicleTypeRepository(QObject *parent = nullptr);

    /*! \brief Add a vehicle type
     */
    virtual Mdt::Expected<VehicleTypeId> add(const VehicleType & vehicle) = 0;

    /*! \brief Get a vehicle type by its id
     *
     * \pre \a id must not be null
     */
    virtual Mdt::Expected<VehicleType> getById(VehicleTypeId id) const = 0;

    /*! \brief Fetch all vehicle types in a asynchronous way
     *
     * Each time a vehicle type was fetched,
     *  newVehicleTypeFetched() will be emitted.
     */
    virtual bool getAllAsync() const = 0;

   signals:

    /*! \brief Emitted each time a vehicle type was fetched
     *
     * This signal is emitted when using getAllAsync()
     */
    void newVehicleTypeFetched(const VehicleType & vehicle) const;
  };

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

#endif // MDT_RAILWAY_TASK_LIST_VEHICLE_TYPE_REPOSITORY_H
