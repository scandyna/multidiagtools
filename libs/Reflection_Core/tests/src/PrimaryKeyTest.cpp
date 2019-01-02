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
#include "PrimaryKeyTest.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include "Mdt/Reflection/IdPrimaryKey.h"
#include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
#include "Mdt/Reflection/PrimaryKeyAlgorithm.h"
#include <QStringList>

#include "Mdt/Reflection/FieldAttributes.h"
#include <boost/fusion/adapted/struct/adapt_assoc_struct.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>

using namespace Mdt::Reflection;

/*
 * Struct
 */

struct PkTestStruct
{
  int int_id;
  qulonglong qulonglong_id;
  QString str_A_id;
  QString str_B_id;
  QString str_C_id;
};

/*
 * Struct - auto generated stuff
 */

struct PkTestDef
{
  using reflected_struct = PkTestStruct;

  static constexpr const char *name()
  {
    return "PkTest";
  }

  struct int_id
  {
    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes();
    }
  };

  struct qulonglong_id
  {
    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes();
    }
  };

  struct str_A_id
  {
    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes();
    }
  };

  struct str_B_id
  {
    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes();
    }
  };

  struct str_C_id
  {
    static constexpr Mdt::Reflection::FieldAttributes fieldAttributes()
    {
      using namespace Mdt::Reflection;
      return Mdt::Reflection::FieldAttributes();
    }
  };
};

BOOST_FUSION_ADAPT_ASSOC_STRUCT(
  PkTestStruct,
  (int_id, PkTestDef::int_id)
  (qulonglong_id, PkTestDef::qulonglong_id)
  (str_A_id, PkTestDef::str_A_id)
  (str_B_id, PkTestDef::str_B_id)
  (str_C_id, PkTestDef::str_C_id)
)

/*
 * Helpers
 */

struct AddFieldNameToList
{
  QStringList fieldNameList;

  template<typename Field>
  void operator()(Field)
  {
  }
};

template<typename PrimaryKey>
QStringList primaryKeyToFieldNameList()
{
  AddFieldNameToList f;

  Mdt::Reflection::forEachPrimaryKeyField<PrimaryKey>(f);

  return f.fieldNameList;
}

/*
 * Type traits tests
 */

/*
 * Tests
 */

void PrimaryKeyTest::idPkTest()
{
  using Pk = Mdt::Reflection::IdPrimaryKey<PkTestDef, PkTestDef::int_id>;

  QCOMPARE(primaryKeyToFieldNameList<Pk>(), QStringList({"int_id"}));
}

void PrimaryKeyTest::autoIdPkTest()
{
  QFAIL("Not complete");
}

void PrimaryKeyTest::oneFieldPkTest()
{
  QFAIL("Not complete");
}

void PrimaryKeyTest::twoFieldPkTest()
{
  QFAIL("Not complete");
}

void PrimaryKeyTest::threeFieldPkTest()
{
  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  PrimaryKeyTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
