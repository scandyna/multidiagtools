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
#include "DataListTest.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataList.h"
#include <QVector>
#include <QList>
#include <vector>
#include <list>

using namespace Mdt::Entity;

void DataListTest::initTestCase()
{
}

void DataListTest::cleanupTestCase()
{
}

struct ArticleDataStruct
{
  qlonglong id;
  QString description;
};

MDT_ENTITY_DEF(
  (ArticleDataStruct),
  Article,
  (id, FieldFlag::IsRequired, FieldFlag::IsUnique),
  (description, FieldMaxLength(250))
)

class ArticleData : public Mdt::Entity::DataTemplate<ArticleEntity>
{
 public:

  void setId(int id)
  {
    dataStruct().id = id;
  }

  int id() const
  {
    return constDataStruct().id;
  }

};

template<typename Container>
using ArticleDataList = DataList<ArticleData, Container>;

class ExtendArticleDataList : public DataList<ArticleData>
{
};

/*
 * Tests
 */

template<typename Container>
void commonTestImpl()
{
  // Default constructor
  ArticleDataList<Container> l0;
  QCOMPARE(l0.size(), 0);
  QCOMPARE(l0.count(), 0);
  QVERIFY(l0.isEmpty());
  // Initial size with default constructed elements
  ArticleDataList<Container> l1(1);
  QCOMPARE(l1.size(), 1);
  QCOMPARE(l1.count(), 1);
  QVERIFY(!l1.isEmpty());
  // Set value
  l1[0].setId(11);
  QCOMPARE(l1.at(0).id(), 11);
  // Cleare
  l1.clear();
  QVERIFY(l1.isEmpty());
  // Add / remove
  ArticleData ad1;
  ad1.setId(1);
  l1.append(ad1);
  QCOMPARE(l1.count(), 1);
  QCOMPARE(l1.at(0).id(), 1);
  l1.removeLast();
  QVERIFY(l1.isEmpty());
}

void DataListTest::commonTest()
{
  commonTestImpl< std::vector<ArticleData> >();
  commonTestImpl< std::list<ArticleData> >();
  commonTestImpl< QVector<ArticleData> >();
  commonTestImpl< QList<ArticleData> >();
}

void DataListTest::extendTest()
{
  ExtendArticleDataList list;
  QCOMPARE(list.size(), 0);
  QCOMPARE(list.count(), 0);
  QVERIFY(list.isEmpty());

  ArticleData d1;
  d1.setId(1);
  list.append(d1);
  QCOMPARE(list.count(), 1);
  QCOMPARE(list.at(0).id(), 1);
  list[0].setId(10);
  QCOMPARE(list.at(0).id(), 10);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  DataListTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
