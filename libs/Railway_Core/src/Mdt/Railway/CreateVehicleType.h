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
#ifndef MDT_RAILWAY_CREATE_VEHICLE_TYPE_H
#define MDT_RAILWAY_CREATE_VEHICLE_TYPE_H

#include "Error.h"
#include "VehicleTypeRepository.h"
#include "MdtRailway_CoreExport.h"
#include <QObject>
#include <QString>
#include <memory>

namespace Mdt{ namespace Railway{

  /*! \brief Request data for CreateVehicleType
   */
  struct MDT_RAILWAY_CORE_EXPORT CreateVehicleTypeRequest
  {
    /*! \brief Vehicle type class
     *
     * This is the vehicle type name,
     *  without taking the variant into account
     */
    QString className;

    /*! \brief Alias
     *
     * The alias is, for example, a project name given to y vehicle type
     *  (for example DOMINO).
     */
    QString alias;

    /*! \brief Manufacturer serie
     */
    QString manufacturerSerie;

    /*! \brief Options
     */
    
  };

  /*! \brief Response data for CreateVehicleType
   */
  struct MDT_RAILWAY_CORE_EXPORT CreateVehicleTypeResponse
  {
    QString vehicleTypeId;
    QString className;
    QString alias;
    QString manufacturerSerie;
  };

  /*! \brief Create a vehicle type
   *
   * Creates a vehicle type, for example, a RBDe 560 DO.
   *
   * There exists various variants of a vehicle type.
   *  For the RBDe 560 DO example:
   *  - Variant with automatic front coupling
   *  - Variant with standard UIC coupling
   *  - Variant with ETCS
   *
   * There also exists various manufacturer series of a vehicle type,
   *  typically serie 1, 2, 3, ..
   *
   * Input data: a instance of CreateVehicleTypeRequest
   *
   * Unique constraint:
   *  the couple className, alias, manufacturerSerie and options
   *  must be unique in the system.
   *
   * \pre All fields in the input data must be set
   * \exception Error if the unique constraint is violated,
   *    or if some infrastructure problem occurs
   * \post The vehicle type is created
   */
  class MDT_RAILWAY_CORE_EXPORT CreateVehicleType : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * \pre \a repository must not be null
     */
    CreateVehicleType(const std::shared_ptr<VehicleTypeRepository> & repository, QObject *parent = nullptr);

   public slots:

    /*! \brief Execute
     */
    bool execute(const CreateVehicleTypeRequest & request);

   signals:

    /*! \brief Emitted after successfull creation of the vehicle type
     */
    void succeed(const CreateVehicleTypeResponse & response);

    /*! \brief Emitted when the creation of the vehicle type failed
     */
    void failed(const Error & error) const;

   private:

    bool checkRequest(const CreateVehicleTypeRequest & request) const;

    std::shared_ptr<VehicleTypeRepository> mRepository;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_CREATE_VEHICLE_TYPE_H
