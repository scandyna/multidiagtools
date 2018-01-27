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
#include "TableModelTest.h"
#include "Mdt/Entity/ReadOnlyTableModel.h"
#include "Mdt/Entity/EditableTableModel.h"
#include "Mdt/Entity/RowResizableTableModel.h"
#include "Mdt/Entity/Def.h"
#include "Mdt/Entity/DataTemplate.h"
#include "Mdt/TestLib/ItemModel.h"
#include "Mdt/TestLib/ItemModelInsertRowTest.h"
#include "Mdt/TestLib/ItemModelRemoveRowTest.h"

#include <QApplication>
#include <QTableView>
#include <vector>

using namespace Mdt::Entity;
using namespace Mdt::TestLib;

void TableModelTest::initTestCase()
{
}

void TableModelTest::cleanupTestCase()
{
}

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

void checkArticleModelHeaderData(const QAbstractItemModel & model)
{
  QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant("id"));
  QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant("description"));
  QCOMPARE(model.headerData(2, Qt::Horizontal), QVariant("qty"));
}

void TableModelTest::readOnlyTest()
{
  using ArticleDataList = std::vector<ArticleData>;

  ArticleDataList articles;
  ArticleData article;
  article.dataStruct().id = 1;
  article.dataStruct().description = "One";
  article.dataStruct().qty = 1.0;
  articles.push_back(article);
  article.dataStruct().id = 2;
  article.dataStruct().description = "Two";
  article.dataStruct().qty = 5.2;
  articles.push_back(article);

  ReadOnlyTableModel<ArticleDataList> model;
  model.setTable(articles);

  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.columnCount(), 3);
  checkArticleModelHeaderData(model);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("One"));
  QCOMPARE(getModelData(model, 0, 2), QVariant(1.0));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("Two"));
  QCOMPARE(getModelData(model, 1, 2), QVariant(5.2));

//   QTableView tv;
//   tv.setModel(&model);
//   tv.show();
//   while(tv.isVisible()){
//     QTest::qWait(1000);
//   }
}

void TableModelTest::editableTest()
{
  using ArticleDataList = std::vector<ArticleData>;

  ArticleDataList articles;
  ArticleData article;
  article.dataStruct().id = 1;
  article.dataStruct().description = "One";
  article.dataStruct().qty = 1.0;
  articles.push_back(article);
  article.dataStruct().id = 2;
  article.dataStruct().description = "Two";
  article.dataStruct().qty = 3.1;
  articles.push_back(article);

  EditableTableModel<ArticleDataList> model;
  model.setTable(articles);

  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.columnCount(), 3);
  checkArticleModelHeaderData(model);
  QCOMPARE(getModelData(model, 0, 0), QVariant(1));
  QCOMPARE(getModelData(model, 0, 1), QVariant("One"));
  QCOMPARE(getModelData(model, 0, 2), QVariant(1.0));
  QCOMPARE(getModelData(model, 1, 0), QVariant(2));
  QCOMPARE(getModelData(model, 1, 1), QVariant("Two"));
  QCOMPARE(getModelData(model, 1, 2), QVariant(3.1));

  QVERIFY(setModelData(model, 0, 0, 11));
  QVERIFY(setModelData(model, 0, 1, "Article 11"));
  QVERIFY(setModelData(model, 0, 2, 5));
  QVERIFY(setModelData(model, 1, 0, 12));
  QVERIFY(setModelData(model, 1, 1, "Article 12"));
  QVERIFY(setModelData(model, 1, 2, 7.6));

  QCOMPARE(getModelData(model, 0, 0), QVariant(11));
  QCOMPARE(getModelData(model, 0, 1), QVariant("Article 11"));
  QCOMPARE(getModelData(model, 0, 2), QVariant(5.0));
  QCOMPARE(getModelData(model, 1, 0), QVariant(12));
  QCOMPARE(getModelData(model, 1, 1), QVariant("Article 12"));
  QCOMPARE(getModelData(model, 1, 2), QVariant(7.6));

//   QTableView tv;
//   tv.setModel(&model);
//   tv.show();
//   while(tv.isVisible()){
//     QTest::qWait(1000);
//   }
}

void TableModelTest::rowResizableTest()
{
  using ArticleDataList = std::vector<ArticleData>;

  RowResizableTableModel<ArticleDataList> model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 3);
  checkArticleModelHeaderData(model);

  RowResizableTableModel<ArticleDataList> modelI;
  ItemModelInsertRowTest insertTest(&modelI);

  RowResizableTableModel<ArticleDataList> modelR;
  ItemModelRemoveRowTest removeTest(&modelR);

//   QTableView tv;
//   tv.setModel(&model);
//   tv.show();
//   tv.resizeColumnsToContents();
//   QTest::qWait(2000);
//   appendRowToModel(model);
//   QTest::qWait(2000);
//   appendRowToModel(model);
//   while(tv.isVisible()){
//     QTest::qWait(1000);
//   }

}

/*
 * Main
 */

int main(int argc, char **argv)
{
  ///Mdt::CoreApplication app(argc, argv);
  QApplication app(argc, argv);
  TableModelTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
