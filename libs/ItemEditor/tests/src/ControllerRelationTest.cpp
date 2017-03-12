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
#include "ControllerRelationTest.h"
#include "AbstractControllerRelationTestClass.h"
#include "ItemModelControllerTester.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemModel/RelationFilterProxyModel.h"
#include "Mdt/ItemModel/RelationKey.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include "Mdt/ItemModel/ForeignKey.h"
#include "Mdt/ItemModel/FormatProxyModel.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include "Mdt/ItemEditor/WidgetMapperController.h"
// #include "Mdt/ItemEditor/ControllerRelation.h"
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

void ControllerRelationTest::initTestCase()
{
}

void ControllerRelationTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ControllerRelationTest::setControllersTest()
{
  /*
   * Initial state
   */
  AbstractControllerRelationTestClass relation;
  QVERIFY(relation.storedOldParentController() == nullptr);
  QVERIFY(relation.storedParentController() == nullptr);
  QVERIFY(relation.storedOldChildController() == nullptr);
  QVERIFY(relation.storedChildController() == nullptr);
  /*
   * Set parent controller
   */
  ItemModelControllerTester parentController;
  relation.setParentController(&parentController);
  QVERIFY(relation.storedOldParentController() == nullptr);
  QCOMPARE(relation.storedParentController(), &parentController);
  QVERIFY(relation.storedOldChildController() == nullptr);
  QVERIFY(relation.storedChildController() == nullptr);
  /*
   * Set child controller
   */
  ItemModelControllerTester childController;
  relation.setChildController(&childController);
  QVERIFY(relation.storedOldParentController() == nullptr);
  QCOMPARE(relation.storedParentController(), &parentController);
  QVERIFY(relation.storedOldChildController() == nullptr);
  QCOMPARE(relation.storedChildController(), &childController);
  /*
   * Change parent controller
   */
  ItemModelControllerTester parentController2;
  relation.setParentController(&parentController2);
  QCOMPARE(relation.storedOldParentController(), &parentController);
  QCOMPARE(relation.storedParentController(), &parentController2);
  QVERIFY(relation.storedOldChildController() == nullptr);
  QCOMPARE(relation.storedChildController(), &childController);
  /*
   * Change child controller
   */
  ItemModelControllerTester childController2;
  relation.setChildController(&childController2);
  QCOMPARE(relation.storedOldParentController(), &parentController);
  QCOMPARE(relation.storedParentController(), &parentController2);
  QCOMPARE(relation.storedOldChildController(), &childController);
  QCOMPARE(relation.storedChildController(), &childController2);
}

void ControllerRelationTest::setModelToControllersFirstTest()
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
   * Set parent controller
   */
  AbstractControllerRelationTestClass relation;
  relation.setParentController(&parentController);
  QCOMPARE(relation.parentController(), &parentController);
  QVERIFY(relation.childController() == nullptr);
  QCOMPARE(relation.parentControllerModel(), &parentModel);
  QVERIFY(relation.childControllerModel() == nullptr);
  // Check that parent model was signaled to subclass
  QCOMPARE(relation.parentControllerStoredModel(), &parentModel);
  QVERIFY(relation.childControllerStoredModel() == nullptr);
  // Check that relation did no mismatch with controllers
  QCOMPARE(parentController.sourceModel(), &parentModel);
  QCOMPARE(parentController.modelForView(), &parentModel);
  /*
   * Set child controller
   */
  relation.setChildController(&childController);
  QCOMPARE(relation.parentController(), &parentController);
  QCOMPARE(relation.childController(), &childController);
  QCOMPARE(relation.parentControllerModel(), &parentModel);
  QCOMPARE(relation.childControllerModel(), &childModel);
  // Check that child model was signaled to subclass
  QCOMPARE(relation.parentControllerStoredModel(), &parentModel);
  QCOMPARE(relation.childControllerStoredModel(), &childModel);
  // Check that relation did no mismatch with controllers
  QCOMPARE(parentController.sourceModel(), &parentModel);
  QCOMPARE(parentController.modelForView(), &parentModel);
  QCOMPARE(childController.sourceModel(), &childModel);
  QCOMPARE(childController.modelForView(), &childModel);
}

