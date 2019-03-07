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
#ifndef MDT_RAILWAY_UPDATE_VEHICLE_TYPE_CLASS_H
#define MDT_RAILWAY_UPDATE_VEHICLE_TYPE_CLASS_H

#include "UpdateVehicleTypeClassRequest.h"
#include "UpdateVehicleTypeClassResponse.h"
#include "VehicleTypeClassId.h"
#include "VehicleTypeClassRepository.h"
#include "Mdt/Error.h"
#include "MdtRailway_CoreExport.h"
#include <QObject>
#include <QString>
#include <memory>

namespace Mdt{ namespace Railway{

  class VehicleTypeClassData;

  /*! \brief Update a vehicle type class
   *
   * \pre All fields in the input data must be set
   * \exception Error if the unique constraint is violated,
   *    or if some infrastructure problem occurs
   * \post The vehicle type class is updated
   */
  class MDT_RAILWAY_CORE_EXPORT UpdateVehicleTypeClass : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * \pre \a repository must not be null
     */
    UpdateVehicleTypeClass(const std::shared_ptr<VehicleTypeClassRepository> & repository, QObject *parent = nullptr);

    /*! \brief Get response
     */
    UpdateVehicleTypeClassResponse response() const
    {
      return mResponse;
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   public slots:

    /*! \brief Execute
     */
    bool execute(const UpdateVehicleTypeClassRequest & request);

   signals:

    /*! \brief Emitted after the vehicle type class was successfully updated
     */
    void succeeded(const UpdateVehicleTypeClassResponse & response);

    /*! \brief Emitted when the update of the vehicle type class failed
     */
    void failed(int transactionId, const Mdt::Error & error) const;
//     void failed(const UpdateVehicleTypeClassRequest & request, const Mdt::Error & error) const;

   private:

    bool checkRequest(const UpdateVehicleTypeClassRequest & request);
    void setResponse(const UpdateVehicleTypeClassResponse & response);
    void buildAndNotifyError(int transactionId, const VehicleTypeClassData & data, const Mdt::Error & error);
//     void setLastError(const UpdateVehicleTypeClassRequest & request, const Mdt::Error & error);

    std::shared_ptr<VehicleTypeClassRepository> mRepository;
    UpdateVehicleTypeClassResponse mResponse;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_UPDATE_VEHICLE_TYPE_CLASS_H
