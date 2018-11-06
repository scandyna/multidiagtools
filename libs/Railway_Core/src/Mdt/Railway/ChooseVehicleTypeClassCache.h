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
#ifndef MDT_RAILWAY_CHOOSE_VEHICLE_TYPE_CLASS_CACHE_H
#define MDT_RAILWAY_CHOOSE_VEHICLE_TYPE_CLASS_CACHE_H

#include "VehicleTypeClassId.h"
#include "Entity/VehicleTypeClass.h"
#include "Mdt/Entity/AbstractAsyncReadOnlyCache.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/QueryExpression/AbstractAsyncSelectQuery.h"
#include "Mdt/QueryExpression/AbstractAsyncSelectQueryFactory.h"
#include "MdtRailway_CoreExport.h"
#include <memory>

namespace Mdt{ namespace Railway{

  /*! \brief
   */
  class MDT_RAILWAY_CORE_EXPORT ChooseVehicleTypeClassCache : public Mdt::Entity::AbstractAsyncReadOnlyCache
  {
   Q_OBJECT

    using SelectQuery = Mdt::QueryExpression::AbstractAsyncSelectQuery;
    using SelectQueryFactory = Mdt::QueryExpression::AbstractAsyncSelectQueryFactory;

   public:

    /*! \brief Constructor
     */
    ChooseVehicleTypeClassCache(QObject *parent = nullptr);

    /*! \brief Set the query factory
     */
    void setQueryFactory(const std::shared_ptr<SelectQueryFactory> & factory);

    /*! \brief Get count of columns
     */
    int columnCount() const noexcept override
    {
      return mSelectStatement.fieldCount();
    }

    /*! \brief Get the id column
     */
    int idColumn() const noexcept
    {
      return 0;
    }

    /*! \brief Get the name column
     */
    int nameColumn() const noexcept
    {
      return 1;
    }

    /*! \brief Get vehicle type class id for \a row
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    VehicleTypeClassId id(int row) const noexcept;

   private:

    bool fetchRecords(int count) override;

    Mdt::Entity::EntitySelectStatement<Entity::VehicleTypeClassEntity> mSelectStatement;
    std::shared_ptr<SelectQueryFactory> mSelectQueryFactory;
    std::unique_ptr<SelectQuery> mSelectQuery;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_CHOOSE_VEHICLE_TYPE_CLASS_CACHE_H