void ControllerRelationTest::setModelToControllersAfterTest()
{
  /*
   * Setup parent and child controllers
   */
  ItemModelControllerTester parentController;
  parentController.setObjectName("parentController");
  ItemModelControllerTester childController;
  childController.setObjectName("childController");
  /*
   * Set parent controller
   */
  AbstractControllerRelationTestClass relation;
  relation.setParentController(&parentController);
  QCOMPARE(relation.parentController(), &parentController);
  QVERIFY(relation.childController() == nullptr);
  QVERIFY(relation.parentControllerModel() == nullptr);
  QVERIFY(relation.childControllerModel() == nullptr);
  // Subclass has not yet recieved any model changed signal
  QVERIFY(relation.parentControllerStoredModel() == nullptr);
  QVERIFY(relation.childControllerStoredModel() == nullptr);
  // Check that relation did no mismatch with controllers
  QVERIFY(parentController.sourceModel() == nullptr);
  QVERIFY(parentController.modelForView() == nullptr);
  /*
   * Set child controller
   */
  relation.setChildController(&childController);
  QCOMPARE(relation.parentController(), &parentController);
  QCOMPARE(relation.childController(), &childController);
  QVERIFY(relation.parentControllerModel() == nullptr);
  QVERIFY(relation.childControllerModel() == nullptr);
  // Subclass has not yet recieved any model changed signal
  QVERIFY(relation.parentControllerStoredModel() == nullptr);
  QVERIFY(relation.childControllerStoredModel() == nullptr);
  // Check that relation did no mismatch with controllers
  QVERIFY(parentController.sourceModel() == nullptr);
  QVERIFY(parentController.modelForView() == nullptr);
  QVERIFY(childController.sourceModel() == nullptr);
  QVERIFY(childController.modelForView() == nullptr);
  /*
   * Setup parent model
   */
  VariantTableModel parentModel;
  parentModel.resize(0, 1);
  parentController.setModel(&parentModel);
  QCOMPARE(relation.parentControllerModel(), &parentModel);
  QVERIFY(relation.childControllerModel() == nullptr);
  // Check that parent model was signaled to subclass
  QCOMPARE(relation.parentControllerStoredModel(), &parentModel);
  QVERIFY(relation.childControllerStoredModel() == nullptr);
  // Check that relation did no mismatch with controllers
  QCOMPARE(parentController.sourceModel(), &parentModel);
  QCOMPARE(parentController.modelForView(), &parentModel);
  /*
   * Setup child model
   */
  VariantTableModel childModel;
  childModel.resize(0, 1);
  childController.setModel(&childModel);
  QCOMPARE(relation.parentControllerModel(), &parentModel);
  QCOMPARE(relation.childControllerModel(), &childModel);
  // Check that child model was signaled to subclass
  QCOMPARE(relation.parentControllerStoredModel(), &parentModel);
  QCOMPARE(relation.childControllerStoredModel(), &childModel);
  // Check that relation did no mismatch with controllers
  QCOMPARE(parentController.sourceModel(), &parentModel);
  QCOMPARE(parentController.modelForView(), &parentModel);
  QCOMPARE(childController.sourceModel(), &childModel);
  QCOMPARE(childController.modelForView(), &childModel);
}


// void ControllerRelationTest::setControllersTest()
// {
//   ItemModelControllerTester parentController;
//   ItemModelControllerTester childController;
//   /*
//    * Create relation
//    */
//   ControllerRelation relation(&parentController);
// //   ControllerRelation<AbstractController, ControllerRelation> relation(&parentController);
// //   ControllerRelation relation(&parentController);
// //   QVERIFY(relation.proxyModel() != nullptr);
// //   QVERIFY(relation.proxyModel()->parentModel() == parentController.modelForView());
// 
//   QFAIL("Not complete");
// }

