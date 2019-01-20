/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "UniqueConstraintTest.h"
#include "Mdt/Reflection/UniqueConstraint.h"
#include "Mdt/Reflection/UniqueConstraintAlgorithm.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>
#include <type_traits>

using namespace Mdt::Reflection;

/*
 * Struct
 */

struct TestStruct
{
  int id;
  QString code;
  QString type;
};

MDT_REFLECT_STRUCT(
  (TestStruct),
  Test,
  (id),
  (code),
  (type)
)


/*
 * Tests
 */

void UniqueConstraintTest::nameFromUniqueConstraintTest()
{
  using UC = UniqueConstraint<TestDef::id>;

  QCOMPARE(nameFromUniqueConstraint<UC>(), "Test");
}

void UniqueConstraintTest::oneFieldConstraintTest()
{
  using UC = UniqueConstraint<TestDef::id>;

  QCOMPARE(fieldNameListFromUniqueConstraint<UC>(), QStringList({QLatin1String("id")}));
}

void UniqueConstraintTest::twoFieldConstraintTest()
{
  using UCA = UniqueConstraint<TestDef::id, TestDef::code>;
  using UCB = UniqueConstraint<TestDef::id, TestDef::type>;
  using UCC = UniqueConstraint<TestDef::code, TestDef::id>;

  QCOMPARE(fieldNameListFromUniqueConstraint<UCA>(), QStringList({QLatin1String("id"),QLatin1String("code")}));
  QCOMPARE(fieldNameListFromUniqueConstraint<UCB>(), QStringList({QLatin1String("id"),QLatin1String("type")}));
  QCOMPARE(fieldNameListFromUniqueConstraint<UCC>(), QStringList({QLatin1String("code"),QLatin1String("id")}));
}

void UniqueConstraintTest::threeFieldConstraintTest()
{
  using UC = UniqueConstraint<TestDef::id, TestDef::code, TestDef::type>;

  QCOMPARE(fieldNameListFromUniqueConstraint<UC>(), QStringList({QLatin1String("id"),QLatin1String("code"),QLatin1String("type")}));
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  UniqueConstraintTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
