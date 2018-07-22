/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "SelectQueryTestBase.h"
#include "Mdt/Sql/InsertQuery.h"
#include "Mdt/Sql/Schema/Driver.h"
#include "Mdt/Entity/SqlTable.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool SelectQueryTestBase::createTestSchema()
{
  Q_ASSERT(database().isValid());

  Mdt::Sql::Schema::Driver driver(database());
  if(!driver.isValid()){
    qWarning() << "Could not create test schema, error: " << driver.lastError().text();
    return false;
  }

  if( !driver.createTable( Mdt::Entity::SqlTable::fromEntity<PersonEntity>() ) ){
    qWarning() << "Could not create table for PersonEntity, error: " << driver.lastError().text();
    return false;
  }
  if( !driver.createTable( Mdt::Entity::SqlTable::fromEntity<AddressEntity>() ) ){
    qWarning() << "Could not create table for AddressEntity, error: " << driver.lastError().text();
    return false;
  }

  return true;
}

bool SelectQueryTestBase::insertPerson(int id, const QString & firstName, int age, const QString & remarks)
{
  constexpr PersonEntity person;
  Mdt::Sql::InsertQuery query(database());

  query.setTableName(person.def().entityName());
  query.addValue( Mdt::Sql::FieldName(person.def().id().fieldName()), id );
  query.addValue( Mdt::Sql::FieldName(person.def().firstName().fieldName()), firstName );
  query.addValue( Mdt::Sql::FieldName(person.def().age().fieldName()), age );
  query.addValue( Mdt::Sql::FieldName(person.def().remarks().fieldName()), remarks );
  if(!query.exec()){
    qWarning() << "Insert person failed: " << query.lastError().text();
    return false;
  }

  return true;
}

bool SelectQueryTestBase::cleanupPersonTable()
{
  QSqlQuery query(database());

  if(!query.exec("DELETE FROM " + PersonEntity::def().entityName())){
    qWarning() << "Cleaning Person table failed: " << query.lastError();
    return false;
  }

  return true;
}
