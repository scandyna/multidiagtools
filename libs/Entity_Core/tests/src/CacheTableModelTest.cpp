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
#include "CacheTableModelTest.h"
#include "PersonCache.h"
#include "Mdt/Entity/AbstractReadOnlyCache.h"
#include "Mdt/Entity/AbstractReadOnlyCacheTableModel.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QSignalSpy>
#include <QVariantList>

/*
 * Test class
 */

class ReadOnlyCacheTableModel : public Mdt::Entity::AbstractReadOnlyCacheTableModel
{
};

/*
 * A table model that defines no header
 */

// class DefaultHeaderTableModel : public Mdt::Entity::AbstractReadOnlyCacheTableModel
// {
//  public:
// 
//   int columnCount(const QModelIndex& parent = QModelIndex()) const override
//   {
//     Q_UNUSED(parent);
// 
//     return 3;
//   }
// };

/*
 * A table model that custom header
 */

// class CustomHeaderTableModel : public Mdt::Entity::AbstractReadOnlyCacheTableModel
// {
//  public:
// 
//   int columnCount(const QModelIndex& parent = QModelIndex()) const override
//   {
//     Q_UNUSED(parent);
// 
//     return 3;
//   }
// 
//  private:
// 
//   QVariant horizontalHeaderDisplayRoleData(int column) const override
//   {
//     switch(column){
//       case 0:
//         return "A";
//       case 1:
//         return "B";
//       case 2:
//         return "C";
//     }
//     return QVariant();
//   }
// };

using namespace Mdt::Entity;
using namespace Mdt::TestLib;

/*
 * Tests
 */

// void CacheTableModelTest::defaultHeaderTest()
// {
//   DefaultHeaderTableModel model;
//   QCOMPARE(model.columnCount(), 3);
//   QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant(1));
//   QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant(2));
//   QCOMPARE(model.headerData(2, Qt::Horizontal), QVariant(3));
//   QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
//   QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
//   QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
//   QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DecorationRole), QVariant());
// }
// 
// void CacheTableModelTest::customHeaderTest()
// {
//   CustomHeaderTableModel model;
//   QCOMPARE(model.columnCount(), 3);
//   QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant("A"));
//   QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant("B"));
//   QCOMPARE(model.headerData(2, Qt::Horizontal), QVariant("C"));
//   QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
//   QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
//   QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
//   QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DecorationRole), QVariant());
// }

void CacheTableModelTest::readOnlyHorizontalHeaderTest()
{
  PersonCache cache;
  ReadOnlyCacheTableModel model;
  model.setCache(&cache);
  QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant("id"));
  QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant("name"));
  QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DecorationRole), QVariant());
}

void CacheTableModelTest::readOnlyNoCacheGetTest()
{
  ReadOnlyCacheTableModel model;
  QVERIFY(model.cache() == nullptr);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);
}

void CacheTableModelTest::readOnlyFlagsTest()
{
  PersonCache cache;
  ReadOnlyCacheTableModel model;

  QFAIL("Not complete");
}

void CacheTableModelTest::readOnlySetCacheTest()
{
  ReadOnlyCacheTableModel model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 0);

  PersonCache emptyCache;
  model.setCache(&emptyCache);
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);

  PersonCache cache;
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  QCOMPARE(cache.rowCount(), 3);
  model.setCache(&cache);
  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(getModelData(model, 0, 1), QVariant("A"));
  QCOMPARE(getModelData(model, 1, 1), QVariant("B"));
  QCOMPARE(getModelData(model, 2, 1), QVariant("C"));
}

void CacheTableModelTest::readOnlySetCacheSignalTest()
{
  ReadOnlyCacheTableModel model;
  QSignalSpy resetSpy(&model, &ReadOnlyCacheTableModel::modelReset);
  QVERIFY(resetSpy.isValid());

  QCOMPARE(resetSpy.count(), 0);
  PersonCache cache;
  model.setCache(&cache);
  QCOMPARE(resetSpy.count(), 1);

  resetSpy.clear();
  populatePersonStorage(cache, {"A","B","C"});
  QVERIFY(cache.fetchAll());
  QCOMPARE(resetSpy.count(), 1);
}


void CacheTableModelTest::readOnlyQtModelTest()
{

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  CacheTableModelTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
