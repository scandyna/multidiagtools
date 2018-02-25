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
#include "ItemModelPrimaryKeyTest.h"
#include "Mdt/Entity/ItemModelPrimaryKey.h"
#include "Mdt/Entity/Def.h"

using namespace Mdt::Entity;

void ItemModelPrimaryKeyTest::initTestCase()
{
}

void ItemModelPrimaryKeyTest::cleanupTestCase()
{
}

/*
 * Entities
 */

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsPrimaryKey),
  (description, FieldFlag::IsRequired, FieldMaxLength(250))
)

struct LinkDataStruct
{
  QString code;
  qlonglong startId;
  qlonglong endId;
  QString linkName;
};

MDT_ENTITY_DEF(
  (LinkDataStruct),
  Link,
  (code),
  (startId, FieldFlag::IsPrimaryKey),
  (endId, FieldFlag::IsPrimaryKey),
  (linkName, FieldFlag::IsRequired, FieldMaxLength(100))
)

/*
 * Tests
 */

void ItemModelPrimaryKeyTest::impkFromEntityTest()
{
  const auto articleImPk = ItemModelPrimaryKey::fromEntity<ArticleEntity>();
  QCOMPARE(articleImPk.columnCount(), 1);
  QCOMPARE(articleImPk.columnAt(0), 0);

  const auto linkImPk = ItemModelPrimaryKey::fromEntity<LinkEntity>();
  QCOMPARE(linkImPk.columnCount(), 2);
  QCOMPARE(linkImPk.columnAt(0), 1);
  QCOMPARE(linkImPk.columnAt(1), 2);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ItemModelPrimaryKeyTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
