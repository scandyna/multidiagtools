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
#include "ItemModelForeignKeyTest.h"
#include "Mdt/Entity/ItemModelForeignKey.h"
#include "Mdt/Entity/Def.h"
#include <QString>

using namespace Mdt::Entity;

void ItemModelForeignKeyTest::initTestCase()
{
}

void ItemModelForeignKeyTest::cleanupTestCase()
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
  (name, FieldFlag::IsRequired, FieldMaxLength(250))
)

struct EmployeeDataStruct
{
  qulonglong id;
  QString firstName;
  QString lastName;
  qulonglong teamId;
};

MDT_ENTITY_DEF(
  (EmployeeDataStruct),
  Employee,
  (id, FieldFlag::IsPrimaryKey),
  (firstName, FieldFlag::IsRequired, FieldMaxLength(120)),
  (lastName, FieldFlag::IsRequired, FieldMaxLength(150)),
  (teamId)
)

using TeamEmployeeFk = ForeignKey<EmployeeEntity, EmployeeDef::teamIdField>;

/*
 * Tests
 */

void ItemModelForeignKeyTest::imfkFromEntityFkTest()
{
  using Fk2 = ForeignKey<EmployeeEntity, EmployeeDef::teamIdField, EmployeeDef::idField>;
  using Fk3 = ForeignKey<EmployeeEntity, EmployeeDef::firstNameField, EmployeeDef::lastNameField, EmployeeDef::teamIdField>;

  const auto teamEmployeeImFk = ItemModelForeignKey::fromEntityForeignKey<TeamEmployeeFk>();
  QCOMPARE(teamEmployeeImFk.columnCount(), 1);
  QCOMPARE(teamEmployeeImFk.columnAt(0), 3);

  const auto imFk2 = ItemModelForeignKey::fromEntityForeignKey<Fk2>();
  QCOMPARE(imFk2.columnCount(), 2);
  QCOMPARE(imFk2.columnAt(0), 3);
  QCOMPARE(imFk2.columnAt(1), 0);

  const auto imFk3 = ItemModelForeignKey::fromEntityForeignKey<Fk3>();
  QCOMPARE(imFk3.columnCount(), 3);
  QCOMPARE(imFk3.columnAt(0), 1);
  QCOMPARE(imFk3.columnAt(1), 2);
  QCOMPARE(imFk3.columnAt(2), 3);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ItemModelForeignKeyTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
