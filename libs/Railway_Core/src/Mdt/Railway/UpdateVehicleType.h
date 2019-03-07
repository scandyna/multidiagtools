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
#ifndef MDT_RAILWAY_UPDATE_VEHICLE_TYPE_H
#define MDT_RAILWAY_UPDATE_VEHICLE_TYPE_H

#include "UpdateVehicleTypeRequest.h"
#include "UpdateVehicleTypeResponse.h"
#include "VehicleTypeId.h"
#include "Mdt/Error.h"
#include "VehicleTypeRepository.h"
#include "MdtRailway_CoreExport.h"
#include <QObject>
#include <QString>
#include <memory>

namespace Mdt{ namespace Railway{

  /*! \brief Update a vehicle type
   */
  class MDT_RAILWAY_CORE_EXPORT UpdateVehicleType : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * \pre \a repository must not be null
     */
    UpdateVehicleType(const std::shared_ptr<VehicleTypeRepository> & repository, QObject *parent = nullptr);

   public slots:

    /*! \brief Execute
     */
    bool execute(const UpdateVehicleTypeRequest & request);

   signals:

    /*! \brief Emitted after successfull creation of the vehicle type
     */
    void succeeded(const UpdateVehicleTypeResponse & response);

    /*! \brief Emitted when the creation of the vehicle type failed
     */
    void failed(int taskId, const Error & error) const;

   private:

    bool checkRequest(const UpdateVehicleTypeRequest & request) const;

    std::shared_ptr<VehicleTypeRepository> mRepository;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_UPDATE_VEHICLE_TYPE_H
