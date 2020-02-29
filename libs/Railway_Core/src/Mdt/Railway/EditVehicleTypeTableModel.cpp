/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "EditVehicleTypeTableModel.h"
#include "Entity/VehicleTypeClass.h"
#include "Mdt/Container/VariantRecord.h"

#include <QDebug>

using Mdt::Entity::QueryEntity;
// using Mdt::Container::VariantRecord;
using namespace Mdt::Container;
using namespace Mdt::ItemModel;

namespace Mdt{ namespace Railway{

EditVehicleTypeTableModel::EditVehicleTypeTableModel(QObject* parent)
 : BaseClass(parent)
{
//   QueryEntity<Entity::VehicleTypeClassEntity> vehicleTypeClass;
// 
//   const auto vehicleTypeId = mSelectStatement.makeSelectField( mSelectStatement.def().id() );
//   const auto vehicleTypeClassId = vehicleTypeClass.makeSelectField( vehicleTypeClass.def().id() );
//   const auto vehicleTypeName = vehicleTypeClass.makeSelectField( vehicleTypeClass.def().name() );
//   const auto manufacturerSerie = mSelectStatement.makeSelectField( mSelectStatement.def().manufacturerSerie() );
// 
//   mSelectStatement.addField( vehicleTypeName );
//   mSelectStatement.addField( manufacturerSerie );
//   mSelectStatement.addField( vehicleTypeId );
//   mSelectStatement.addField( vehicleTypeClassId );
//   mSelectStatement.joinEntity<Mdt::Railway::Entity::VehicleTypeClassVehicleTypeRelation>();

//   connect(&mCreateVehicleType, &CreateVehicleType::succeed, this, &EditVehicleTypeTableModel::onCreateVehicleTypeSucceded);
//   connect(&mCreateVehicleType, &CreateVehicleType::failed, this, &EditVehicleTypeTableModel::onCreateVehicleTypeFailed);
}

// void EditVehicleTypeTableModel::setQueryFactory(const std::shared_ptr< SelectQueryFactory >& factory)
// {
//   Q_ASSERT(factory.get() != nullptr);
// 
//   mSelectQueryFactory = factory;
//   mSelectQuery = std::move( mSelectQueryFactory->createSelectQuery() );
//   connect( mSelectQuery.get(), &SelectQuery::newRecordAvailable, this, &EditVehicleTypeTableModel::fromBackendAppendRecord );
// //   connect( mSelectQuery.get(), &SelectQuery::errorOccured, this, &ChooseVehicleTypeCache::setLastError );
// }

// void EditVehicleTypeTableModel::setVehicleTypeRepository(const std::shared_ptr<VehicleTypeRepository> & repository)
// {
//   Q_ASSERT(repository.get() != nullptr);
// 
//   mVehicleTypeRepository = repository;
// }

int EditVehicleTypeTableModel::columnCount(const QModelIndex& /*parent*/) const
{
  return mSelectStatement.fieldCount();
}

QVariant EditVehicleTypeTableModel::horizontalHeaderDisplayRoleData(int column) const
{
  if(column == mSelectStatement.vehicleTypeNameColumn()){
    return tr("Type");
  }
  if(column == mSelectStatement.manufacturerSerieColumn()){
    return tr("Serie");
  }
  if(column == mSelectStatement.vehicleTypeIdColumn()){
    return tr("V. Id");
  }
  if(column == mSelectStatement.vehicleTypeClassIdColumn()){
    return tr("VtC. Id");
  }

  return BaseClass::horizontalHeaderDisplayRoleData(column);
}

// int EditVehicleTypeTableModel::idColumn() const
// {
//   return 2;
// }

void EditVehicleTypeTableModel::onCreateVehicleTypeSucceeded(const CreateVehicleTypeResponse & response)
{
  const TableCacheTask task(response.transactionId);
  VariantRecord record = makeVariantRecord(response);

  taskSucceeded(task, record);

//   TableCacheTransaction transaction(response.transactionId);
//   VariantRecord record = makeVariantRecord(response);
// 
//   transactionSucceeded(transaction, record);
}

void EditVehicleTypeTableModel::onCreateVehicleTypeFailed(int transactionId, const Mdt::Error & error)
{
  const TableCacheTask task(transactionId);

  taskFailed(task, error);

//   TableCacheTransaction transaction(transactionId);
// 
//   transactionFailed(transaction, error);
}

// void EditVehicleTypeTableModel::onCreateVehicleTypeFailed(const CreateVehicleTypeRequest & request, const Mdt::Error & error)
// {
//   TableCacheTransaction transaction(request.transactionId);
// 
//   transactionFailed(transaction, error);
// }

void EditVehicleTypeTableModel::onUpdateVehicleTypeSucceeded(const UpdateVehicleTypeResponse & response)
{
}

void EditVehicleTypeTableModel::onUpdateVehicleTypeFailed(int taskId, const Mdt::Error & error)
{
}

bool EditVehicleTypeTableModel::fetchRecords(int count)
{
  emit fetchAllRequested(mSelectStatement, count);
//   mSelectQuery->submitStatement(mSelectStatement, count);

  return true;
}

bool EditVehicleTypeTableModel::addRecordToBackend(const Mdt::Container::TableCacheRowTask & rowTtask)
{
  CreateVehicleTypeRequest request = makeCreateVehicleTypeRequest(rowTtask.row());
  request.transactionId = rowTtask.taskId();
  emit createVehicleTypeRequested(request);

  return true;
}

bool EditVehicleTypeTableModel::updateRecordInBackend(const Mdt::Container::TableCacheRowTask & rowTtask)
{
  UpdateVehicleTypeRequest request = makeUpdateVehicleTypeRequest(rowTtask.row());
  request.taskId = rowTtask.taskId();
  emit updateVehicleTypeRequested(request);

  return false;
}

bool EditVehicleTypeTableModel::removeRecordFromBackend(const Mdt::Container::TableCacheRowTask & rowTask)
{
}

CreateVehicleTypeRequest EditVehicleTypeTableModel::makeCreateVehicleTypeRequest(int row) const
{
  CreateVehicleTypeRequest request;
  const VariantRecord rec = record(row);

  qDebug() << "make request, id: " << rec.value( mSelectStatement.vehicleTypeClassIdColumn() );

  request.vehicleTypeClassId = VehicleTypeClassId::fromQVariant( rec.value( mSelectStatement.vehicleTypeClassIdColumn() ) );
  request.manufacturerSerie = rec.value( mSelectStatement.manufacturerSerieColumn() ).toString();

  return request;
}

VariantRecord EditVehicleTypeTableModel::makeVariantRecord(const CreateVehicleTypeResponse & response) const
{
  VariantRecord record(columnCount());

  record.setValue( mSelectStatement.vehicleTypeIdColumn(), response.vehicleTypeId.value() );
  record.setValue( mSelectStatement.vehicleTypeClassIdColumn(), response.vehicleTypeClassId.value() );
  record.setValue( mSelectStatement.manufacturerSerieColumn(), response.manufacturerSerie );

  return record;
}

UpdateVehicleTypeRequest EditVehicleTypeTableModel::makeUpdateVehicleTypeRequest(int row) const
{
}

VariantRecord EditVehicleTypeTableModel::makeVariantRecord(const UpdateVehicleTypeRequest & response) const
{
}

}} // namespace Mdt{ namespace Railway{
