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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_SQL_REPOSITORY_H
#define MDT_RAILWAY_VEHICLE_TYPE_CLASS_SQL_REPOSITORY_H

#include "Mdt/Railway/VehicleTypeClassRepository.h"
#include "MdtRailway_SqlExport.h"
#include <QSqlDatabase>

namespace Mdt{ namespace Railway{

  /*! \brief SQL implementation of vehicle type class repository
   */
  class MDT_RAILWAY_SQL_EXPORT VehicleTypeClassSqlRepository : public AbstractVehicleTypeClassRepository
  {
   public:

//     /*! \brief Construct a repository on \a dbConnection
//      */
//     VehicleTypeClassSqlRepository();

    /*! \brief Set database connection
     *
     * \pre \a dbConnection must be valid (must have a driver loaded)
     */
    void setDbConnection(const QSqlDatabase & dbConnection);

   private:

    bool fetchRecords(int count) override;
    bool insertRecordToStorage(const VehicleTypeClassData& record, QVariant& autoId) override;
    bool removeRecordFromStorage(int row) override;
    bool updateRecordInStorage(int row) override;

    QSqlDatabase mDbConnection;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_SQL_REPOSITORY_H
