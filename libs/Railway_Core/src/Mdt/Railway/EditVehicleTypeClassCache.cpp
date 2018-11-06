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
#include "EditVehicleTypeClassCache.h"
#include "CreateVehicleTypeClass.h"
#include "Mdt/Entity/EntitySelectStatement.h"
// #include "Mdt/QueryExpression/SelectQuery.h"
#include "Entity/VehicleTypeClass.h"
#include "Mdt/Entity/FieldAt.h"
#include "Mdt/Container/VariantRecord.h"

#include <QDebug>

using namespace Mdt::Entity;
using Mdt::Container::VariantRecord;

namespace Mdt{ namespace Railway{

// EditVehicleTypeClassCache::EditVehicleTypeClassCache(QObject* parent)
//  : AbstractEditableCache(parent)
// {
// }

void EditVehicleTypeClassCache::setVehicleTypeClassRepository(const std::shared_ptr<VehicleTypeClassRepository> & repository)
{
  Q_ASSERT(repository.get() != nullptr);

  mVehicleTypeClassRepository = repository;
}

void EditVehicleTypeClassCache::setSelectQueryFactory(const std::shared_ptr<SelectQueryFactory> & factory)
{
  Q_ASSERT(factory.get() != nullptr);
  Q_ASSERT(factory->isValid());

  mSelectQueryFactory = factory;
}

int EditVehicleTypeClassCache::columnCount() const noexcept
{
  return fieldCount<Entity::VehicleTypeClassDef>();
}

QString EditVehicleTypeClassCache::horizontalHeaderName(int column) const
{
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  constexpr Entity::VehicleTypeClassDef vehicleTypeClassDef;

  return fieldNameAt(vehicleTypeClassDef, column);
}

bool EditVehicleTypeClassCache::fetchRecords(int count)
{
  qDebug() << "Fetching max " << count << " recs..";

  EntitySelectStatement<Entity::VehicleTypeClassEntity> stm;
  stm.addField(stm.def().id());
  stm.addField(stm.def().name());
  stm.addField(stm.def().alias());

  auto query = mSelectQueryFactory->createSelectQuery();
  if(!query->exec(stm)){
    setLastError(query->lastError());
    return false;
  }
  for(int i = 0; i < count; ++i){
    if(!query->next()){
      return true;
    }
    VariantRecord record(3);
    record.setValue(0, query->value(0));
    record.setValue(1, query->value(1));
    record.setValue(2, query->value(2));
    fromBackendAppendRecord(record);
  }

  return true;
}

bool EditVehicleTypeClassCache::addRecordToBackend(int row)
{
  qDebug() << "Add new vehicle type class to backend ...";
  CreateVehicleTypeClass cvtc(mVehicleTypeClassRepository);

  CreateVehicleTypeClassRequest request;
  request.name = data(row, 1).toString();
  request.alias = data(row, 2).toString();
//   request.name = record.value(1).toString();
//   request.alias = record.value(2).toString();

  if(!cvtc.execute(request)){
    setLastError(cvtc.lastError());
    return false;
  }
  const auto response = cvtc.response();
  VariantRecord record(3);
  record.setValue(0, response.id.value());
  record.setValue(1, response.name);
  record.setValue(2, response.alias);
  fromBackendSetRecord(row, record);

  return true;
}

bool EditVehicleTypeClassCache::updateRecordInBackend(int row)
{
  UpdateVehicleTypeClass updateVtc(mVehicleTypeClassRepository);

  UpdateVehicleTypeClassRequest request;
  request.id = VehicleTypeClassId::fromQVariant( data(row, 0) );
  request.name = data(row, 1).toString();
  request.alias = data(row, 2).toString();

  if(!updateVtc.execute(request)){
    setLastError(updateVtc.lastError());
    return false;
  }
  const auto response = updateVtc.response();
  VariantRecord record(3);
  record.setValue(0, response.id.value());
  record.setValue(1, response.name);
  record.setValue(2, response.alias);
  fromBackendSetRecord(row, record);

  return true;
}

bool EditVehicleTypeClassCache::removeRecordFromBackend(int row)
{
  qDebug() << "EditVehicleTypeClassCache::removeRecordFromBackend() not implemented";
  return false;
}

}} // namespace Mdt{ namespace Railway{
