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
#ifndef MDT_RAILWAY_REMOVE_VEHICLE_TYPE_CLASS_H
#define MDT_RAILWAY_REMOVE_VEHICLE_TYPE_CLASS_H

#include "RemoveVehicleTypeClassRequest.h"
#include "RemoveVehicleTypeClassResponse.h"
#include "VehicleTypeClassId.h"
#include "VehicleTypeClassRepository.h"
#include "Mdt/Error.h"
#include "MdtRailway_CoreExport.h"
#include <QObject>
#include <QString>
#include <memory>

namespace Mdt{ namespace Railway{

  /*! \brief Remove a vehicle type class
   */
  class MDT_RAILWAY_CORE_EXPORT RemoveVehicleTypeClass : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * \pre \a repository must not be null
     */
    RemoveVehicleTypeClass(const std::shared_ptr<VehicleTypeClassRepository> & repository, QObject *parent = nullptr);

   public slots:

    /*! \brief Execute
     */
    bool execute(const RemoveVehicleTypeClassRequest & request);

   signals:

    /*! \brief Emitted after the vehicle type class was successfully updated
     */
    void succeeded(const RemoveVehicleTypeClassResponse & response);

    /*! \brief Emitted when the update of the vehicle type class failed
     */
    void failed(int taskId, const Mdt::Error & error) const;

   private:

    std::shared_ptr<VehicleTypeClassRepository> mRepository;
  };

}} // namespace Mdt{ namespace Railway{

#endif // MDT_RAILWAY_REMOVE_VEHICLE_TYPE_CLASS_H
