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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_EDITION_H
#define MDT_RAILWAY_VEHICLE_TYPE_EDITION_H

#include "VehicleTypeClassData.h"
#include "VehicleTypeClassTableModel.h"
#include "VehicleTypeData.h"
#include "MdtRailway_CoreExport.h"
#include "Mdt/Entity/FieldDataValidator.h"
#include "Mdt/ItemModel/ProxyModelContainer.h"
#include "Mdt/Error.h"
#include <QObject>

class QAbstractItemModel;

namespace Mdt{ namespace Railway{

  /*! \brief
   */
  class MDT_RAILWAY_CORE_EXPORT VehicleTypeEdition /*: public QObject*/
  {
//    Q_OBJECT

   public:

    VehicleTypeEdition();

    /*! \brief Set vehicle type name
     */
    bool setVehicleTypeName(const QString & name);

    /*! \brief Get vehicle type name
     */
    QString vehicleTypeName() const
    {
      ///return mVehicleTypeClassTableModel.constRecord(mVehicleTypeClassCurrentRow).name();
      ///return mCurrentVehicleTypeClassData.name();
    }

    /*! \brief Access vehicle type class table model
     */
    QAbstractItemModel *vehicleTypeClassTableModel()
    {
      return mVehicleTypeClassTableModelContainer.modelForView();
//       return &mVehicleTypeClassTableModel;
    }

    /*! \brief Get vehicle type name
     */
    int vehicleTypeNameColumn() const
    {
      return mVehicleTypeClassTableModel.nameColumn();
    }

    /*! \brief Get vehicle type name field attributes
     */
    Mdt::Entity::FieldAttributes vehicleTypeNameFieldAttributes() const;

    /*! \brief Set manufacturer serie
     */
    bool setManufacturerSerie(const QString & serie);

    /*! \brief Get manufacturer serie
     */
    QString manufacturerSerie() const
    {
      return mVehicleTypeData.manufacturerSerie();
    }

    /*! \brief Get manufacturer serie field attributes
     */
    Mdt::Entity::FieldAttributes manufacturerSerieFieldAttributes() const;

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

//     VehicleTypeClassData mCurrentVehicleTypeClassData;
    int mVehicleTypeClassCurrentRow = -1;
    VehicleTypeClassTableModel mVehicleTypeClassTableModel;
    Mdt::ItemModel::ProxyModelContainer mVehicleTypeClassTableModelContainer;
    VehicleTypeData mVehicleTypeData;
    Mdt::Entity::FieldDataValidator<> mFieldDataValidator;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_EDITION_H
