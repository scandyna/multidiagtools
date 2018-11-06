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
#ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_TABLE_MODEL_H
#define MDT_RAILWAY_EDIT_VEHICLE_TYPE_TABLE_MODEL_H

#include "VehicleTypeId.h"
#include "Entity/VehicleType.h"
#include "Entity/VehicleTypeClass.h"
#include "Mdt/ItemModel/AbstractEditableCachedTableModel.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/QueryExpression/AbstractAsyncSelectQuery.h"
#include "Mdt/QueryExpression/AbstractAsyncSelectQueryFactory.h"
#include "MdtRailway_CoreExport.h"
#include <memory>

namespace Mdt{ namespace Railway{

  class ListVehicleTypeToEditSelectStatement : public Mdt::Entity::EntitySelectStatement<Entity::VehicleTypeEntity>
  {
   public:

    ListVehicleTypeToEditSelectStatement()
    {
      mVehicleTypeId = makeSelectField( def().id() );
      mManufacturerSerie = makeSelectField( def().manufacturerSerie() );
      mVehicleTypeName = mVehicleTypeClass.makeSelectField( mVehicleTypeClass.def().name() );
      mVehicleTypeClassId = mVehicleTypeClass.makeSelectField( mVehicleTypeClass.def().id() );

      addField(mVehicleTypeName);
      addField(mManufacturerSerie);
      addField(mVehicleTypeId);
      addField(mVehicleTypeClassId);
      joinEntity<Mdt::Railway::Entity::VehicleTypeClassVehicleTypeRelation>();
    }

    int manufacturerSerieColumn() const
    {
      return fieldIndex(mManufacturerSerie);
    }

    int vehicleTypeNameColumn() const
    {
      return fieldIndex(mVehicleTypeName);
    }

    int vehicleTypeIdColumn() const
    {
      return fieldIndex(mVehicleTypeId);
    }

    int vehicleTypeClassIdColumn() const
    {
      return fieldIndex(mVehicleTypeClassId);
    }

   private:

    Mdt::QueryExpression::SelectField mVehicleTypeId;
    Mdt::QueryExpression::SelectField mVehicleTypeName;
    Mdt::QueryExpression::SelectField mManufacturerSerie;
    Mdt::QueryExpression::SelectField mVehicleTypeClassId;
    Mdt::Entity::QueryEntity<Entity::VehicleTypeClassEntity> mVehicleTypeClass;
  };

  /*! \brief
   */
  class MDT_RAILWAY_CORE_EXPORT EditVehicleTypeTableModel : public Mdt::ItemModel::AbstractEditableCachedTableModel
  {
   Q_OBJECT

    using BaseClass = AbstractEditableCachedTableModel;
    using SelectQuery = Mdt::QueryExpression::AbstractAsyncSelectQuery;
    using SelectQueryFactory = Mdt::QueryExpression::AbstractAsyncSelectQueryFactory;

   public:

    explicit EditVehicleTypeTableModel(QObject *parent = nullptr);

    /*! \brief Set the query factory
     */
    void setQueryFactory(const std::shared_ptr<SelectQueryFactory> & factory);

    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    int manufacturerSerieColumn() const
    {
      return mSelectStatement.manufacturerSerieColumn();
    }

    int vehicleTypeNameColumn() const
    {
      return mSelectStatement.vehicleTypeNameColumn();
    }

    QVariant horizontalHeaderDisplayRoleData(int column) const override;

//     /*! \brief Get the id column
//      */
//     int idColumn() const;

   private:

    bool fetchRecords(int count) override;

//     Mdt::Entity::EntitySelectStatement<Entity::VehicleTypeEntity> mSelectStatement;
    ListVehicleTypeToEditSelectStatement mSelectStatement;
    std::shared_ptr<SelectQueryFactory> mSelectQueryFactory;
    std::unique_ptr<SelectQuery> mSelectQuery;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_TABLE_MODEL_H
