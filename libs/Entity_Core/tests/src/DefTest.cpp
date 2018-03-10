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
#include "DefTest.h"
#include "Mdt/Entity/Def.h"
#include <QString>
#include <type_traits>

/*
 * Note: to check that the macro works properly,
 * we not declare using namespace Mdt::Entity
 */

void DefTest::initTestCase()
{
}

void DefTest::cleanupTestCase()
{
}

/*
 * Tests
 */

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
  QString remarks;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsRequired | FieldFlag::IsUnique),
  (description, FieldFlag::IsRequired, FieldMaxLength(250)),
  (remarks)
)

namespace A{
  struct SellerDataStruct
  {
    qlonglong id;
    QString name;
  };
}

MDT_ENTITY_DEF(
  (A, SellerDataStruct),
  Seller,
  (id, FieldFlag::IsRequired),
  (name)
)

static_assert(std::is_same< ArticleEntity::def_type, ArticleDef >::value, "");
static_assert(std::is_same< ArticleEntity::data_struct_type, ArticleDataStruct >::value, "");
static_assert(std::is_same< decltype(ArticleEntity::data_struct_type::id), qlonglong >::value, "");
static_assert(std::is_same< decltype(ArticleEntity::data_struct_type::description), QString >::value, "");
static_assert(std::is_same< decltype(ArticleEntity::data_struct_type::remarks), QString >::value, "");

static_assert(std::is_same< A::SellerEntity::def_type, A::SellerDef >::value, "");
static_assert(std::is_same< A::SellerEntity::data_struct_type, A::SellerDataStruct >::value, "");
static_assert(std::is_same< decltype(A::SellerEntity::data_struct_type::id), qlonglong >::value, "");
static_assert(std::is_same< decltype(A::SellerEntity::data_struct_type::name), QString >::value, "");

void DefTest::defUsageTest()
{
  constexpr ArticleDef articleDef;
  QCOMPARE(articleDef.entityName(), QString("Article"));
  QCOMPARE(articleDef.id().fieldName(), QString("id"));
  QCOMPARE(articleDef.description().fieldName(), QString("description"));
  QCOMPARE(articleDef.remarks().fieldName(), QString("remarks"));
  QVERIFY(articleDef.id().fieldAttributes().isRequired());
  QVERIFY(articleDef.id().fieldAttributes().isUnique());
  QVERIFY(articleDef.id().fieldAttributes().maxLength() < 1);
  QVERIFY(articleDef.description().fieldAttributes().isRequired());
  QVERIFY(!articleDef.description().fieldAttributes().isUnique());
  QCOMPARE(articleDef.description().fieldAttributes().maxLength(), 250);
  QVERIFY(!articleDef.remarks().fieldAttributes().isRequired());
  QVERIFY(!articleDef.remarks().fieldAttributes().isUnique());
  QVERIFY(articleDef.remarks().fieldAttributes().maxLength() < 1);

  constexpr ArticleEntity::def_type articleDef2;
  QCOMPARE(articleDef2.entityName(), QString("Article"));
  QCOMPARE(articleDef2.id().fieldName(), QString("id"));
  QCOMPARE(articleDef2.description().fieldName(), QString("description"));
  QCOMPARE(articleDef2.remarks().fieldName(), QString("remarks"));

  const A::SellerDef sellerDef;
  QCOMPARE(sellerDef.entityName(), QString("Seller"));
  QCOMPARE(sellerDef.id().fieldName(), QString("id"));
  QCOMPARE(sellerDef.name().fieldName(), QString("name"));

  constexpr A::SellerEntity::def_type sellerDef2;
  QCOMPARE(sellerDef2.entityName(), QString("Seller"));
  QCOMPARE(sellerDef2.id().fieldName(), QString("id"));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  DefTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