// void ControllerRelationTest::relationSetModelToControllersFirstTest()
// {
//   /*
//    * Setup parent model and controller
//    */
//   VariantTableModel parentModel;
//   parentModel.resize(0, 1);
//   ItemModelControllerTester parentController;
//   parentController.setModel(&parentModel);
//   QCOMPARE(parentController.sourceModel(), &parentModel);
//   /*
//    * Setup child model and controller
//    */
//   VariantTableModel childModel;
//   childModel.resize(0, 1);
//   ItemModelControllerTester childController;
//   childController.setModel(&childModel);
//   QCOMPARE(childController.sourceModel(), &childModel);
//   QCOMPARE(childController.modelForView(), &childModel);
//   /*
//    * Setup relation
//    */
//   ControllerRelation relation(&parentController);
//   QCOMPARE(parentController.sourceModel(), &parentModel);
//   QCOMPARE(childController.sourceModel(), &childModel);
//   QCOMPARE(childController.modelForView(), &childModel);
//   relation.setChildController(&childController);
//   relation.setRelationFilter( ChildModelColumn(0) == ParentModelColumn(0) );
//   QCOMPARE(parentController.sourceModel(), &parentModel);
//   QCOMPARE(relation.relationFilterModel()->parentModel(), &parentModel);
//   QCOMPARE(relation.relationFilterModel()->sourceModel(), &childModel);
//   QCOMPARE(childController.sourceModel(), &childModel);
//   QCOMPARE(childController.modelForView(), relation.relationFilterModel());
// }

// void ControllerRelationTest::relationSetModelToControllersAfterTest()
// {
//   /*
//    * Setup parent and child controllers
//    */
//   ItemModelControllerTester parentController;
//   parentController.setObjectName("parentController");
//   ItemModelControllerTester childController;
//   childController.setObjectName("childController");
//   /*
//    * Setup relation
//    */
//   ControllerRelation relation(&parentController);
//   relation.setChildController(&childController);
//   relation.setRelationFilter( ChildModelColumn(0) == ParentModelColumn(0) );
//   QVERIFY(parentController.sourceModel() == nullptr);
//   QVERIFY(childController.sourceModel() == nullptr);
//   QVERIFY(relation.relationFilterModel()->parentModel() == nullptr);
//   QVERIFY(relation.relationFilterModel()->sourceModel() == nullptr);
//   /*
//    * Setup parent model
//    */
//   VariantTableModel parentModel;
//   parentModel.resize(0, 1);
//   parentController.setModel(&parentModel);
//   QCOMPARE(parentController.sourceModel(), &parentModel);
//   QVERIFY(childController.sourceModel() == nullptr);
//   QCOMPARE(relation.relationFilterModel()->parentModel(), &parentModel);
//   QVERIFY(relation.relationFilterModel()->sourceModel() == nullptr);
//   /*
//    * Setup child model
//    */
//   VariantTableModel childModel;
//   childModel.resize(0, 1);
//   childController.setModel(&childModel);
//   QCOMPARE(parentController.sourceModel(), &parentModel);
//   QCOMPARE(relation.relationFilterModel()->parentModel(), &parentModel);
//   QCOMPARE(relation.relationFilterModel()->sourceModel(), &childModel);
//   QCOMPARE(childController.sourceModel(), &childModel);
//   QCOMPARE(childController.modelForView(), relation.relationFilterModel());
// }

// void ControllerRelationTest::relationFilterConditionTest()
// {
//   RelationKey rk;
//   /*
//    * Setup parent model and controller
//    */
//   VariantTableModel parentModel;
//   parentModel.resize(0, 3);
//   ItemModelControllerTester parentController;
//   parentController.setModel(&parentModel);
//   QCOMPARE(parentController.sourceModel(), &parentModel);
//   /*
//    * Setup child model and controller
//    */
//   VariantTableModel childModel;
//   childModel.resize(0, 3);
//   ItemModelControllerTester childController;
//   childController.setModel(&childModel);
//   QCOMPARE(childController.sourceModel(), &childModel);
//   QCOMPARE(childController.modelForView(), &childModel);
//   /*
//    * Setup relation using expression
//    */
//   ControllerRelation relation1(&parentController);
//   relation1.setChildController(&childController);
//   relation1.setRelationFilter( ChildModelColumn(2) == ParentModelColumn(1) );
//   rk = relation1.relationFilterModel()->relationKeyForEquality();
//   QCOMPARE(rk.columnPairCount(), 1);
//   QCOMPARE(rk.columnPairAt(0).parentModelColumn(), 1);
//   QCOMPARE(rk.columnPairAt(0).childModelColumn(), 2);
//   /*
//    * Setup relation using primary and foreign keys
//    */
//   parentController.setPrimaryKey({2});
//   childController.setForeignKey({3});
//   ControllerRelation relation2(&parentController);
//   relation2.setChildController(&childController);
//   relation2.setRelationFilterFromPkFk();
//   rk = relation2.relationFilterModel()->relationKeyForEquality();
//   QCOMPARE(rk.columnPairCount(), 1);
//   QCOMPARE(rk.columnPairAt(0).parentModelColumn(), 2);
//   QCOMPARE(rk.columnPairAt(0).childModelColumn(), 3);
// }

