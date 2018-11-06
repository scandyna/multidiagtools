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
#include "EditVehicleTypeClassCacheAsync.h"
#include <QString>

namespace Mdt{ namespace Railway{

EditVehicleTypeClassCacheAsync::EditVehicleTypeClassCacheAsync(QObject* parent)
 : AbstractAsyncReadOnlyCache(parent)
{
  mSelectAllVehicleTypeClassStatement.addField(mSelectAllVehicleTypeClassStatement.def().id());
  mSelectAllVehicleTypeClassStatement.addField(mSelectAllVehicleTypeClassStatement.def().name());
  mSelectAllVehicleTypeClassStatement.addField(mSelectAllVehicleTypeClassStatement.def().alias());
}

QString EditVehicleTypeClassCacheAsync::horizontalHeaderName(int column) const
{
  switch(column){
    case 0:
      return QStringLiteral("Id");
    case 1:
      return QStringLiteral("Name");
    case 2:
      return QStringLiteral("Alias");
    default:
      break;
  }

  return QString();
}

void EditVehicleTypeClassCacheAsync::setQueryFactory(const std::shared_ptr< EditVehicleTypeClassCacheAsync::SelectQueryFactory >& factory)
{
  Q_ASSERT(factory.get() != nullptr);

  mSelectQueryFactory = factory;
  mSelectQuery = std::move( mSelectQueryFactory->createSelectQuery() );
  connect( mSelectQuery.get(), &SelectQuery::newRecordAvailable, this, &EditVehicleTypeClassCacheAsync::fromBackendAppendRecord );
  connect( mSelectQuery.get(), &SelectQuery::errorOccured, this, &EditVehicleTypeClassCacheAsync::setLastError );
}

bool EditVehicleTypeClassCacheAsync::fetchRecords(int count)
{
  mSelectQuery->submitStatement(mSelectAllVehicleTypeClassStatement, count);

  return true;
}

}} // namespace Mdt{ namespace Railway{
