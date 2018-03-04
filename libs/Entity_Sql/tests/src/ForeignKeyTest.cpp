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
#include "ForeignKeyTest.h"
#include "Mdt/Entity/SqlForeignKey.h"
#include "Mdt/Entity/Relation.h"
#include "Mdt/Entity/Def.h"
#include <QString>
#include <QStringList>

using namespace Mdt::Entity;
namespace Sql = Mdt::Sql;

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

void ForeignKeyTest::fromEntityRelationTest()
{
  using Sql::Schema::ForeignKeyAction;

  Sql::Schema::ForeignKeySettings sqlFkSettings;
  sqlFkSettings.setOnDeleteAction(ForeignKeyAction::Restrict);
  sqlFkSettings.setOnUpdateAction(ForeignKeyAction::Cascade);
  sqlFkSettings.setIndexed(true);

  const auto fk = SqlForeignKey::fromEntityRelation<TeamEmployeeRelation>(sqlFkSettings);
  QCOMPARE(fk.parentTableName(), QString("Team"));
  QCOMPARE(fk.childTableName(), QString("Employee"));
  QCOMPARE(fk.parentTableFieldNameList(), QStringList({"id"}));
  QCOMPARE(fk.childTableFieldNameList(), QStringList({"teamId"}));
  QCOMPARE(fk.onDeleteAction(), ForeignKeyAction::Restrict);
  QCOMPARE(fk.onUpdateAction(), ForeignKeyAction::Cascade);
  QVERIFY(fk.isIndexed());

  auto fk2 = SqlForeignKey::fromEntityRelation<Relation2>(sqlFkSettings);
  QCOMPARE(fk2.parentTableName(), QString("Parent2"));
  QCOMPARE(fk2.childTableName(), QString("Child2"));
  QCOMPARE(fk2.parentTableFieldNameList(), QStringList({"idA","idB"}));
  QCOMPARE(fk2.childTableFieldNameList(), QStringList({"parentIdA","parentIdB"}));
  QCOMPARE(fk2.onDeleteAction(), ForeignKeyAction::Restrict);
  QCOMPARE(fk2.onUpdateAction(), ForeignKeyAction::Cascade);
  QVERIFY(fk2.isIndexed());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ForeignKeyTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