// void ControllerRelationTest::relationParentControllerCurrentRowTest()
// {
//   /*
//    * Setup parent model and controller
//    */
//   VariantTableModel parentModel;
//   parentModel.setObjectName("parentModel");
//   parentModel.resize(2, 1);
//   ItemModelControllerTester parentController;
//   parentController.setModel(&parentModel);
//   /*
//    * Setup child model and controller
//    */
//   VariantTableModel childModel;
//   childModel.setObjectName("childModel");
//   childModel.resize(2, 1);
//   ItemModelControllerTester childController;
//   childController.setModel(&childModel);
//   /*
//    * Initial state
//    */
//   QCOMPARE(parentController.currentRow(), 0);
//   ControllerRelation relation(&parentController);
//   QCOMPARE(relation.relationFilterModel()->parentModelMatchRow(), 0);
// //   QCOMPARE(relation.relationKeyCopier()->parentModelCurrentRow(), 0);
//   /*
//    * Change parent controller current row
//    */
//   QVERIFY(parentController.setCurrentRow(1));
//   QCOMPARE(parentController.currentRow(), 1);
//   QCOMPARE(relation.relationFilterModel()->parentModelMatchRow(), 1);
// //   QCOMPARE(relation.relationKeyCopier()->parentModelCurrentRow(), 1);
//   /*
//    * Change parent controllers model
//    */
//   VariantTableModel parentModel2;
//   parentModel2.resize(4, 1);
//   parentController.setModel(&parentModel2);
//   QCOMPARE(parentController.currentRow(), 0);
//   QCOMPARE(relation.relationFilterModel()->parentModelMatchRow(), 0);
// //   QCOMPARE(relation.relationKeyCopier()->parentModelCurrentRow(), 0);
//   /*
//    * Change parent controller current row
//    */
//   QVERIFY(parentController.setCurrentRow(1));
//   QCOMPARE(parentController.currentRow(), 1);
//   QCOMPARE(relation.relationFilterModel()->parentModelMatchRow(), 1);
// //   QCOMPARE(relation.relationKeyCopier()->parentModelCurrentRow(), 1);
// }

