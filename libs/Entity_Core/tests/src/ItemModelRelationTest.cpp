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
#include "ItemModelRelationTest.h"
#include "Mdt/Entity/Relation.h"
#include "Mdt/Entity/ItemModelRelation.h"
#include "Mdt/Entity/Def.h"

using namespace Mdt::Entity;

void ItemModelRelationTest::initTestCase()
{
}

void ItemModelRelationTest::cleanupTestCase()
{
}

/*
 * Entities
 */

struct TeamDataStruct
{
  qulonglong id;
  QString name;
};

MDT_ENTITY_DEF(
  (TeamDataStruct),
  Team,
  (id, FieldFlag::IsPrimaryKey),
  (name)
)

struct EmployeeDataStruct
{
  qulonglong id;
  QString firstName;
  qulonglong teamId;
};

MDT_ENTITY_DEF(
  (EmployeeDataStruct),
  Employee,
  (id, FieldFlag::IsPrimaryKey),
  (firstName),
  (teamId)
)

using TeamEmployeeRelation = Relation<TeamEntity, EmployeeEntity, EmployeeDef::teamIdField>;

struct ParentDataStruct2
{
  qulonglong idA;
  QString var1;
  qulonglong idB;
};

MDT_ENTITY_DEF(
  (ParentDataStruct2),
  Parent2,
  (idA, FieldFlag::IsPrimaryKey),
  (var1),
  (idB, FieldFlag::IsPrimaryKey)
)

struct ChildDataStruct2
{
  qulonglong id;
  qulonglong parentIdA;
  QString var1;
  qulonglong parentIdB;
};

MDT_ENTITY_DEF(
  (ChildDataStruct2),
  Child2,
  (id, FieldFlag::IsPrimaryKey),
  (parentIdA),
  (var1),
  (parentIdB)
)

using Relation2 = Relation<Parent2Entity, Child2Entity, Child2Def::parentIdAField, Child2Def::parentIdBField>;

/*
 * Tests
 */

void ItemModelRelationTest::keyFromEntityRelationTest()
{
  const auto key = ItemModelRelation::keyFromRelation<TeamEmployeeRelation>();
  QCOMPARE(key.columnPairCount(), 1);
  QCOMPARE(key.columnPairAt(0).parentModelColumn(), 0);
  QCOMPARE(key.columnPairAt(0).childModelColumn(), 2);

  const auto key2 = ItemModelRelation::keyFromRelation<Relation2>();
  QCOMPARE(key2.columnPairCount(), 2);
  QCOMPARE(key2.columnPairAt(0).parentModelColumn(), 0);
  QCOMPARE(key2.columnPairAt(0).childModelColumn(), 1);
  QCOMPARE(key2.columnPairAt(1).parentModelColumn(), 2);
  QCOMPARE(key2.columnPairAt(1).childModelColumn(), 3);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ItemModelRelationTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
