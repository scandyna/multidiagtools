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
#ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_CACHE_H
#define MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_CACHE_H

#include "VehicleTypeClassRepository.h"
#include "UpdateVehicleTypeClass.h"
#include "Mdt/Entity/AbstractEditableCache.h"
#include "Mdt/QueryExpression/AbstractSelectQueryFactory.h"
#include "MdtRailway_CoreExport.h"
#include <QVariant>
#include <QString>
#include <vector>
#include <array>
#include <memory>

namespace Mdt{ namespace Railway{

  /*! \brief
   */
  class MDT_RAILWAY_CORE_EXPORT EditVehicleTypeClassCache : public Mdt::Entity::AbstractEditableCache
  {
    using SelectQueryFactory = Mdt::QueryExpression::AbstractSelectQueryFactory;

   public:

//     /*! \brief Constructor
//      */
//     explicit EditVehicleTypeClassCache(QObject *parent = nullptr);

    /*! \brief Set the vehicle type class repository
     */
    void setVehicleTypeClassRepository(const std::shared_ptr<VehicleTypeClassRepository> & repository);

    /*! \brief Set the query factory
     *
     * \pre \a factory must be valid
     */
    void setSelectQueryFactory(const std::shared_ptr<SelectQueryFactory> & factory);

    /*! \brief Get column count
     */
    int columnCount() const noexcept override;
//     {
//       return mColumnCount;
//     }

    /*! \brief Get the horizontal header name at \a column
     *
     * This default implementation returns a emty string
     *
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    QString horizontalHeaderName(int column) const override;

   private:

    bool fetchRecords(int count) override;
    bool addRecordToBackend(int row) override;
    bool updateRecordInBackend(int row) override;
    bool removeRecordFromBackend(int row) override;

//     static constexpr int mColumnCount = 3;

//     using Record = std::array<QVariant, mColumnCount>;
//     using Table = std::vector<Record>;
// 
//     Table mTable;
    std::shared_ptr<VehicleTypeClassRepository> mVehicleTypeClassRepository;
    std::shared_ptr<SelectQueryFactory> mSelectQueryFactory;
//     Mdt::QueryExpression::SelectQueryFactory mSelectQueryFactory;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_CACHE_H
