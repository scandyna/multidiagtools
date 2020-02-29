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
#ifndef MDT_RAILWAY_TASK_LIST_GET_ALL_VEHICLE_TYPES_H
#define MDT_RAILWAY_TASK_LIST_GET_ALL_VEHICLE_TYPES_H

#include "GetAllVehicleTypesRecord.h"
#include "GetAllVehicleTypesError.h"
#include "VehicleTypeRepository.h"
#include "VehicleType.h"
#include "MdtRailway_TaskList_CoreExport.h"
#include <QObject>
#include <memory>

namespace Mdt{ namespace Railway{ namespace TaskList{

  /*!\brief Get a list of all existing vehicle types
   */
  class MDT_RAILWAY_TASKLIST_CORE_EXPORT GetAllVehicleTypes : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    GetAllVehicleTypes(const std::shared_ptr<VehicleTypeRepository> & repository, QObject *parent = nullptr);

   public slots:

    /*! \brief Execute
     */
    bool execute();

   signals:

    /*! \brief Emitted each time a new record was fetched
     */
    void newRecordFetched(const GetAllVehicleTypesRecord & record);

    /*! \brief Emitted on failure
     */
    void failed(const GetAllVehicleTypesError & error);

   private slots:

    void onNewVehicleTypeFetched(const VehicleType & vehicle);

   private:

    std::shared_ptr<VehicleTypeRepository> mRepository;
  };

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{

#endif // MDT_RAILWAY_TASK_LIST_GET_ALL_VEHICLE_TYPES_H
