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
#include "NavigationControllerRelationTest.h"
#include "ItemModelControllerTester.h"
#include "Mdt/ItemEditor/NavigationControllerRelation.h"
#include "Mdt/ItemModel/VariantTableModel.h"

using namespace Mdt::ItemEditor;
using namespace Mdt::ItemModel;

/*
 * Init/Cleanup
 */

void NavigationControllerRelationTest::initTestCase()
{
}

void NavigationControllerRelationTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void NavigationControllerRelationTest::setControllersTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  parentModel.resize(0, 1);
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  QCOMPARE(parentController.sourceModel(), &parentModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  childModel.resize(0, 1);
  ItemModelControllerTester childController;
  childController.setModel(&childModel);
  QCOMPARE(childController.sourceModel(), &childModel);
  QCOMPARE(childController.modelForView(), &childModel);
  /*
   * Setup relation
   */
  NavigationControllerRelation relation;
  relation.setParentController(&parentController);
  relation.setChildController(&childController);
//   QCOMPARE(relation.relationFilterModel()->parentModel(), &parentModel);
//   QCOMPARE(relation.relationFilterModel()->sourceModel(), &childModel);

  QFAIL("Not complete");
}

void NavigationControllerRelationTest::setCurrentRowTest()
{
  /*
   * Setup model
   */
  VariantTableModel model;
  model.resize(3, 1);
  /*
   * Setup parent controller
   */
  ItemModelControllerTester parentController;
  parentController.setModel(&model);
  QCOMPARE(parentController.sourceModel(), &model);
  /*
   * Setup child controller
   */
  ItemModelControllerTester childController;
  childController.setModel(&model);
  QCOMPARE(childController.sourceModel(), &model);
  QCOMPARE(childController.modelForView(), &model);
  /*
   * Setup relation
   */
  NavigationControllerRelation relation;
  relation.setParentController(&parentController);
  relation.setChildController(&childController);
  QCOMPARE(parentController.currentRow(), 0);
  QCOMPARE(childController.currentRow(), 0);
  /*
   * Change current row
   */
  QVERIFY(parentController.setCurrentRow(1));
  QCOMPARE(parentController.currentRow(), 1);
  QCOMPARE(childController.currentRow(), 1);
  /*
   * Edit in child
   */
  childController.startEditing();
  QVERIFY(!parentController.setCurrentRow(0));
  QCOMPARE(parentController.currentRow(), 1);
  QCOMPARE(childController.currentRow(), 1);
  childController.stopEditing();
  QVERIFY(parentController.setCurrentRow(0));
  QCOMPARE(parentController.currentRow(), 0);
  QCOMPARE(childController.currentRow(), 0);

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  NavigationControllerRelationTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
