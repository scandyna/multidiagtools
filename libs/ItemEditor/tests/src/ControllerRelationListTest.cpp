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
#include "ControllerRelationListTest.h"
#include "AbstractControllerRelationTestClass.h"
#include "ItemModelControllerTester.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemModel/RelationKey.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include "Mdt/ItemModel/ForeignKey.h"
#include "Mdt/ItemEditor/ControllerRelation.h"
#include "Mdt/ItemEditor/ControllerRelationList.h"
#include <QSignalSpy>
#include <QStringListModel>
#include <QTableView>
#include <QSpinBox>
#include <QComboBox>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>

#include <QDebug>

using namespace Mdt::ItemModel;
using namespace Mdt::ItemEditor;

/*
 * Init/Cleanup
 */

void ControllerRelationListTest::initTestCase()
{
}

void ControllerRelationListTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ControllerRelationListTest::addChildControllerTest()
{
  AbstractControllerRelationTestClass *relation;
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  parentModel.resize(0, 1);
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  /*
   * Setup first child model and controller
   */
  VariantTableModel childModel1;
  childModel1.resize(0, 1);
  ItemModelControllerTester childController1;
  childController1.setModel(&childModel1);
  /*
   * Setup second child model and controller
   */
  VariantTableModel childModel2;
  childModel2.resize(0, 1);
  ItemModelControllerTester childController2;
  childController2.setModel(&childModel2);
  /*
   * Setup relation list
   */
  ControllerRelationList<AbstractController, AbstractControllerRelationTestClass> relationList(&parentController);
  QCOMPARE(relationList.childControllerCount(), 0);
  QVERIFY(relationList.cbegin() == relationList.cend());
  // Add first child controller
  relation = relationList.addChildController<AbstractControllerRelationTestClass>(&childController1);
  QCOMPARE(relation->parentController(), &parentController);
  QCOMPARE(relation->childController(), &childController1);
  QCOMPARE(relationList.childControllerCount(), 1);
  QVERIFY(relationList.cbegin() != relationList.cend());
  // Add second child controller
  relation = relationList.addChildController<AbstractControllerRelationTestClass>(&childController2);
  QCOMPARE(relation->parentController(), &parentController);
  QCOMPARE(relation->childController(), &childController2);
  QCOMPARE(relationList.childControllerCount(), 2);
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ControllerRelationListTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
