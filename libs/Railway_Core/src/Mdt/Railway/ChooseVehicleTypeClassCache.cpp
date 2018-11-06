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
#include "ChooseVehicleTypeClassCache.h"

namespace Mdt{ namespace Railway{

ChooseVehicleTypeClassCache::ChooseVehicleTypeClassCache(QObject* parent)
 : AbstractAsyncReadOnlyCache(parent)
{
  mSelectStatement.addField( mSelectStatement.def().id() );
  mSelectStatement.addField( mSelectStatement.def().name() );
}

void ChooseVehicleTypeClassCache::setQueryFactory(const std::shared_ptr< ChooseVehicleTypeClassCache::SelectQueryFactory >& factory)
{
  Q_ASSERT(factory.get() != nullptr);

  mSelectQueryFactory = factory;
  mSelectQuery = std::move( mSelectQueryFactory->createSelectQuery() );
  connect( mSelectQuery.get(), &SelectQuery::newRecordAvailable, this, &ChooseVehicleTypeClassCache::fromBackendAppendRecord );
  connect( mSelectQuery.get(), &SelectQuery::errorOccured, this, &ChooseVehicleTypeClassCache::setLastError );
}

VehicleTypeClassId ChooseVehicleTypeClassCache::id(int row) const noexcept
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());

  return VehicleTypeClassId::fromQVariant( data(row, idColumn()) );
}

bool ChooseVehicleTypeClassCache::fetchRecords(int count)
{
  mSelectQuery->submitStatement(mSelectStatement, count);

  return true;
}

}} // namespace Mdt{ namespace Railway{
