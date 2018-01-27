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
#include "TableModelRecordAdapterTest.h"
#include "Mdt/Entity/TableModelRecordAdapter.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include <boost/fusion/container.hpp>

using namespace Mdt::Entity;

void TableModelRecordAdapterTest::initTestCase()
{
}

void TableModelRecordAdapterTest::cleanupTestCase()
{
}

struct OneMemberStruct
{
  int id;
};

MDT_ENTITY_DEF(
  (OneMemberStruct),
  OneMember,
  (id)
)

class OneMemberData : public DataTemplate<OneMemberStruct, OneMemberDef>
{
};

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
  double qty;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsRequired, FieldFlag::IsUnique),
  (description, FieldMaxLength(250)),
  (qty)
)

class ArticleData : public DataTemplate<ArticleDataStruct, ArticleDef>
{
};

/*
 * Tests
 */

template<typename Data>
void containerSizeTestImpl(int expectedSize)
{
  TableModelRecordAdapter<Data> adapter;
  Data data;
  QCOMPARE(adapter.containerSize(data), expectedSize);
}

void TableModelRecordAdapterTest::containerSizeTest()
{
//   containerSizeTestImpl< boost::fusion::vector<> >(0);
//   containerSizeTestImpl< boost::fusion::vector<int> >(1);
//   containerSizeTestImpl< ArticleDataStruct >(3);
  containerSizeTestImpl<OneMemberData>(1);
  containerSizeTestImpl<ArticleData>(3);
}

template<typename Data>
void containerIsEmptyTestImpl(bool expectedIsEmpty)
{
  TableModelRecordAdapter<Data> adapter;
  Data data;
  QCOMPARE(adapter.containerIsEmpty(data), expectedIsEmpty);
}

void TableModelRecordAdapterTest::containerIsEmptyTest()
{
//   containerIsEmptyTestImpl< boost::fusion::vector<> >(true);
//   containerIsEmptyTestImpl< boost::fusion::vector<int> >(false);
//   containerIsEmptyTestImpl< ArticleDataStruct >(false);
  containerIsEmptyTestImpl<ArticleData>(false);
  containerIsEmptyTestImpl<OneMemberData>(false);
}

void TableModelRecordAdapterTest::valueAtIndexTest()
{
  TableModelRecordAdapter<ArticleData> adapter;
  ArticleData article;

  article.dataStruct().id = 125;
  article.dataStruct().description = "Article 125";
  article.dataStruct().qty = 2.3;
  QCOMPARE(adapter.valueAtIndex(article, 0), QVariant(125));
  QCOMPARE(adapter.valueAtIndex(article, 1), QVariant("Article 125"));
  QCOMPARE(adapter.valueAtIndex(article, 2), QVariant(2.3));
}

void TableModelRecordAdapterTest::setValueAtIndexTest()
{
  TableModelRecordAdapter<ArticleData> adapter;
  ArticleData article;

  article.dataStruct().id = 0;
  article.dataStruct().qty = 0.0;
  QCOMPARE(adapter.valueAtIndex(article, 0), QVariant(0));
  QVERIFY(adapter.valueAtIndex(article, 1).isNull());
  QCOMPARE(adapter.valueAtIndex(article, 2), QVariant(0.0));
  adapter.setValueAtIndex(article, 0, 5);
  adapter.setValueAtIndex(article, 1, "Article 5");
  adapter.setValueAtIndex(article, 2, 3.0);
  QCOMPARE(adapter.valueAtIndex(article, 0), QVariant(5));
  QCOMPARE(adapter.valueAtIndex(article, 1), QVariant("Article 5"));
  QCOMPARE(adapter.valueAtIndex(article, 2), QVariant(3.0));
}

void TableModelRecordAdapterTest::initializeContainerTest()
{
  TableModelRecordAdapter<ArticleData> adapter;

  auto articleA = adapter.initializeContainer(3, 1);
  QCOMPARE(articleA.dataStruct().id, 1);
  QCOMPARE(articleA.dataStruct().description, QString("1"));
  QCOMPARE(articleA.dataStruct().qty, 1.0);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  TableModelRecordAdapterTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
