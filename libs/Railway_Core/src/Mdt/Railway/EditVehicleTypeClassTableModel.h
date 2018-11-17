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
#ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_TABLE_MODEL_H
#define MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_TABLE_MODEL_H

#include "EditVehicleTypeClassCache.h"
#include "VehicleTypeClassRepository.h"
#include "Entity/VehicleTypeClass.h"

#include "Mdt/Entity/AbstractEditableCacheTableModel.h"
// #include "Mdt/ItemModel/AbstractCachedTableModel.h"
// #include "Mdt/QueryExpression/SelectQueryFactory.h"
#include "MdtRailway_CoreExport.h"
#include <memory>

namespace Mdt{ namespace Railway{

  /*! \brief
   */
  class MDT_RAILWAY_CORE_EXPORT EditVehicleTypeClassTableModel : public Mdt::Entity::AbstractEditableCacheTableModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit EditVehicleTypeClassTableModel(QObject *parent = nullptr);

//     /*! \brief Set the vehicle type class repository
//      */
//     void setVehicleTypeClassRepository(const std::shared_ptr<VehicleTypeClassRepository> & repository);

//     /*! \brief Set the query factory
//      *
//      * \pre \a factory must be valid
//      */
//     void setSelectQueryFactory(const Mdt::QueryExpression::SelectQueryFactory & factory);

    
//     /*! \brief Fetch all
//      */
//     bool fetchAll();

//     Mdt::Error lastError() const
//     {
//       return mCache.lastError();
//     }

   private:

//     int columnCountImpl() const override;
//     int cachedRowCount() const override;
//     QVariant displayRoleData(int row, int column) const override;
//     QVariant horizontalHeaderDisplayRoleData(int column) const override;
//     Container::TableCacheOperation operationAtRow(int row) const override;
// 
//     EditVehicleTypeClassCache mCache;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_TABLE_MODEL_H
