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
  ControllerRelationList relationList(&parentController);
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

void ControllerRelationListTest::submitTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  parentModel.resize(1, 1);
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  childModel.resize(1, 1);
  ItemModelControllerTester childController;
  childController.setModel(&childModel);
  /*
   * Setup relation list
   */
  ControllerRelationList relationList(&parentController);
  relationList.addChildController<AbstractControllerRelationTestClass>(&childController);
  /*
   * Edit in child controller and submit
   */
  childController.startEditing();
  childController.clearDataSubmitToModelCount();
  QVERIFY(relationList.submitForEachChild());
  QCOMPARE(childController.dataSubmitToModelCount(), 1);

  /*
   * Edit then submit in child controller
   */


  QFAIL("Not complete");
}

void ControllerRelationListTest::revertTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  parentModel.resize(1, 1);
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  childModel.resize(1, 1);
  ItemModelControllerTester childController;
  childController.setModel(&childModel);
  /*
   * Setup relation list
   */
  ControllerRelationList relationList(&parentController);
  relationList.addChildController<AbstractControllerRelationTestClass>(&childController);
  /*
   * Edit in child controller and revert
   */
  childController.startEditing();
  childController.clearDataRevertFromModelCount();
  relationList.revertForEachChild();
  QCOMPARE(childController.dataRevertFromModelCount(), 1);
  /*
   * Edit then revert in parent controller
   */


  /*
   * Edit then revert in child controller
   */

  QFAIL("Not complete");
}

void ControllerRelationListTest::editSubmitRevertParentChildStateTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  parentModel.resize(1, 1);
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  childModel.resize(1, 1);
  ItemModelControllerTester childController;
  childController.setModel(&childModel);
  /*
   * Setup relation
   *
   * parentController
   *       |
   *       ---> childController
   */
  ControllerRelationList relationList(&parentController);
  relationList.addChildController<AbstractControllerRelationTestClass>(&childController);
  QCOMPARE(parentController.controllerState(), ControllerState::Visualizing);
  QCOMPARE(childController.controllerState(), ControllerState::Visualizing);
  /*
   * Begin editing from parent controller
   */
  // Begin editing
  parentController.startEditing();
  QCOMPARE(parentController.controllerState(), ControllerState::Editing);
  QCOMPARE(childController.controllerState(), ControllerState::ParentEditing);
  // Submit from parent controller
  QVERIFY(relationList.submitForEachChild());
  QVERIFY(parentController.submit());
  QCOMPARE(parentController.controllerState(), ControllerState::Visualizing);
  QCOMPARE(childController.controllerState(), ControllerState::Visualizing);
  // Begin editing
  parentController.startEditing();
  QCOMPARE(parentController.controllerState(), ControllerState::Editing);
  QCOMPARE(childController.controllerState(), ControllerState::ParentEditing);
  // Revert from parent controller
  relationList.revertForEachChild();
  parentController.revert();
  QCOMPARE(parentController.controllerState(), ControllerState::Visualizing);
  QCOMPARE(childController.controllerState(), ControllerState::Visualizing);
  /*
   * Begin editing from child controller
   */
  // Begin editing
  childController.startEditing();
  QCOMPARE(parentController.controllerState(), ControllerState::ChildEditing);
  QCOMPARE(childController.controllerState(), ControllerState::Editing);
  // Submit from parent controller
  QVERIFY(relationList.submitForEachChild());
  QVERIFY(parentController.submit());
  QCOMPARE(parentController.controllerState(), ControllerState::Visualizing);
  QCOMPARE(childController.controllerState(), ControllerState::Visualizing);
  // Begin editing
  childController.startEditing();
  QCOMPARE(parentController.controllerState(), ControllerState::ChildEditing);
  QCOMPARE(childController.controllerState(), ControllerState::Editing);
  // Submit from child controller
  QVERIFY(childController.submit());
  /** \todo
   * Here, all should return to Visualizing .
   * If we start editing from parent controller, then from child controller:
   *  - Start editing from parent controller:
   *     -> parentController : Editing
   *     -> childController  : ParentEditing
   *  - Start editing from childController:
   *     -> parentController : Editing ? ChildEditing ?
   *     -> childController  : Editing ?
   *  - Submit from child controller:
   *     -> parentController : Editing
   *     -> childController  : Visualizing
   */
  
  // Begin editing
  childController.startEditing();
  QCOMPARE(parentController.controllerState(), ControllerState::ChildEditing);
  QCOMPARE(childController.controllerState(), ControllerState::Editing);
  // Revert from parent controller
  relationList.revertForEachChild();
  parentController.revert();
  QCOMPARE(parentController.controllerState(), ControllerState::Visualizing);
  QCOMPARE(childController.controllerState(), ControllerState::Visualizing);

  QFAIL("Not complete");
}

void ControllerRelationListTest::editSubmitRevertTopMiddleChildStateTest()
{
  /*
   * Setup top parent model and controller
   */
  VariantTableModel topParentModel;
  topParentModel.resize(1, 1);
  ItemModelControllerTester topParentController;
  topParentController.setModel(&topParentModel);
  /*
   * Setup middle model and controller
   */
  VariantTableModel middleModel;
  middleModel.resize(1, 1);
  ItemModelControllerTester middleController;
  middleController.setModel(&middleModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  childModel.resize(1, 1);
  ItemModelControllerTester childController;
  childController.setModel(&childModel);
  /*
   * Setup relations
   *
   * topParentController
   *       |
   *       ---> middleController
   *                 |
   *                 ---> childController
   */
  ControllerRelationList topMiddleRelationList(&topParentController);
  topMiddleRelationList.addChildController<AbstractControllerRelationTestClass>(&middleController);
  ControllerRelationList middleChildRelationList(&middleController);
  middleChildRelationList.addChildController<AbstractControllerRelationTestClass>(&childController);
  QCOMPARE(topParentController.controllerState(), ControllerState::Visualizing);
  QCOMPARE(middleController.controllerState(), ControllerState::Visualizing);
  QCOMPARE(childController.controllerState(), ControllerState::Visualizing);
  /*
   * Begin editing from top parent controller
   */
  // Begin editing
  topParentController.startEditing();
  QCOMPARE(topParentController.controllerState(), ControllerState::Editing);
  QCOMPARE(middleController.controllerState(), ControllerState::ParentEditing);
  QCOMPARE(childController.controllerState(), ControllerState::ParentEditing);

  QFAIL("Not complete");
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
