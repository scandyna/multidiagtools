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
#include "UniqueIdTest.h"
#include "Mdt/Entity/IntegralUniqueIdTemplate.h"
#include <QMetaType>
#include <QVariant>

// #include <QDebug>

class ArticleId : public Mdt::Entity::IntegralUniqueIdTemplate<>
{
 public:

  using IntegralUniqueIdTemplate<>::IntegralUniqueIdTemplate;
};

class MetaTypeKnownId : public Mdt::Entity::IntegralUniqueIdTemplate<>
{
 public:

  using IntegralUniqueIdTemplate<>::IntegralUniqueIdTemplate;
};
Q_DECLARE_METATYPE(MetaTypeKnownId)

/*
 * Tests
 */

static_assert( ArticleId(1) == ArticleId(1) ,"" );
static_assert( ArticleId(1) != ArticleId(2) ,"" );
static_assert( ArticleId(1) < ArticleId(2) ,"" );
static_assert( ArticleId(1) <= ArticleId(2) ,"" );
static_assert( ArticleId(2) > ArticleId(1) ,"" );
static_assert( ArticleId(2) >= ArticleId(1) ,"" );

void UniqueIdTest::articleIdTest()
{
  ArticleId id0;
  QCOMPARE(id0.value(), 0ull);
  QVERIFY(id0.isNull());

  ArticleId id1(1);
  QCOMPARE(id1.value(), 1ull);
  QVERIFY(!id1.isNull());
}

void UniqueIdTest::QVariantTest()
{
  MetaTypeKnownId id1(1);
  auto vId1 = QVariant::fromValue(id1);
  QCOMPARE(vId1.value<MetaTypeKnownId>().value(), 1ull);
}

void UniqueIdTest::fromQVariantTest()
{
  QVariant vId25(25);

  const auto id = ArticleId::fromQVariant(vId25);
  QVERIFY(!id.isNull());
  QCOMPARE(id.value(), 25u);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  UniqueIdTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
