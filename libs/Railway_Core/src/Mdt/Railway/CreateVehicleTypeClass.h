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
#ifndef MDT_RAILWAY_CREATE_VEHICLE_TYPE_CLASS_H
#define MDT_RAILWAY_CREATE_VEHICLE_TYPE_CLASS_H

#include "VehicleTypeClassId.h"
#include "VehicleTypeClassRepository.h"
#include "Mdt/Error.h"
#include "MdtRailway_CoreExport.h"
#include <QObject>
#include <QString>
#include <memory>

namespace Mdt{ namespace Railway{

  /*! \brief Request data for CreateVehicleTypeClass
   */
  struct MDT_RAILWAY_CORE_EXPORT CreateVehicleTypeClassRequest
  {
    QString name;
    QString alias;
  };

  /*! \brief Response data for CreateVehicleTypeClass
   */
  struct MDT_RAILWAY_CORE_EXPORT CreateVehicleTypeClassResponse
  {
    VehicleTypeClassId id;
    QString name;
    QString alias;
  };

  /*! \brief Create a vehicle type class
   *
   * \pre All fields in the input data must be set
   * \exception Error if the unique constraint is violated,
   *    or if some infrastructure problem occurs
   * \post The vehicle type is created
   */
  class MDT_RAILWAY_CORE_EXPORT CreateVehicleTypeClass : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * \pre \a repository must not be null
     */
    CreateVehicleTypeClass(const std::shared_ptr<VehicleTypeClassRepository> & repository, QObject *parent = nullptr);

    /*! \brief Get response
     */
    CreateVehicleTypeClassResponse response() const
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
    bool execute(const CreateVehicleTypeClassRequest & request);

   signals:

    /*! \brief Emitted after successfull creation of the vehicle type class
     */
    void succeed(const CreateVehicleTypeClassResponse & response);

    /*! \brief Emitted when the creation of the vehicle type class failed
     */
    void failed(const Mdt::Error & error) const;

   private:

    bool checkRequest(const CreateVehicleTypeClassRequest & request);
    void setResponse(const CreateVehicleTypeClassResponse & response);
    void setLastError(const Mdt::Error & error);

    std::shared_ptr<VehicleTypeClassRepository> mRepository;
    CreateVehicleTypeClassResponse mResponse;
    Mdt::Error mLastError;
  };

//   /*! \brief Recieve a response from CreateVehicleTypeClass
//    */
//   class CreateVehicleTypeClassResponseReciever : public QObject
//   {
//    Q_OBJECT
// 
//    public:
// 
//     /*! \brief Constructor
//      */
//     CreateVehicleTypeClassResponseReciever(CreateVehicleTypeClass *cvtc, QObject *parent = nullptr);
// 
//     /*! \brief Check a error was set
//      */
//     bool hasError() const
//     {
//       return !mError.isNull();
//     }
// 
//     /*! \brief Get response
//      */
//     CreateVehicleTypeClassResponse response() const
//     {
//       return mResponse;
//     }
// 
//     /*! \brief Get error
//      */
//     Mdt::Error error() const
//     {
//       return mError;
//     }
// 
//    public slots:
// 
//     void setResponse(const CreateVehicleTypeClassResponse & response)
//     {
//       mResponse = response;
//     }
// 
//     void setError(const Mdt::Error & error)
//     {
//       mError = error;
//     }
// 
//    private:
// 
//     CreateVehicleTypeClassResponse mResponse;
//     Mdt::Error mError;
//   };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_CREATE_VEHICLE_TYPE_CLASS_H