// void ControllerRelationTest::relationFilterTest()
// {
//   /*
//    * Setup parent model
//    * -------------
//    * | Id | Name |
//    * -------------
//    * | 1  | C1   |
//    * -------------
//    * | 2  | C2   |
//    * -------------
//    */
//   VariantTableModel clientModel;
//   clientModel.setObjectName("clientModel");
//   clientModel.resize(2, 2);
//   clientModel.populateColumn(0, {1,2});
//   clientModel.populateColumn(1, {"C1","C2"});
//   /*
//    * Setup child model
//    * ------------------------
//    * | Id | Cli_Id | Street |
//    * ------------------------
//    * | 11 |   1    |  S11   |
//    * ------------------------
//    * | 12 |   1    |  S12   |
//    * ------------------------
//    * | 21 |   2    |  S21   |
//    * ------------------------
//    */
//   VariantTableModel addressModel;
//   addressModel.setObjectName("addressModel");
//   addressModel.resize(3, 3);
//   addressModel.populateColumn(0, {11,12,21});
//   addressModel.populateColumn(1, {1 ,1 ,2 });
//   addressModel.populateColumn(2, {"S11","S12","S21"});
//   /*
//    * Setup controllers and views
//    */
//   ItemModelControllerTester clientController;
//   clientController.setModel(&clientModel);
//   ItemModelControllerTester addressController;
//   addressController.setModel(&addressModel);
//   /*
//    * Setup relation
//    */
//   ParentModelColumn clientId(0);
//   FilterColumn addressClientId(1);
//   ControllerRelation relation(&clientController);
//   relation.setChildController(&addressController);
//   relation.setRelationFilter(addressClientId == clientId);
//   /*
//    * Check that filter was applied for current client
//    * ------------------------
//    * | Id | Cli_Id | Street |
//    * ------------------------
//    * | 11 |   1    |  S11   |
//    * ------------------------
//    * | 12 |   1    |  S12   |
//    * ------------------------
//    */
//   QCOMPARE(clientController.currentRow(), 0);
//   QCOMPARE(addressController.modelForView()->rowCount(), 2);
//   QCOMPARE(getModelData(addressController.modelForView(), 0, 0), QVariant(11));
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 0), QVariant(12));
//   /*
//    * Change current client and check address model
//    * ------------------------
//    * | Id | Cli_Id | Street |
//    * ------------------------
//    * | 21 |   2    |  S21   |
//    * ------------------------
//    */
//   QVERIFY(clientController.setCurrentRow(1));
//   QCOMPARE(addressController.modelForView()->rowCount(), 1);
//   QCOMPARE(getModelData(addressController.modelForView(), 0, 0), QVariant(21));
//   QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
//   /*
//    * Insert a row with address controller, at end
//    * Address model filtered on client 2, after insert done:
//    * ------------------------
//    * | Id | Cli_Id | Street |
//    * ------------------------
//    * | 21 |   2    |  S21   |
//    * ------------------------
//    * |    |   2    |        |
//    * ------------------------
//    */
//   addressController.setInsertLocation(TableViewController::InsertAtEnd);
//   QVERIFY(addressController.insert());
//   QCOMPARE(addressController.modelForView()->rowCount(), 2);
//   QCOMPARE(getModelData(addressController.modelForView(), 0, 0), QVariant(21));
//   QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 1), QVariant(2));
//   /*
//    * Edit freshly inserted row
//    * Address model filtered on client 2, after edition done:
//    * ------------------------
//    * | Id | Cli_Id | Street |
//    * ------------------------
//    * | 21 |   2    |  S21   |
//    * ------------------------
//    * | 22 |   2    |  S22   |
//    * ------------------------
//    */
//   setModelData(addressController.modelForView(), 1, 0, 22);
//   setModelData(addressController.modelForView(), 1, 2, "S22");
//   QCOMPARE(getModelData(addressController.modelForView(), 0, 0), QVariant(21));
//   QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
//   QCOMPARE(getModelData(addressController.modelForView(), 0, 2), QVariant("S21"));
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 0), QVariant(22));
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 1), QVariant(2));
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 2), QVariant("S22"));
//   /*
//    * Insert a row into address model, at beginning
//    * Address model filtered on client 2, after insert done:
//    * ------------------------
//    * | Id | Cli_Id | Street |
//    * ------------------------
//    * |    |   2    |        |
//    * ------------------------
//    * | 21 |   2    |  S21   |
//    * ------------------------
//    * | 22 |   2    |  S22   |
//    * ------------------------
//    */
//   addressController.setInsertLocation(TableViewController::InsertAtBeginning);
//   QVERIFY(addressController.insert());
//   QCOMPARE(addressController.modelForView()->rowCount(), 3);
//   QVERIFY(getModelData(addressController.modelForView(), 0, 0).isNull());
//   QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
//   QVERIFY(getModelData(addressController.modelForView(), 0, 2).isNull());
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 0), QVariant(21));
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 1), QVariant(2));
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 2), QVariant("S21"));
//   QCOMPARE(getModelData(addressController.modelForView(), 2, 0), QVariant(22));
//   QCOMPARE(getModelData(addressController.modelForView(), 2, 1), QVariant(2));
//   QCOMPARE(getModelData(addressController.modelForView(), 2, 2), QVariant("S22"));
//   /*
//    * Enable formatting for address controller
//    * (Will append a proxy model)
//    * Address model filtered on client 2, after proxy model insertion done:
//    * ------------------------
//    * | Id | Cli_Id | Street |
//    * ------------------------
//    * |    |   2    |        |
//    * ------------------------
//    * | 21 |   2    |  S21   |
//    * ------------------------
//    * | 22 |   2    |  S22   |
//    * ------------------------
//    */
//   FormatProxyModel addressFormatModel;
//   addressController.appendProxyModel(&addressFormatModel);
//   addressFormatModel.setTextAlignmentForColumn(0, Qt::AlignCenter);
//   QCOMPARE(addressController.modelForView()->rowCount(), 3);
//   QVERIFY(getModelData(addressController.modelForView(), 0, 0).isNull());
//   QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
//   QVERIFY(getModelData(addressController.modelForView(), 0, 2).isNull());
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 0), QVariant(21));
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 1), QVariant(2));
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 2), QVariant("S21"));
//   QCOMPARE(getModelData(addressController.modelForView(), 2, 0), QVariant(22));
//   QCOMPARE(getModelData(addressController.modelForView(), 2, 1), QVariant(2));
//   QCOMPARE(getModelData(addressController.modelForView(), 2, 2), QVariant("S22"));
//   /*
//    * Enable formatting for client controller
//    * (Will append a proxy model)
//    * Address model filtered on client 2, after proxy model insertion done:
//    * ------------------------
//    * | Id | Cli_Id | Street |
//    * ------------------------
//    * |    |   2    |        |
//    * ------------------------
//    * | 21 |   2    |  S21   |
//    * ------------------------
//    * | 22 |   2    |  S22   |
//    * ------------------------
//    */
//   FormatProxyModel clientFormatModel;
//   clientController.appendProxyModel(&clientFormatModel);
//   clientFormatModel.setTextAlignmentForColumn(0, Qt::AlignCenter);
//   // Client controller will go to first row after model change, go back to client 2
//   clientController.setCurrentRow(1);
//   QCOMPARE(addressController.modelForView()->rowCount(), 3);
//   QVERIFY(getModelData(addressController.modelForView(), 0, 0).isNull());
//   QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
//   QVERIFY(getModelData(addressController.modelForView(), 0, 2).isNull());
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 0), QVariant(21));
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 1), QVariant(2));
//   QCOMPARE(getModelData(addressController.modelForView(), 1, 2), QVariant("S21"));
//   QCOMPARE(getModelData(addressController.modelForView(), 2, 0), QVariant(22));
//   QCOMPARE(getModelData(addressController.modelForView(), 2, 1), QVariant(2));
//   QCOMPARE(getModelData(addressController.modelForView(), 2, 2), QVariant("S22"));
// }

