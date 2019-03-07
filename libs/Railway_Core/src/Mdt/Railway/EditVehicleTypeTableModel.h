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
#include "CreateVehicleType.h"
#include "UpdateVehicleTypeRequest.h"
#include "UpdateVehicleTypeResponse.h"
#include "VehicleTypeClassId.h"
#include "Entity/VehicleType.h"
#include "Entity/VehicleTypeClass.h"
#include "Mdt/ItemModel/AbstractEditableCachedTableModel.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/QueryExpression/SelectStatement.h"
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
      mVehicleTypeAlias = mVehicleTypeClass.makeSelectField( mVehicleTypeClass.def().alias() );
      mVehicleTypeClassId = mVehicleTypeClass.makeSelectField( mVehicleTypeClass.def().id() );

      addField(mVehicleTypeName);
      addField(mVehicleTypeAlias);
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

    int vehicleTypeAliasColumn() const
    {
      return fieldIndex(mVehicleTypeAlias);
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
    Mdt::QueryExpression::SelectField mVehicleTypeAlias;
    Mdt::Entity::QueryEntity<Entity::VehicleTypeClassEntity> mVehicleTypeClass;
  };

  /*! \brief Cached table model to edit vehicle types
   */
  class MDT_RAILWAY_CORE_EXPORT EditVehicleTypeTableModel : public Mdt::ItemModel::AbstractEditableCachedTableModel
  {
   Q_OBJECT

    using BaseClass = AbstractEditableCachedTableModel;

   public:

    explicit EditVehicleTypeTableModel(QObject *parent = nullptr);

//     /*! \brief Set the query factory
//      */
//     void setQueryFactory(const std::shared_ptr<SelectQueryFactory> & factory);

//     void setVehicleTypeRepository(const std::shared_ptr<VehicleTypeRepository> & repository);

    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    int vehicleTypeIdColumn() const
    {
      return mSelectStatement.vehicleTypeIdColumn();
    }

    int vehicleTypeClassIdColumn() const
    {
      return mSelectStatement.vehicleTypeClassIdColumn();
    }

    int vehicleTypeNameColumn() const
    {
      return mSelectStatement.vehicleTypeNameColumn();
    }

    int vehicleTypeAliasColumn() const
    {
      return mSelectStatement.vehicleTypeAliasColumn();
    }

    int manufacturerSerieColumn() const
    {
      return mSelectStatement.manufacturerSerieColumn();
    }

    QVariant horizontalHeaderDisplayRoleData(int column) const override;

//     /*! \brief Get the id column
//      */
//     int idColumn() const;

   signals:

    void fetchAllRequested(const Mdt::QueryExpression::SelectStatement & statement, int count);
//     void fetchVehicleTypeClassRequested
    void createVehicleTypeRequested(const CreateVehicleTypeRequest & request);
    void updateVehicleTypeRequested(const UpdateVehicleTypeRequest & request);

   public slots:

    void onCreateVehicleTypeSucceeded(const CreateVehicleTypeResponse & response);
    void onCreateVehicleTypeFailed(int transactionId, const Mdt::Error & error);
//     void onCreateVehicleTypeFailed(const CreateVehicleTypeRequest & request, const Mdt::Error & error);
    void onUpdateVehicleTypeSucceeded(const UpdateVehicleTypeResponse & response);
    void onUpdateVehicleTypeFailed(int taskId, const Mdt::Error & error);

   private:

    bool fetchRecords(int count) override;
    bool addRecordToBackend(const Mdt::Container::TableCacheRowTask & rowTask) override;
    bool updateRecordInBackend(const Mdt::Container::TableCacheRowTask & rowTask) override;
    bool removeRecordFromBackend(const Mdt::Container::TableCacheRowTask & rowTask) override;

    CreateVehicleTypeRequest makeCreateVehicleTypeRequest(int row) const;
    Mdt::Container::VariantRecord makeVariantRecord(const CreateVehicleTypeResponse & response) const;

    UpdateVehicleTypeRequest makeUpdateVehicleTypeRequest(int row) const;
    Mdt::Container::VariantRecord makeVariantRecord(const UpdateVehicleTypeRequest & response) const;

//     Mdt::Entity::EntitySelectStatement<Entity::VehicleTypeEntity> mSelectStatement;
    ListVehicleTypeToEditSelectStatement mSelectStatement;
//     std::shared_ptr<SelectQueryFactory> mSelectQueryFactory;
//     std::unique_ptr<SelectQuery> mSelectQuery;
//     std::shared_ptr<VehicleTypeRepository> mVehicleTypeRepository;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_TABLE_MODEL_H
