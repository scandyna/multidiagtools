/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "FieldTest.h"
#include "Mdt/Entity/SqlField.h"
#include "Mdt/Entity/Field.h"
#include "Mdt/Sql/Schema/Field.h"

using namespace Mdt::Entity;
using namespace Mdt::Sql::Schema;

void FieldTest::initTestCase()
{
}

void FieldTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void FieldTest::sqlFieldFromEntityFieldTest()
{
  /*
   * Simple field
   */
  MDT_ENTITY_FIELD(qlonglong, simpleId)
  const auto simpleIdSql = SqlField::fromEntityField(simpleId);
  QCOMPARE(simpleIdSql.name(), QString("simpleId_PK"));

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  FieldTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
