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
#include "RelationTest.h"
#include "Mdt/Entity/Relation.h"
#include "Mdt/Entity/Def.h"
#include <QStringList>

using namespace Mdt::Entity;

void RelationTest::initTestCase()
{
}

void RelationTest::cleanupTestCase()
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

struct TwoFieldPkDataStruct
{
  int pk1;
  QString pk2;
};

MDT_ENTITY_DEF(
  (TwoFieldPkDataStruct),
  TwoFieldPk,
  (pk1, FieldFlag::IsPrimaryKey),
  (pk2, FieldFlag::IsPrimaryKey)
)

struct TwoFieldFkDataStruct
{
  int fk1;
  QString fk2;
};

MDT_ENTITY_DEF(
  (TwoFieldFkDataStruct),
  TwoFieldFk,
  (fk1),
  (fk2)
)

using TwoFieldRelation = Relation<TwoFieldPkEntity, TwoFieldFkEntity, TwoFieldFkDef::fk1Field, TwoFieldFkDef::fk2Field>;

/*
 * Tests
 */

struct ToStringList
{
  template<typename Field>
  void operator()(const Field &)
  {
    fieldNameList.append( Field::fieldName() );
  }

  QStringList fieldNameList;
};

void RelationTest::forEachForeignFieldTest()
{
  ToStringList f;
  forEachRelationForeignField<TeamEmployeeRelation>(f);
  QCOMPARE(f.fieldNameList.size(), 1);
  QCOMPARE(f.fieldNameList.at(0), QString("teamId"));

  f.fieldNameList.clear();
  forEachRelationForeignField<TwoFieldRelation>(f);
  QCOMPARE(f.fieldNameList.size(), 2);
  QCOMPARE(f.fieldNameList.at(0), QString("fk1"));
  QCOMPARE(f.fieldNameList.at(1), QString("fk2"));
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  RelationTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
