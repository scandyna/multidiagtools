/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
  (id, FieldFlag::IsRequired, FieldFlag::IsUnique),
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

void DefTest::defUsageTest()
{
  const ArticleDef articleDef;
  QCOMPARE(articleDef.entityName(), QString("Article"));
  QCOMPARE(articleDef.id.fieldName(), QString("id"));
  QCOMPARE(articleDef.description.fieldName(), QString("description"));
  QCOMPARE(articleDef.remarks.fieldName(), QString("remarks"));

  const A::SellerDef sellerDef;
  QCOMPARE(sellerDef.entityName(), QString("Seller"));
  QCOMPARE(sellerDef.id.fieldName(), QString("id"));
  QCOMPARE(sellerDef.name.fieldName(), QString("name"));
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
