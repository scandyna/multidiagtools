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
#ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_CACHE_ASYNC_H
#define MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_CACHE_ASYNC_H

#include "Mdt/Railway/Entity/VehicleTypeClass.h"
#include "Mdt/Entity/AbstractAsyncReadOnlyCache.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/QueryExpression/AbstractAsyncSelectQuery.h"
#include "Mdt/QueryExpression/AbstractAsyncSelectQueryFactory.h"
#include "MdtRailway_CoreExport.h"
#include <QString>
#include <memory>

namespace Mdt{ namespace Railway{

  /*! \brief
   */
  class MDT_RAILWAY_CORE_EXPORT EditVehicleTypeClassCacheAsync : public Mdt::Entity::AbstractAsyncReadOnlyCache
  {
   Q_OBJECT

    using SelectQuery = Mdt::QueryExpression::AbstractAsyncSelectQuery;
    using SelectQueryFactory = Mdt::QueryExpression::AbstractAsyncSelectQueryFactory;

  public:

    EditVehicleTypeClassCacheAsync(QObject *parent = nullptr);

    void setQueryFactory(const std::shared_ptr<SelectQueryFactory> & factory);

    int columnCount() const noexcept override
    {
      return mSelectAllVehicleTypeClassStatement.fieldCount();
    }

    QString horizontalHeaderName(int column) const override;

  private:

    bool fetchRecords(int count) override;

    Mdt::Entity::EntitySelectStatement<Entity::VehicleTypeClassEntity> mSelectAllVehicleTypeClassStatement;
    std::shared_ptr<SelectQueryFactory> mSelectQueryFactory;
    std::unique_ptr<SelectQuery> mSelectQuery;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_CACHE_ASYNC_H
