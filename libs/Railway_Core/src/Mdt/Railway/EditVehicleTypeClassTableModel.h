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

#include "CreateVehicleTypeClassRequest.h"
#include "CreateVehicleTypeClassResponse.h"
#include "UpdateVehicleTypeClassRequest.h"
#include "UpdateVehicleTypeClassResponse.h"
#include "RemoveVehicleTypeClassRequest.h"
#include "RemoveVehicleTypeClassResponse.h"
#include "VehicleTypeClassRepository.h"
#include "Entity/VehicleTypeClass.h"
#include "Mdt/QueryExpression/SelectStatement.h"
#include "Mdt/Entity/EntitySelectStatement.h"
#include "Mdt/ItemModel/AbstractEditableCachedTableModel.h"
// #include "Mdt/QueryExpression/AbstractAsyncSelectQuery.h"
// #include "Mdt/QueryExpression/AbstractAsyncSelectQueryFactory.h"
#include "MdtRailway_CoreExport.h"
#include <memory>

#include "EditVehicleTypeClassCache.h"
#include "Mdt/Entity/AbstractEditableCacheTableModel.h"
// #include "Mdt/ItemModel/AbstractCachedTableModel.h"
// #include "Mdt/QueryExpression/SelectQueryFactory.h"

namespace Mdt{ namespace Railway{

  class ListVehicleTypeClassToEditSelectStatement : public Mdt::Entity::EntitySelectStatement<Entity::VehicleTypeClassEntity>
  {
   public:

    ListVehicleTypeClassToEditSelectStatement()
    {
//       mVehicleTypeClassId = makeSelectField( def().id() );
//       mVehicleTypeName = makeSelectField( def().name() );
//       mVehicleTypeAlias = makeSelectField( def().alias() );

      addField(mVehicleTypeClassId);
      addField(mVehicleTypeName);
      addField(mVehicleTypeAlias);
    }

    int idColumn() const
    {
      return fieldIndex(mVehicleTypeClassId);
    }

    int vehicleTypeNameColumn() const
    {
      return fieldIndex(mVehicleTypeName);
    }

    int vehicleTypeAliasColumn() const
    {
      return fieldIndex(mVehicleTypeAlias);
    }

   private:

    Mdt::QueryExpression::SelectField mVehicleTypeClassId;
    Mdt::QueryExpression::SelectField mVehicleTypeName;
    Mdt::QueryExpression::SelectField mVehicleTypeAlias;
  };

  /*! \brief
   */
  class MDT_RAILWAY_CORE_EXPORT EditVehicleTypeClassTableModel : public Mdt::ItemModel::AbstractEditableCachedTableModel
  {
   Q_OBJECT

    using BaseClass = AbstractEditableCachedTableModel;

   public:

    /*! \brief Constructor
     */
    explicit EditVehicleTypeClassTableModel(QObject *parent = nullptr);

    /*! \brief Get count of columns
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override
    {
      Q_UNUSED(parent);

      return mSelectStatement.fieldCount();
    }

    int idColumn() const
    {
      return mSelectStatement.idColumn();
    }

    int vehicleTypeNameColumn() const
    {
      return mSelectStatement.vehicleTypeNameColumn();
    }

    int vehicleTypeAliasColumn() const
    {
      return mSelectStatement.vehicleTypeAliasColumn();
    }

   signals:

    void fetchVehicleTypeClassesRequested(const Mdt::QueryExpression::SelectStatement & statement, int count);
    void createVehicleTypeClassRequested(const CreateVehicleTypeClassRequest & request);
    void updateVehicleTypeClassRequested(const UpdateVehicleTypeClassRequest & request);
    void removeVehicleTypeClassRequested(const RemoveVehicleTypeClassRequest & request);

   public slots:

    void onCreateVehicleTypeClassSucceded(const CreateVehicleTypeClassResponse & response);
//     void onCreateVehicleTypeClassFailed(const CreateVehicleTypeClassRequest & request, const Mdt::Error & error);
    void onCreateVehicleTypeClassFailed(int transactionId, const Mdt::Error & error);

    void onUpdateVehicleTypeClassSucceded(const UpdateVehicleTypeClassResponse & response);
//     void onUpdateVehicleTypeClassFailed(const UpdateVehicleTypeClassRequest & request, const Mdt::Error & error);
    void onUpdateVehicleTypeClassFailed(int transactionId, const Mdt::Error & error);

    void onRemoveVehicleTypeClassSucceeded(const RemoveVehicleTypeClassResponse & response);
    void onRemoveVehicleTypeClassFailed(int taskId, const Mdt::Error & error);

   private:

    bool fetchRecords(int count) override;
    bool addRecordToBackend(const Mdt::Container::TableCacheRowTask & rowTask) override;
    bool updateRecordInBackend(const Mdt::Container::TableCacheRowTask & rowTask) override;
    bool removeRecordFromBackend(const Mdt::Container::TableCacheRowTask & rowTask) override;

    CreateVehicleTypeClassRequest makeCreateVehicleTypeRequest(int row) const;
    UpdateVehicleTypeClassRequest makeUpdateVehicleTypeRequest(int row) const;

//     Mdt::Container::VariantRecord makeVariantRecord(const CreateVehicleTypeClassResponse & response) const;
//     Mdt::Container::VariantRecord makeVariantRecord(const UpdateVehicleTypeClassResponse & response) const;

    template<typename Response>
    Mdt::Container::VariantRecord makeVariantRecord(const Response & response) const
    {
      Mdt::Container::VariantRecord record(columnCount());

      record.setValue(idColumn(), response.id.value());
      record.setValue(vehicleTypeNameColumn(), response.name);
      record.setValue(vehicleTypeAliasColumn(), response.alias);

      return record;
    }

    ListVehicleTypeClassToEditSelectStatement mSelectStatement;
  };

}} // namespace Mdt{ namespace Railway{

#endif // #ifndef MDT_RAILWAY_EDIT_VEHICLE_TYPE_CLASS_TABLE_MODEL_H
