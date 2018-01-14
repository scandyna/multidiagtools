/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "ItemModelTest.h"
#include "Mdt/TestLib/ItemModel.h"
#include <QStringList>
#include <QStringListModel>

using namespace Mdt::TestLib;

void ItemModelTest::initTestCase()
{
}

void ItemModelTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ItemModelTest::getModelDataTest()
{
  QStringListModel model(QStringList{"A","B","C"});

  QCOMPARE( getModelData(&model, 0, 0), QVariant("A") );
  QCOMPARE( getModelData(&model, 1, 0), QVariant("B") );
  QCOMPARE( getModelData(&model, 2, 0), QVariant("C") );

  QCOMPARE( getModelData(model, 0, 0), QVariant("A") );
  QCOMPARE( getModelData(model, 1, 0), QVariant("B") );
  QCOMPARE( getModelData(model, 2, 0), QVariant("C") );
}

void ItemModelTest::setModelDataTest()
{
  QStringListModel model(QStringList{"A","B","C"});

  QCOMPARE( getModelData(model, 0, 0), QVariant("A") );
  QCOMPARE( getModelData(model, 1, 0), QVariant("B") );
  QCOMPARE( getModelData(model, 2, 0), QVariant("C") );
  QVERIFY( setModelData(&model, 0, 0, 1) );
  QVERIFY( setModelData( model, 1, 0, 2) );
  QVERIFY( setModelData( model, 2, 0, 3) );
  QCOMPARE( getModelData(model, 0, 0), QVariant(1) );
  QCOMPARE( getModelData(model, 1, 0), QVariant(2) );
  QCOMPARE( getModelData(model, 2, 0), QVariant(3) );
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ItemModelTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
