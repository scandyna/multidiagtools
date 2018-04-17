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
#include "SelectStatementTest.h"
#include "Mdt/Entity/SqlSelectStatement.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include <QString>

using namespace Mdt::Entity;

/*
 * Entities
 */

struct PersonDataStruct
{
  qulonglong id;
  QString firstName;
};

MDT_ENTITY_DEF(
  (PersonDataStruct),
  Person,
  (id, FieldFlag::IsPrimaryKey),
  (firstName, FieldMaxLength(250))
)

class PersonData : public DataTemplate<PersonEntity>
{
 public:

};

/*
 * Tests
 */

void SelectStatementTest::sandbox()
{

}

void SelectStatementTest::selectAllFromEntityTest()
{
  QString sql;
  QString expectedSql;

  sql = SqlSelectStatement::selectAllFromEntity<PersonEntity>();
  expectedSql = "SELECT * FROM Person";
  QCOMPARE(sql, expectedSql);

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  SelectStatementTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
