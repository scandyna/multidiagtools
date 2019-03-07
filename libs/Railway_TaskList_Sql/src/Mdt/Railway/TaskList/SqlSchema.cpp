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
#include "SqlSchema.h"
#include "Mdt/Railway/TaskList/VehicleTypeDataStruct.h"
#include "Mdt/Reflection/Relation.h"
#include "Mdt/Sql/Schema/Reflection.h"
#include "Mdt/Sql/Schema/ForeignKeySettings.h"
#include "Mdt/Sql/Schema/Driver.h"

namespace Mdt{ namespace Railway{ namespace TaskList{

SqlSchema::SqlSchema(QObject *parent)
 : QObject(parent)
{
}

bool SqlSchema::create(const QSqlDatabase& db)
{
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.isOpen());

  using Mdt::Sql::Schema::ForeignKeyAction;

  Mdt::Sql::Schema::ForeignKeySettings commonForeignKeySettings;
  commonForeignKeySettings.setIndexed(true);
  commonForeignKeySettings.setOnDeleteAction(ForeignKeyAction::Restrict);
  commonForeignKeySettings.setOnUpdateAction(ForeignKeyAction::Cascade);

  Mdt::Sql::Schema::Driver driver(db);
  if(!driver.isValid()){
    setLastError(driver.lastError());
    return false;
  }

  auto vehicleTypeTable = Mdt::Sql::Schema::tableFromReflected<VehicleTypeDef, VehicleTypePrimaryKey>();
  if(!driver.createTable(vehicleTypeTable)){
    setLastError(driver.lastError());
    return false;
  }

  return true;
}

void SqlSchema::setLastError(const Mdt::Error & error)
{
  Q_ASSERT(!error.isNull());

  mLastError = error;
}

}}} // namespace Mdt{ namespace Railway{ namespace TaskList{
