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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_H
#define MDT_RAILWAY_VEHICLE_TYPE_CLASS_H

#include "VehicleTypeClassRepository.h"
#include "Mdt/Error.h"
#include "MdtRailway_CoreExport.h"
#include <QObject>

namespace Mdt{ namespace Railway{

  /*! \brief
   */
  class MDT_RAILWAY_CORE_EXPORT VehicleTypeClass : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit VehicleTypeClass(QObject* parent = nullptr);

    /*! \brief Set the repository
     */
    void setRepository(const VehicleTypeClassRepositoryHandle & repositoryHandle);

    /*! \brief Validate changes in the cache
     *
     * \note Some constraints, like the unique constraint, are not validated here,
     *   but by submitChanges()
     *
     * \sa submitChanges()
     */
    bool validateChanges();

    /*! \brief Submit changes to the storage
     *
     * Will first call validateChanges()
     */
    bool submitChanges();

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

    void setLastError(const Mdt::Error & error);

    VehicleTypeClassRepositoryHandle mRepositoryHandle;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_H
