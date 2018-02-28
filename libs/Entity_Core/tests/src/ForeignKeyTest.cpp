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
#include "Mdt/Entity/ForeignKey.h"
#include "Mdt/Entity/Def.h"
#include <QString>

// #include <QDebug>

using namespace Mdt::Entity;

void ForeignKeyTest::initTestCase()
{
}

void ForeignKeyTest::cleanupTestCase()
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
 * tests
 */

void ForeignKeyTest::toFieldNameListTest()
{
  using Fk2 = ForeignKey<EmployeeEntity, EmployeeDef::teamIdField, EmployeeDef::idField>;
  using Fk3 = ForeignKey<EmployeeEntity, EmployeeDef::firstNameField, EmployeeDef::lastNameField, EmployeeDef::teamIdField>;

  QStringList fieldNames;

  fieldNames = TeamEmployeeFk::toFieldNameList();
  QCOMPARE(fieldNames.count(), 1);
  QCOMPARE(fieldNames.at(0), QString("teamId"));

  fieldNames = Fk2::toFieldNameList();
  QCOMPARE(fieldNames.count(), 2);
  QCOMPARE(fieldNames.at(0), QString("teamId"));
  QCOMPARE(fieldNames.at(1), QString("id"));

  fieldNames = Fk3::toFieldNameList();
  QCOMPARE(fieldNames.count(), 3);
  QCOMPARE(fieldNames.at(0), QString("firstName"));
  QCOMPARE(fieldNames.at(1), QString("lastName"));
  QCOMPARE(fieldNames.at(2), QString("teamId"));
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
