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
#ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_TABLE_MODEL_H
#define MDT_RAILWAY_VEHICLE_TYPE_CLASS_TABLE_MODEL_H

#include "VehicleTypeClassDataList.h"
#include "VehicleTypeClassRepository.h"
#include "MdtRailway_CoreExport.h"
#include "Mdt/Entity/CachedEntityRepositoryTableModel.h"

namespace Mdt{ namespace Railway{

  /*! \brief Table model for vehicle type
   */
  class MDT_RAILWAY_CORE_EXPORT VehicleTypeClassTableModel : public Mdt::Entity::CachedEntityRepositoryTableModel<AbstractVehicleTypeClassRepository>
  {
   Q_OBJECT

   public:

    using ParentClass = Mdt::Entity::CachedEntityRepositoryTableModel<AbstractVehicleTypeClassRepository>;
//     using ParentClass::ParentClass;

    /*! \brief Constructor
     */
    VehicleTypeClassTableModel(QObject *parent = nullptr);

    /*! \brief Get the row for a given vehicle type class name
     *
     * Returns the row of given name or -1 if not found.
     */
//     int rowOfVehicleTypeName(const QString & name) const;

    /*! \brief Check if this list contains a vehicle type class designed by \a name
     */
//     bool containsVehicleTypeName(const QString & name) const;

    /*! \brief Get column index of vehicle type class name
     */
    int nameColumn() const;

    /*! \brief Set vehicle type name
     */
    bool setName(const QModelIndex & index, const QString & name);

    /*! \brief Get column index of vehicle type alias
     */
    int aliasColumn() const;

    /*! \brief Set vehicle type alias
     */
    bool setAlias(const QModelIndex & index, const QString & alias);

    /*! \brief Set data
     */
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_VEHICLE_TYPE_CLASS_TABLE_MODEL_H
