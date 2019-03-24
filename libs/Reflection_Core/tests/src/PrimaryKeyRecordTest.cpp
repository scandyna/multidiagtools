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
#include "PrimaryKeyRecordTest.h"
#include "Mdt/Reflection/PrimaryKeyRecord.h"
#include "Mdt/Reflection/PrimaryKeyRecordAlgorithm.h"
#include "Mdt/Reflection/PrimaryKey.h"
// #include "Mdt/Reflection/IdPrimaryKey.h"
// #include "Mdt/Reflection/AutoIncrementIdPrimaryKey.h"
// #include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include <QLatin1String>
#include <QStringList>
#include <QVariantList>
#include <array>
// #include <type_traits>

// #include <boost/fusion/container/vector.hpp>
// 
// #include "Mdt/Reflection/FieldAlgorithm.h"
// #include "Mdt/Reflection/PrimaryKeyAlgorithm.h"
// #include <QVariant>
// #include <boost/mpl/at.hpp>

using namespace Mdt::Reflection;

/**
template<typename PrimaryKey>
class PrimaryKeyRecord_VX
{
  using field_list = typename PrimaryKey::field_list;
  using first_field = typename boost::mpl::at_c<field_list, 0>::type;

 public:

  using value_type = typename TypeFromField<first_field>::type;

  static constexpr int fieldCount() noexcept
  {
    return primaryKeyFieldCount<PrimaryKey>();
  }

  value_type value(int fieldIndex) const
  {
    Q_ASSERT(fieldIndex >= 0);
    Q_ASSERT(fieldIndex < fieldCount());

  }

 private:

  std::array<value_type, fieldCount()> mRecord;
};
*/



/*
 * Struct
 */

struct CableLinkDataStruct
{
  int startConnectionId = 0;
  int endConnectionId = 0;
  QString identification;
};

MDT_REFLECT_STRUCT(
  (CableLinkDataStruct),
  CableLink,
  (startConnectionId),
  (endConnectionId),
  (identification, FieldFlag::IsRequired)
)

using CableLinkPrimaryKey = Mdt::Reflection::PrimaryKey<CableLinkDef::startConnectionId, CableLinkDef::endConnectionId>;
using CableLinkIdRecord = PrimaryKeyRecord<CableLinkPrimaryKey>;


struct PkTestStruct
{
  short int short_int_id;
  int int_id;
  qlonglong qlonglong_id;
  qulonglong qulonglong_id;
  QString str_A_id;
  QString str_B_id;
  QString str_C_id;
};

MDT_REFLECT_STRUCT(
  (PkTestStruct),
  PkTest,
  (short_int_id),
  (int_id),
  (qlonglong_id),
  (qulonglong_id),
  (str_A_id),
  (str_B_id),
  (str_C_id)
)

using PkStrA = PrimaryKey<PkTestDef::str_A_id>;
using PkStrARecord = PrimaryKeyRecord<PkStrA>;

/*
 * Functors
 */

// struct AddFieldNameAndValueToLists
// {
//   AddFieldNameAndValueToLists(QStringList & fieldNames, QVariantList & values)
//    : fieldNameList(fieldNames),
//      valueList(values)
//   {
//   }
// 
//   template<typename Field, typename Value>
//   void operator()(const Field, const Value & v)
//   {
//     fieldNameList << QString::fromLatin1( fieldName<Field>() );
//     valueList << v;
//   }
// 
//   QStringList & fieldNameList;
//   QVariantList & valueList;
// };

/*
 * Tests
 */

void PrimaryKeyRecordTest::primaryKeyRecordTest()
{
  CableLinkIdRecord pkr0;
  QCOMPARE(pkr0.fieldCount(), 2);
  QVERIFY(pkr0.variantValueAt(0).isNull());
  QVERIFY(pkr0.variantValueAt(1).isNull());
  QVERIFY(pkr0.isNull());

  CableLinkIdRecord pkr1;
  pkr1.setValueAt(0, 21);
  QCOMPARE(pkr1.variantValueAt(0), QVariant(21));
  QVERIFY(pkr1.variantValueAt(1).isNull());
  QVERIFY(pkr1.isNull());
  pkr1.setValueAt(1, 32);
  QCOMPARE(pkr1.variantValueAt(0), QVariant(21));
  QCOMPARE(pkr1.variantValueAt(1), QVariant(32));
  QVERIFY(!pkr1.isNull());

  pkr1.setValue<CableLinkDef::startConnectionId>(11);
  pkr1.setValue<CableLinkDef::endConnectionId>(12);
  QCOMPARE(pkr1.variantValueAt(0), QVariant(11));
  QCOMPARE(pkr1.variantValueAt(1), QVariant(12));
  QCOMPARE(pkr1.variantValue<CableLinkDef::startConnectionId>(), QVariant(11));
  QCOMPARE(pkr1.variantValue<CableLinkDef::endConnectionId>(), QVariant(12));
  QCOMPARE(pkr1.value<CableLinkDef::startConnectionId>(), 11);
  QCOMPARE(pkr1.value<CableLinkDef::endConnectionId>(), 12);

  PkStrARecord pkra;
  pkra.setValue<PkTestDef::str_A_id>(QLatin1String("a1"));
  QCOMPARE(pkra.value<PkTestDef::str_A_id>(), QLatin1String("a1"));
}

void PrimaryKeyRecordTest::primaryKeyRecordFromStructTest()
{
  CableLinkDataStruct link;
  link.startConnectionId = 14;
  link.endConnectionId = 31;

  CableLinkIdRecord pkr = Mdt::Reflection::primaryKeyRecordFromStruct<CableLinkIdRecord>(link);
  QCOMPARE(pkr.fieldCount(), 2);
  QCOMPARE(pkr.value<CableLinkDef::startConnectionId>(), 14);
  QCOMPARE(pkr.value<CableLinkDef::endConnectionId>(), 31);

  using Pk1 = PrimaryKey<PkTestDef::int_id>;
  using Pk2 = PrimaryKey<PkTestDef::str_A_id, PkTestDef::int_id>;
  using Pk3 = PrimaryKey<PkTestDef::int_id, PkTestDef::short_int_id, PkTestDef::str_B_id>;

  using Pkr1 = PrimaryKeyRecord<Pk1>;
  using Pkr2 = PrimaryKeyRecord<Pk2>;
  using Pkr3 = PrimaryKeyRecord<Pk3>;

  PkTestStruct s;
  s.short_int_id = 1;
  s.int_id = 10;
  s.str_A_id = QLatin1String("A");
  s.str_B_id = QLatin1String("B");

  Pkr1 pkr1 = primaryKeyRecordFromStruct<Pkr1>(s);
  QCOMPARE(pkr1.fieldCount(), 1);
  QCOMPARE(pkr1.value<PkTestDef::int_id>(), 10);

  Pkr2 pkr2 = primaryKeyRecordFromStruct<Pkr2>(s);
  QCOMPARE(pkr2.fieldCount(), 2);
  QCOMPARE(pkr2.value<PkTestDef::str_A_id>(), QLatin1String("A"));
  QCOMPARE(pkr2.value<PkTestDef::int_id>(), 10);

  Pkr3 pkr3 = primaryKeyRecordFromStruct<Pkr3>(s);
  QCOMPARE(pkr3.fieldCount(), 3);
  QCOMPARE(pkr3.value<PkTestDef::int_id>(), 10);
  QCOMPARE(pkr3.value<PkTestDef::short_int_id>(), 1);
  QCOMPARE(pkr3.value<PkTestDef::str_B_id>(), QLatin1String("B"));
}


/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  PrimaryKeyRecordTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
