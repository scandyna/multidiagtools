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
#include "ValueTest.h"
#include "Mdt/Entity/Value.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/IntegralUniqueIdTemplate.h"
#include "Mdt/Entity/DataTemplate.h"

using namespace Mdt::Entity;

/*
 * Entities
 */

class ArticleId : public IntegralUniqueIdTemplate<ArticleId, int>
{
 public:

  using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
};

struct ArticleDataStruct
{
  int id;
  QString description;
  int qty;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsPrimaryKey),
  (description, FieldFlag::IsRequired, FieldMaxLength(250)),
  (qty)
)

class ArticleData : public DataTemplate<ArticleEntity>
{
};

struct ArticleTypeDataStruct
{
  QString code;
  QString label;
};

MDT_ENTITY_DEF(
  (ArticleTypeDataStruct),
  ArticleType,
  (code, FieldFlag::IsPrimaryKey),
  (label)
)

/*
 * Tests
 */

void ValueTest::valueTest()
{
  ArticleDataStruct ads;
  int id;
  QString description;

  ads.id = 15;
  ads.description = "DES 15";
  id = value<ArticleDataStruct, ArticleDef::idField>(ads);
  QCOMPARE(id, 15);
  description = value<ArticleDataStruct, ArticleDef::descriptionField>(ads);
  QCOMPARE(description, QString("DES 15"));
}

void ValueTest::setValueTest()
{
  ArticleDataStruct ads;

  ads.id = 3;
  ads.description = "DES 3";
  ads.qty = 15;

  setValue<ArticleDataStruct, ArticleDef::idField>(ads, 7);
  QCOMPARE(ads.id, 7);
  QCOMPARE(ads.description, QString("DES 3"));
  QCOMPARE(ads.qty, 15);

  setValue<ArticleDataStruct, ArticleDef::descriptionField>(ads, "DES 7");
  QCOMPARE(ads.id, 7);
  QCOMPARE(ads.description, QString("DES 7"));
  QCOMPARE(ads.qty, 15);

  setValue<ArticleDataStruct, ArticleDef::qtyField>(ads, 25);
  QCOMPARE(ads.id, 7);
  QCOMPARE(ads.description, QString("DES 7"));
  QCOMPARE(ads.qty, 25);
}

void ValueTest::uniqueIdValueTest()
{
  ArticleData article;

  article.dataStruct().id = 2;
  article.dataStruct().description = "DESC 2";
  article.dataStruct().qty = 11;

  const auto id = uniqueIdValue<ArticleData, ArticleId>(article);
  QCOMPARE(id.value(), 2);

  setUniqueIdValue<ArticleData, ArticleId>(article, ArticleId(5));
  QCOMPARE(article.constDataStruct().id, 5);
  QCOMPARE(article.constDataStruct().description, QString("DESC 2"));
  QCOMPARE(article.constDataStruct().qty, 11);
}

void ValueTest::isNullUniqueIdTest()
{
  bool isNullUid;

  ArticleDataStruct ads;

  ads.id = 0;
  ads.description = "A";
  ads.qty = 0;
  isNullUid = isNullUniqueId<ArticleDataStruct, ArticleDef::idField, ArticleId>(ads);
  QVERIFY(isNullUid);
  isNullUid = isNullUniqueId<ArticleDataStruct, ArticleDef::descriptionField, ArticleId>(ads);
  QVERIFY(!isNullUid);
  isNullUid = isNullUniqueId<ArticleDataStruct, ArticleDef::qtyField, ArticleId>(ads);
  QVERIFY(!isNullUid);

  ads.id = 1;
  ads.description = "A";
  ads.qty = 0;
  isNullUid = isNullUniqueId<ArticleDataStruct, ArticleDef::idField, ArticleId>(ads);
  QVERIFY(!isNullUid);
  isNullUid = isNullUniqueId<ArticleDataStruct, ArticleDef::descriptionField, ArticleId>(ads);
  QVERIFY(!isNullUid);
  isNullUid = isNullUniqueId<ArticleDataStruct, ArticleDef::qtyField, ArticleId>(ads);
  QVERIFY(!isNullUid);

  ArticleTypeDataStruct atds;
  isNullUid = isNullUniqueId<ArticleTypeDataStruct, ArticleTypeDef::codeField, ArticleId>(atds);
  QVERIFY(!isNullUid);

}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ValueTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