// void ControllerRelationTest::relationFilterTableViewTest()
// {
//   QModelIndex index;
//   /*
//    * Setup parent model
//    * -------------
//    * | Id | Name |
//    * -------------
//    * | 1  | C1   |
//    * -------------
//    * | 2  | C2   |
//    * -------------
//    */
//   VariantTableModel clientModel;
//   clientModel.resize(2, 2);
//   clientModel.populateColumn(0, {1,2});
//   clientModel.populateColumn(1, {"C1","C2"});
//   /*
//    * Setup child model
//    * ------------------------
//    * | Id | Cli_Id | Street |
//    * ------------------------
//    * | 11 |   1    |  S11   |
//    * ------------------------
//    * | 12 |   1    |  S12   |
//    * ------------------------
//    * | 21 |   2    |  S21   |
//    * ------------------------
//    */
//   VariantTableModel addressModel;
//   addressModel.resize(3, 3);
//   addressModel.populateColumn(0, {11,12,21});
//   addressModel.populateColumn(1, {1 ,1 ,2 });
//   addressModel.populateColumn(2, {"S11","S12","S21"});
//   /*
//    * Setup controllers and views
//    */
//   QTableView clientView;
//   TableViewController clientController;
//   clientController.setModel(&clientModel);
//   clientController.setView(&clientView);
//   QTableView addressView;
//   TableViewController addressController;
//   addressController.setInsertLocation(TableViewController::InsertAtEnd);
//   addressController.setModel(&addressModel);
//   addressController.setView(&addressView);
//   /*
//    * Setup relation
//    */
//   ParentModelColumn clientId(0);
//   FilterColumn addressClientId(1);
//   ControllerRelation relation(&clientController);
//   relation.setChildController(&addressController);
//   relation.setRelationFilter(addressClientId == clientId);
//   /*
//    * Check that filter was applied for current client
//    */
//   QCOMPARE(clientController.currentRow(), 0);
//   QCOMPARE(addressView.model()->rowCount(), 2);
//   index = addressView.model()->index(0, 0);
//   QCOMPARE(addressView.model()->data(index), QVariant(11));
//   /*
//    * Change current client
//    */
//   QVERIFY(clientController.setCurrentRow(1));
//   QCOMPARE(addressView.model()->rowCount(), 1);
//   index = addressView.model()->index(0, 0);
//   QCOMPARE(addressView.model()->data(index), QVariant(21));
//   /*
//    * Insert a row in address model
//    * Address model filtered on client 2, after inert done:
//    * ------------------------
//    * | Id | Cli_Id | Street |
//    * ------------------------
//    * | 21 |   2    |  S21   |
//    * ------------------------
//    * | 22 |   2    |  S22   |
//    * ------------------------
//    */
//   QVERIFY(addressController.insertLocation() == TableViewController::InsertAtEnd);
//   QVERIFY(addressController.insert());
//   // New inserted row must not be filtered until it is saved
//   QCOMPARE(addressView.model()->rowCount(), 2);
//   QVERIFY(setModelData(addressView.model(), 1, 0, 22));
//   QVERIFY(setModelData(addressView.model(), 1, 1, 2));
//   QVERIFY(setModelData(addressView.model(), 1, 2, "S22"));
//   /*
//    * Save by calling controller's method
//    */
//   QVERIFY(addressController.submit());
//   QCOMPARE(addressView.model()->rowCount(), 2);
//   QCOMPARE(getModelData(addressView.model(), 0, 0), QVariant(21));
//   QCOMPARE(getModelData(addressView.model(), 0, 1), QVariant(2));
//   QCOMPARE(getModelData(addressView.model(), 1, 0), QVariant(22));
//   QCOMPARE(getModelData(addressView.model(), 1, 1), QVariant(2));
//   /*
//    * Check that relation filter works again
//    */
//   
//   /**
//    * \todo Check also submiting in view directly
//    */
//   
//   /*
//    * Play
//    */
// //   clientModel.setHeaderData(0, Qt::Horizontal, "Id");
// //   clientModel.setHeaderData(1, Qt::Horizontal, "Name");
// //   clientView.setWindowTitle("Client");
// //   clientView.resizeColumnsToContents();
// //   clientView.show();
// //   addressView.setWindowTitle("Address");
// //   addressView.resizeColumnsToContents();
// //   addressView.show();
// //   while(clientView.isVisible()){
// //     QTest::qWait(500);
// //   }
//   
//   QFAIL("Not complete");
// }

