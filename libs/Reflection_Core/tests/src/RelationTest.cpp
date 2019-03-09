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
#include "RelationTest.h"
#include "Mdt/Reflection/Relation.h"
#include "Mdt/Reflection/RelationAlgorithm.h"
#include "Mdt/Reflection/PrimaryKey.h"
#include "Mdt/Reflection/ReflectStruct.h"
#include <QLatin1String>
#include <QStringList>
#include <type_traits>

using namespace Mdt::Reflection;


struct PrimaryEntityDataStruct
{
  int p_id;
  QString p_A_id;
  QString p_B_id;
  QString p_C_id;
};

MDT_REFLECT_STRUCT(
  (PrimaryEntityDataStruct),
  PrimaryEntity,
  (p_id),
  (p_A_id),
  (p_B_id),
  (p_C_id)
)

struct RelatedEntityDataStruct
{
  int f_id;
  QString f_A_id;
  QString f_B_id;
  QString f_C_id;
};

MDT_REFLECT_STRUCT(
  (RelatedEntityDataStruct),
  RelatedEntity,
  (f_id),
  (f_A_id),
  (f_B_id),
  (f_C_id)
)

/*
 * Compile time tests
 */

void relationStructDefsTest()
{
  using Pk = PrimaryKey<PrimaryEntityDef::p_id>;
  using Relation = Relation<Pk, RelatedEntityDef::f_id>;

  static_assert( std::is_same< Relation::primary_struct_def, PrimaryEntityDef >::value , "" );
  static_assert( std::is_same< Relation::related_struct_def, RelatedEntityDef >::value , "" );
}

/*
 * Tests
 */

void RelationTest::namesFromRelationTest()
{
  using Pk = PrimaryKey<PrimaryEntityDef::p_id>;
  using Relation = Relation<Pk, RelatedEntityDef::f_id>;

  QCOMPARE( primaryNameFromRelation<Relation>(), "PrimaryEntity" );
  QCOMPARE( relatedNameFromRelation<Relation>(), "RelatedEntity" );
  QCOMPARE( primaryNameFromRelationQString<Relation>(), QLatin1String("PrimaryEntity") );
  QCOMPARE( relatedNameFromRelationQString<Relation>(), QLatin1String("RelatedEntity") );
}

struct RelationToFieldPairNames
{
  RelationToFieldPairNames(QStringList & sl)
   : mFieldPairNames(sl)
  {
  }

  template<typename PrimaryStructField, typename RelatedStructField>
  void operator()(PrimaryStructField, RelatedStructField)
  {
    const QString s = QLatin1String( fieldName<PrimaryStructField>() ) + QLatin1String("-") + QLatin1String( fieldName<RelatedStructField>() );
    mFieldPairNames << s;
  }

 private:

  QStringList & mFieldPairNames;
};

void RelationTest::forEachFieldPairInRelationTest()
{
  using Pk = PrimaryKey<PrimaryEntityDef::p_A_id, PrimaryEntityDef::p_B_id, PrimaryEntityDef::p_C_id>;
  using Relation = Relation<Pk, RelatedEntityDef::f_A_id, RelatedEntityDef::f_B_id, RelatedEntityDef::f_C_id>;

  QStringList fieldPairNames;
  RelationToFieldPairNames f(fieldPairNames);

  forEachFieldPairInRelation<Relation>(f);
  QCOMPARE( fieldPairNames, QStringList({QLatin1String("p_A_id-f_A_id"),QLatin1String("p_B_id-f_B_id"),QLatin1String("p_C_id-f_C_id")}) );
}

void RelationTest::fieldNameListFromRelationOneFieldTest()
{
  using Pk = PrimaryKey<PrimaryEntityDef::p_id>;
  using Relation = Relation<Pk, RelatedEntityDef::f_id>;

  QCOMPARE( primaryStructFieldNameListFromRelation<Relation>(), QStringList({QLatin1String("p_id")}) );
  QCOMPARE( relatedStructFieldNameListFromRelation<Relation>(), QStringList({QLatin1String("f_id")}) );
}

void RelationTest::fieldNameListFromRelationTwoFieldTest()
{
  using Pk = PrimaryKey<PrimaryEntityDef::p_A_id, PrimaryEntityDef::p_B_id>;
  using Relation = Relation<Pk, RelatedEntityDef::f_A_id, RelatedEntityDef::f_B_id>;

  QCOMPARE( primaryStructFieldNameListFromRelation<Relation>(), QStringList({QLatin1String("p_A_id"),QLatin1String("p_B_id")}) );
  QCOMPARE( relatedStructFieldNameListFromRelation<Relation>(), QStringList({QLatin1String("f_A_id"),QLatin1String("f_B_id")}) );
}

void RelationTest::fieldNameListFromRelationThreeFieldTest()
{
  using Pk = PrimaryKey<PrimaryEntityDef::p_A_id, PrimaryEntityDef::p_B_id, PrimaryEntityDef::p_C_id>;
  using Relation = Relation<Pk, RelatedEntityDef::f_A_id, RelatedEntityDef::f_B_id, RelatedEntityDef::f_C_id>;

  QCOMPARE( primaryStructFieldNameListFromRelation<Relation>(), QStringList({QLatin1String("p_A_id"),QLatin1String("p_B_id"),QLatin1String("p_C_id")}) );
  QCOMPARE( relatedStructFieldNameListFromRelation<Relation>(), QStringList({QLatin1String("f_A_id"),QLatin1String("f_B_id"),QLatin1String("f_C_id")}) );
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
