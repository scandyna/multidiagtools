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
#include "EditVehicleTypeClassTableModel.h"

using namespace Mdt::Container;
using namespace Mdt::ItemModel;

namespace Mdt{ namespace Railway{

EditVehicleTypeClassTableModel::EditVehicleTypeClassTableModel(QObject* parent)
 : BaseClass(parent)
{
}

// void EditVehicleTypeClassTableModel::setQueryFactory(const std::shared_ptr<SelectQueryFactory> & factory)
// {
//   Q_ASSERT(factory.get() != nullptr);
// 
//   mSelectQueryFactory = factory;
//   mSelectQuery = std::move( mSelectQueryFactory->createSelectQuery() );
//   connect( mSelectQuery.get(), &SelectQuery::newRecordAvailable, this, &EditVehicleTypeClassTableModel::fromBackendAppendRecord );
// }

void EditVehicleTypeClassTableModel::onCreateVehicleTypeClassSucceded(const CreateVehicleTypeClassResponse & response)
{
  const TableCacheTask task(response.transactionId);
  const VariantRecord record = makeVariantRecord(response);

  taskSucceeded(task, record);

//   const TableCacheTransaction transaction(response.transactionId);
//   const VariantRecord record = makeVariantRecord(response);
// 
//   transactionSucceeded(transaction, record);
}

// void EditVehicleTypeClassTableModel::onCreateVehicleTypeClassFailed(const CreateVehicleTypeClassRequest & request, const Mdt::Error & error)
// {
//   const TableCacheTransaction transaction(request.transactionId);
// 
//   transactionFailed(transaction, error);
// }

void EditVehicleTypeClassTableModel::onCreateVehicleTypeClassFailed(int transactionId, const Mdt::Error & error)
{
  const TableCacheTask task(transactionId);

  taskFailed(task, error);

//   const TableCacheTransaction transaction(transactionId);
// 
//   transactionFailed(transaction, error);
}

void EditVehicleTypeClassTableModel::onUpdateVehicleTypeClassSucceded(const UpdateVehicleTypeClassResponse & response)
{
  const TableCacheTask task(response.transactionId);
  const VariantRecord record = makeVariantRecord(response);

  taskSucceeded(task, record);

//   const TableCacheTransaction transaction(response.transactionId);
//   const VariantRecord record = makeVariantRecord(response);
// 
//   transactionSucceeded(transaction, record);
}

// void EditVehicleTypeClassTableModel::onUpdateVehicleTypeClassFailed(const UpdateVehicleTypeClassRequest & request, const Mdt::Error & error)
// {
//   const TableCacheTransaction transaction(request.transactionId);
// 
//   transactionFailed(transaction, error);
// }

void EditVehicleTypeClassTableModel::onUpdateVehicleTypeClassFailed(int transactionId, const Mdt::Error & error)
{
  const TableCacheTask task(transactionId);

  taskFailed(task, error);

//   const TableCacheTransaction transaction(transactionId);
// 
//   transactionFailed(transaction, error);
}

void EditVehicleTypeClassTableModel::onRemoveVehicleTypeClassSucceeded(const RemoveVehicleTypeClassResponse & response)
{
  const TableCacheTask task(response.taskId);

  removeRecordTaskSucceeded(task);
}

void EditVehicleTypeClassTableModel::onRemoveVehicleTypeClassFailed(int taskId, const Mdt::Error & error)
{
  const TableCacheTask task(taskId);

  taskFailed(task, error);
}

bool EditVehicleTypeClassTableModel::fetchRecords(int count)
{
  emit fetchVehicleTypeClassesRequested(mSelectStatement, count);
//   mSelectQuery->submitStatement(mSelectStatement, count);

  return true;
}

bool EditVehicleTypeClassTableModel::addRecordToBackend(const Mdt::Container::TableCacheRowTask & rowTask)
{
  CreateVehicleTypeClassRequest request = makeCreateVehicleTypeRequest(rowTask.row());
  request.transactionId = rowTask.taskId();

  emit createVehicleTypeClassRequested(request);

  return true;
}

bool EditVehicleTypeClassTableModel::updateRecordInBackend(const Mdt::Container::TableCacheRowTask & rowTask)
{
  UpdateVehicleTypeClassRequest request = makeUpdateVehicleTypeRequest(rowTask.row());
  request.transactionId = rowTask.taskId();

  emit updateVehicleTypeClassRequested(request);

  return true;
}

bool EditVehicleTypeClassTableModel::removeRecordFromBackend(const Mdt::Container::TableCacheRowTask & rowTask)
{
  RemoveVehicleTypeClassRequest request;
  const auto rec = record(rowTask.row());

  request.id = VehicleTypeClassId::fromQVariant( rec.value(idColumn()) );
  request.taskId = rowTask.taskId();

  emit removeVehicleTypeClassRequested(request);

  return true;
}

CreateVehicleTypeClassRequest EditVehicleTypeClassTableModel::makeCreateVehicleTypeRequest(int row) const
{
  CreateVehicleTypeClassRequest request;
  VariantRecord rec = record(row);

  request.name = rec.value(vehicleTypeNameColumn()).toString();
  request.alias = rec.value(vehicleTypeAliasColumn()).toString();

  return request;
}

UpdateVehicleTypeClassRequest EditVehicleTypeClassTableModel::makeUpdateVehicleTypeRequest(int row) const
{
  UpdateVehicleTypeClassRequest request;
  VariantRecord rec = record(row);

  request.id = VehicleTypeClassId::fromQVariant( rec.value(idColumn()) );
  request.name = rec.value(vehicleTypeNameColumn()).toString();
  request.alias = rec.value(vehicleTypeAliasColumn()).toString();

  return request; 
}

// Mdt::Container::VariantRecord EditVehicleTypeClassTableModel::makeVariantRecord(const CreateVehicleTypeClassResponse & response) const
// {
//   VariantRecord record(columnCount());
// 
//   return record;
// }
// 
// Mdt::Container::VariantRecord EditVehicleTypeClassTableModel::makeVariantRecord(const UpdateVehicleTypeClassResponse & response) const
// {
//   VariantRecord record(columnCount());
// 
//   return record;
// }

}} // namespace Mdt{ namespace Railway{