void ControllerRelationTest::relationListSetModelToControllersFirstTest()
{
  ControllerRelation *relation;
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
  ControllerRelationList<AbstractController, ControllerRelation> relationList(&parentController);
  QCOMPARE(relationList.childControllerCount(), 0);
  // Add first child controller
  relationList.addChildController(&childController1, FilterColumn(0) == ParentModelColumn(0));
  QCOMPARE(relationList.childControllerCount(), 1);
  relation = *(relationList.cbegin());
  QCOMPARE(childController1.modelForView(), relation->relationFilterModel());
  // Add second child controller
  relationList.addChildController(&childController2, FilterColumn(0) == ParentModelColumn(0));
  QCOMPARE(relationList.childControllerCount(), 2);
  relation = *(relationList.cbegin() + 1);
  QCOMPARE(childController2.modelForView(), relation->relationFilterModel());
}

void ControllerRelationTest::relationListSetModelToControllersAfterTest()
{

  QFAIL("Not complete");
}

void ControllerRelationTest::relationListFilterConditionTest()
{
  ControllerRelation *relation;
  RelationKey rk;
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  parentModel.resize(0, 3);
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  /*
   * Setup relation list
   */
  ControllerRelationList<AbstractController, ControllerRelation> relationList(&parentController);
  QCOMPARE(relationList.childControllerCount(), 0);
  /*
   * Add a child controller by specifying filter
   */
  VariantTableModel childModel1;
  childModel1.resize(0, 3);
  ItemModelControllerTester childController1;
  childController1.setModel(&childModel1);
  relationList.addChildController(&childController1, ChildModelColumn(2) == ParentModelColumn(1));
  QCOMPARE(relationList.childControllerCount(), 1);
  relation = *(relationList.cbegin());
  rk = relation->relationFilterModel()->relationKeyForEquality();
  QCOMPARE(rk.columnPairCount(), 1);
  QCOMPARE(rk.columnPairAt(0).parentModelColumn(), 1);
  QCOMPARE(rk.columnPairAt(0).childModelColumn(), 2);
  /*
   * Add a child controller using PK/FK
   */
  VariantTableModel childModel2;
  childModel2.resize(0, 3);
  ItemModelControllerTester childController2;
  childController2.setModel(&childModel2);
  parentController.setPrimaryKey({2});
  childController2.setForeignKey({3});
  relationList.addChildController(&childController2);
  QCOMPARE(relationList.childControllerCount(), 2);
  relation = *(relationList.cbegin() + 1);
  rk = relation->relationFilterModel()->relationKeyForEquality();
  QCOMPARE(rk.columnPairCount(), 1);
  QCOMPARE(rk.columnPairAt(0).parentModelColumn(), 2);
  QCOMPARE(rk.columnPairAt(0).childModelColumn(), 3);
}

