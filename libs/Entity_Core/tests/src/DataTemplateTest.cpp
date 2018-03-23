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
#include "DataTemplateTest.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/Debug.h"

#include <QDebug>

void DataTemplateTest::initTestCase()
{
}

void DataTemplateTest::cleanupTestCase()
{
}

/*
 * Tests
 */

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
};

/**
 * For tests, create structs:
 *
 * struct NotCopiable
 * {
 * };
 *
 * struct NotMovable
 * {
 * };
 */

void DataTemplateTest::dataUsageTest()
{
  ArticleData art1;
  const auto def = art1.def();
  QCOMPARE(def.entityName(), QString("Article"));
  QCOMPARE(def.id().fieldName(), QString("id"));
  QCOMPARE(def.description().fieldName(), QString("description"));
  art1.dataStruct().id = 1;
  art1.dataStruct().description = "Article 1";
  QCOMPARE(art1.constDataStruct().id, 1);
  QCOMPARE(art1.constDataStruct().description, QString("Article 1"));
  Mdt::Entity::printEntityDataToConsole(art1);
}

void DataTemplateTest::fieldIndexTest()
{
  ArticleData article;
  QCOMPARE(article.fieldIndex(article.def().id()), 0);
  QCOMPARE(article.fieldIndex(article.def().description()), 1);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  DataTemplateTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