void ControllerRelationTest::relationListBasicSetGetTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  ItemModelControllerTester childController;
  childController.setModel(&childModel);
  /*
   * Setup relation list
   */
  ControllerRelationList<AbstractController, ControllerRelation> relationList(&parentController);
  QCOMPARE(relationList.childControllerCount(), 0);
  QVERIFY(relationList.cbegin() == relationList.cend());
  /*
   * Add child controller
   */
  relationList.addChildController(&childController, FilterColumn(0) == ParentModelColumn(0));
  QCOMPARE(relationList.childControllerCount(), 1);
  QVERIFY(relationList.cbegin() != relationList.cend());

  QFAIL("Not complete");
}

void ControllerRelationTest::controllerAddChildControllerTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  ItemModelControllerTester childController;
  childController.setModel(&childModel);
  /*
   * Add child controller
   */
  ParentModelColumn pmc(0);
  FilterColumn fc(0);
  parentController.addChildController(&childController, fc == pmc);
//   QVERIFY(childController.isRelationFilterEnabled());
//   QCOMPARE(childController.relationFilterModel()->parentModel(), &parentModel);
//   QCOMPARE(childController.relationFilterModel()->sourceModel(), &childModel);

  QFAIL("Not complete (missing check of correct model mappings)");
}

void ControllerRelationTest::controllerChangeModelForViewTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  TableViewController parentController;
  parentController.setModel(&parentModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  TableViewController childController;
  childController.setModel(&childModel);
  /*
   * Add child controller
   */
  ParentModelColumn pmc(0);
  FilterColumn fc(0);
  parentController.addChildController(&childController, fc == pmc);
//   QVERIFY(childController.isRelationFilterEnabled());
//   QCOMPARE(childController.relationFilterModel()->parentModel(), &parentModel);
//   QCOMPARE(childController.relationFilterModel()->sourceModel(), &childModel);
  /*
   * Enable filter on parent controller
   */
  QCOMPARE(parentController.modelForView(), &parentModel);
  parentController.setFilter(fc > 5);
  QVERIFY(parentController.modelForView() != &parentModel);
  QVERIFY(parentController.isFilterEnabled());
//   QCOMPARE(childController.relationFilterModel()->parentModel(), parentController.modelForView());
//   QCOMPARE(childController.relationFilterModel()->sourceModel(), &childModel);
  
  QFAIL("Not complete (missing check of correct model mappings)");
}

void ControllerRelationTest::controllerRelationFilterTest()
{

  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ControllerRelationTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
