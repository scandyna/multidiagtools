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
#include "FilterControllerRelationTest.h"
#include "ItemModelControllerTester.h"
#include "Mdt/ItemEditor/FilterControllerRelation.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemModel/RelationFilterProxyModel.h"
#include "Mdt/ItemModel/RelationKey.h"
#include "Mdt/ItemModel/FormatProxyModel.h"
#include <QTableView>

using namespace Mdt::ItemEditor;
using namespace Mdt::ItemModel;

/*
 * Init/Cleanup
 */

void FilterControllerRelationTest::initTestCase()
{
}

void FilterControllerRelationTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void FilterControllerRelationTest::setControllersTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  parentModel.resize(0, 1);
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  QCOMPARE(parentController.sourceModel(), &parentModel);
  auto *defaultParentModelForView = parentController.modelForView();
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  childModel.resize(0, 1);
  ItemModelControllerTester childController;
  childController.setModel(&childModel);
  QCOMPARE(childController.sourceModel(), &childModel);
  /*
   * Setup relation
   */
  FilterControllerRelation relation;
  relation.setParentController(&parentController);
  relation.setChildController(&childController);
  QCOMPARE(relation.relationFilterModel()->parentModel(), defaultParentModelForView);
  QCOMPARE(relation.relationFilterModel()->sourceModel(), &childModel);
}

void FilterControllerRelationTest::filterConditionTest()
{
  RelationKey rk;
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  parentModel.resize(0, 3);
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  QCOMPARE(parentController.sourceModel(), &parentModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  childModel.resize(0, 3);
  ItemModelControllerTester childController;
  childController.setModel(&childModel);
  QCOMPARE(childController.sourceModel(), &childModel);
  /*
   * Setup relation using expression
   */
  FilterControllerRelation relation1;
  relation1.setParentController(&parentController);
  relation1.setChildController(&childController);
  relation1.setRelationFilter( ChildModelColumn(2) == ParentModelColumn(1) );
  rk = relation1.relationFilterModel()->relationKeyForEquality();
  QCOMPARE(rk.columnPairCount(), 1);
  QCOMPARE(rk.columnPairAt(0).parentModelColumn(), 1);
  QCOMPARE(rk.columnPairAt(0).childModelColumn(), 2);
  /*
   * Setup relation using primary and foreign keys
   */
  parentController.setPrimaryKey({2});
  parentController.setEntityName("Parent");
//   childController.setForeignKey({3});
  childController.addForeignKey("Parent", {3});
  FilterControllerRelation relation2;
  relation2.setParentController(&parentController);
  relation2.setChildController(&childController);
  relation2.setRelationFilterFromPkFk();
  rk = relation2.relationFilterModel()->relationKeyForEquality();
  QCOMPARE(rk.columnPairCount(), 1);
  QCOMPARE(rk.columnPairAt(0).parentModelColumn(), 2);
  QCOMPARE(rk.columnPairAt(0).childModelColumn(), 3);
}

void FilterControllerRelationTest::parentControllerCurrentRowTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  parentModel.setObjectName("parentModel");
  parentModel.resize(2, 1);
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  childModel.setObjectName("childModel");
  childModel.resize(2, 1);
  ItemModelControllerTester childController;
  childController.setModel(&childModel);
  /*
   * Initial state
   */
  QCOMPARE(parentController.currentRow(), 0);
  FilterControllerRelation relation;
  relation.setParentController(&parentController);
  QCOMPARE(relation.relationFilterModel()->parentModelMatchRow(), 0);
//   QCOMPARE(relation.relationKeyCopier()->parentModelCurrentRow(), 0);
  /*
   * Change parent controller current row
   */
  QVERIFY(parentController.setCurrentRow(1));
  QCOMPARE(parentController.currentRow(), 1);
  QCOMPARE(relation.relationFilterModel()->parentModelMatchRow(), 1);
//   QCOMPARE(relation.relationKeyCopier()->parentModelCurrentRow(), 1);
  /*
   * Change parent controllers model
   */
  VariantTableModel parentModel2;
  parentModel2.resize(4, 1);
  parentController.setModel(&parentModel2);
  QCOMPARE(parentController.currentRow(), 0);
  QCOMPARE(relation.relationFilterModel()->parentModelMatchRow(), 0);
//   QCOMPARE(relation.relationKeyCopier()->parentModelCurrentRow(), 0);
  /*
   * Change parent controller current row
   */
  QVERIFY(parentController.setCurrentRow(1));
  QCOMPARE(parentController.currentRow(), 1);
  QCOMPARE(relation.relationFilterModel()->parentModelMatchRow(), 1);
//   QCOMPARE(relation.relationKeyCopier()->parentModelCurrentRow(), 1);
}

void FilterControllerRelationTest::insertPermissionTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  parentModel.setObjectName("parentModel");
  ItemModelControllerTester parentController;
  parentController.setModel(&parentModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  childModel.setObjectName("childModel");
  ItemModelControllerTester childController;
  childController.setModel(&childModel);
  /*
   * Setup relation
   */
  FilterControllerRelation relation;
  relation.setParentController(&parentController);
  relation.setChildController(&childController);
  /*
   * Because parent controller is at a invalid row,
   * child controller must not be able to insert
   */
  QCOMPARE(parentController.currentRow(), -1);
  QVERIFY(parentController.canInsert());
  QVERIFY(!childController.canInsert());
  /*
   * Place parent controller to a valid row
   */
  parentModel.resize(2, 1);
  QCOMPARE(parentController.currentRow(), 0);
  QVERIFY(parentController.canInsert());
  QVERIFY(childController.canInsert());
}

void FilterControllerRelationTest::filterTest()
{
  /*
   * Setup parent model
   * -------------
   * | Id | Name |
   * -------------
   * | 1  | C1   |
   * -------------
   * | 2  | C2   |
   * -------------
   */
  VariantTableModel clientModel;
  clientModel.setObjectName("clientModel");
  clientModel.resize(2, 2);
  clientModel.populateColumn(0, {1,2});
  clientModel.populateColumn(1, {"C1","C2"});
  /*
   * Setup child model
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 11 |   1    |  S11   |
   * ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 21 |   2    |  S21   |
   * ------------------------
   */
  VariantTableModel addressModel;
  addressModel.setObjectName("addressModel");
  addressModel.resize(3, 3);
  addressModel.populateColumn(0, {11,12,21});
  addressModel.populateColumn(1, {1 ,1 ,2 });
  addressModel.populateColumn(2, {"S11","S12","S21"});
  /*
   * Setup controllers and views
   */
  ItemModelControllerTester clientController;
  clientController.setModel(&clientModel);
  ItemModelControllerTester addressController;
  addressController.setModel(&addressModel);
  /*
   * Setup relation
   */
  ParentModelColumn clientId(0);
  FilterColumn addressClientId(1);
  FilterControllerRelation relation;
  relation.setParentController(&clientController);
  relation.setChildController(&addressController);
  relation.setRelationFilter(addressClientId == clientId);
  /*
   * Check that filter was applied for current client
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 11 |   1    |  S11   |
   * ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   */
  QCOMPARE(clientController.currentRow(), 0);
  QCOMPARE(addressController.modelForView()->rowCount(), 2);
  QCOMPARE(getModelData(addressController.modelForView(), 0, 0), QVariant(11));
  QCOMPARE(getModelData(addressController.modelForView(), 1, 0), QVariant(12));
  /*
   * Change current client and check address model
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 21 |   2    |  S21   |
   * ------------------------
   */
  QVERIFY(clientController.setCurrentRow(1));
  QCOMPARE(addressController.modelForView()->rowCount(), 1);
  QCOMPARE(getModelData(addressController.modelForView(), 0, 0), QVariant(21));
  QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
  /*
   * Insert a row with address controller, at end
   * Address model filtered on client 2, after insert done:
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 21 |   2    |  S21   |
   * ------------------------
   * |    |   2    |        |
   * ------------------------
   */
  addressController.setInsertLocation(ItemModelControllerTester::InsertAtEnd);
  QVERIFY(addressController.insert());
  QCOMPARE(addressController.modelForView()->rowCount(), 2);
  QCOMPARE(getModelData(addressController.modelForView(), 0, 0), QVariant(21));
  QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
  QCOMPARE(getModelData(addressController.modelForView(), 1, 1), QVariant(2));
  /*
   * Edit freshly inserted row
   * Address model filtered on client 2, after edition done:
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 21 |   2    |  S21   |
   * ------------------------
   * | 22 |   2    |  S22   |
   * ------------------------
   */
  setModelData(addressController.modelForView(), 1, 0, 22);
  setModelData(addressController.modelForView(), 1, 2, "S22");
  QVERIFY(addressController.submit());
  QCOMPARE(getModelData(addressController.modelForView(), 0, 0), QVariant(21));
  QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
  QCOMPARE(getModelData(addressController.modelForView(), 0, 2), QVariant("S21"));
  QCOMPARE(getModelData(addressController.modelForView(), 1, 0), QVariant(22));
  QCOMPARE(getModelData(addressController.modelForView(), 1, 1), QVariant(2));
  QCOMPARE(getModelData(addressController.modelForView(), 1, 2), QVariant("S22"));
  /*
   * Insert a row into address model, at beginning
   * Address model filtered on client 2, after insert done:
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * |    |   2    |        |
   * ------------------------
   * | 21 |   2    |  S21   |
   * ------------------------
   * | 22 |   2    |  S22   |
   * ------------------------
   */
  addressController.setInsertLocation(ItemModelControllerTester::InsertAtBeginning);
  QVERIFY(addressController.insert());
  QCOMPARE(addressController.modelForView()->rowCount(), 3);
  QVERIFY(getModelData(addressController.modelForView(), 0, 0).isNull());
  QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
  QVERIFY(getModelData(addressController.modelForView(), 0, 2).isNull());
  QCOMPARE(getModelData(addressController.modelForView(), 1, 0), QVariant(21));
  QCOMPARE(getModelData(addressController.modelForView(), 1, 1), QVariant(2));
  QCOMPARE(getModelData(addressController.modelForView(), 1, 2), QVariant("S21"));
  QCOMPARE(getModelData(addressController.modelForView(), 2, 0), QVariant(22));
  QCOMPARE(getModelData(addressController.modelForView(), 2, 1), QVariant(2));
  QCOMPARE(getModelData(addressController.modelForView(), 2, 2), QVariant("S22"));
  QVERIFY(addressController.submit());
  /*
   * Enable formatting for address controller
   * (Will append a proxy model)
   * Address model filtered on client 2, after proxy model insertion done:
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * |    |   2    |        |
   * ------------------------
   * | 21 |   2    |  S21   |
   * ------------------------
   * | 22 |   2    |  S22   |
   * ------------------------
   */
  FormatProxyModel addressFormatModel;
  addressController.appendProxyModel(&addressFormatModel);
  addressFormatModel.setTextAlignmentForColumn(0, Qt::AlignCenter);
  QCOMPARE(addressController.modelForView()->rowCount(), 3);
  QVERIFY(getModelData(addressController.modelForView(), 0, 0).isNull());
  QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
  QVERIFY(getModelData(addressController.modelForView(), 0, 2).isNull());
  QCOMPARE(getModelData(addressController.modelForView(), 1, 0), QVariant(21));
  QCOMPARE(getModelData(addressController.modelForView(), 1, 1), QVariant(2));
  QCOMPARE(getModelData(addressController.modelForView(), 1, 2), QVariant("S21"));
  QCOMPARE(getModelData(addressController.modelForView(), 2, 0), QVariant(22));
  QCOMPARE(getModelData(addressController.modelForView(), 2, 1), QVariant(2));
  QCOMPARE(getModelData(addressController.modelForView(), 2, 2), QVariant("S22"));
  /*
   * Enable formatting for client controller
   * (Will append a proxy model)
   * Address model filtered on client 2, after proxy model insertion done:
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * |    |   2    |        |
   * ------------------------
   * | 21 |   2    |  S21   |
   * ------------------------
   * | 22 |   2    |  S22   |
   * ------------------------
   */
  FormatProxyModel clientFormatModel;
  clientController.appendProxyModel(&clientFormatModel);
  clientFormatModel.setTextAlignmentForColumn(0, Qt::AlignCenter);
  // Client controller will go to first row after model change, go back to client 2
  clientController.setCurrentRow(1);
  QCOMPARE(addressController.modelForView()->rowCount(), 3);
  QVERIFY(getModelData(addressController.modelForView(), 0, 0).isNull());
  QCOMPARE(getModelData(addressController.modelForView(), 0, 1), QVariant(2));
  QVERIFY(getModelData(addressController.modelForView(), 0, 2).isNull());
  QCOMPARE(getModelData(addressController.modelForView(), 1, 0), QVariant(21));
  QCOMPARE(getModelData(addressController.modelForView(), 1, 1), QVariant(2));
  QCOMPARE(getModelData(addressController.modelForView(), 1, 2), QVariant("S21"));
  QCOMPARE(getModelData(addressController.modelForView(), 2, 0), QVariant(22));
  QCOMPARE(getModelData(addressController.modelForView(), 2, 1), QVariant(2));
  QCOMPARE(getModelData(addressController.modelForView(), 2, 2), QVariant("S22"));
}

void FilterControllerRelationTest::filterTableViewTest()
{
  QModelIndex index;
  /*
   * Setup parent model
   * -------------
   * | Id | Name |
   * -------------
   * | 1  | C1   |
   * -------------
   * | 2  | C2   |
   * -------------
   */
  VariantTableModel clientModel;
  clientModel.resize(2, 2);
  clientModel.populateColumn(0, {1,2});
  clientModel.populateColumn(1, {"C1","C2"});
  /*
   * Setup child model
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 11 |   1    |  S11   |
   * ------------------------
   * | 12 |   1    |  S12   |
   * ------------------------
   * | 21 |   2    |  S21   |
   * ------------------------
   */
  VariantTableModel addressModel;
  addressModel.resize(3, 3);
  addressModel.populateColumn(0, {11,12,21});
  addressModel.populateColumn(1, {1 ,1 ,2 });
  addressModel.populateColumn(2, {"S11","S12","S21"});
  /*
   * Setup controllers and views
   */
  QTableView clientView;
  TableViewController clientController;
  clientController.setModel(&clientModel);
  clientController.setView(&clientView);
  QTableView addressView;
  TableViewController addressController;
  addressController.setInsertLocation(TableViewController::InsertAtEnd);
  addressController.setModel(&addressModel);
  addressController.setView(&addressView);
  /*
   * Setup relation
   */
  ParentModelColumn clientId(0);
  FilterColumn addressClientId(1);
  FilterControllerRelation relation;
  relation.setParentController(&clientController);
  relation.setChildController(&addressController);
  relation.setRelationFilter(addressClientId == clientId);
  /*
   * Check that filter was applied for current client
   */
  QCOMPARE(clientController.currentRow(), 0);
  QCOMPARE(addressView.model()->rowCount(), 2);
  index = addressView.model()->index(0, 0);
  QCOMPARE(addressView.model()->data(index), QVariant(11));
  /*
   * Change current client
   */
  QVERIFY(clientController.setCurrentRow(1));
  QCOMPARE(addressView.model()->rowCount(), 1);
  index = addressView.model()->index(0, 0);
  QCOMPARE(addressView.model()->data(index), QVariant(21));
  /*
   * Insert a row in address model
   * Address model filtered on client 2, after inert done:
   * ------------------------
   * | Id | Cli_Id | Street |
   * ------------------------
   * | 21 |   2    |  S21   |
   * ------------------------
   * | 22 |   2    |  S22   |
   * ------------------------
   */
  QVERIFY(addressController.insertLocation() == TableViewController::InsertAtEnd);
  QVERIFY(addressController.insert());
  // New inserted row must not be filtered until it is saved
  QCOMPARE(addressView.model()->rowCount(), 2);
  QVERIFY(setModelData(addressView.model(), 1, 0, 22));
  QVERIFY(setModelData(addressView.model(), 1, 1, 2));
  QVERIFY(setModelData(addressView.model(), 1, 2, "S22"));
  /*
   * Save by calling controller's method
   */
  QVERIFY(addressController.submit());
  QCOMPARE(addressView.model()->rowCount(), 2);
  QCOMPARE(getModelData(addressView.model(), 0, 0), QVariant(21));
  QCOMPARE(getModelData(addressView.model(), 0, 1), QVariant(2));
  QCOMPARE(getModelData(addressView.model(), 1, 0), QVariant(22));
  QCOMPARE(getModelData(addressView.model(), 1, 1), QVariant(2));
  /*
   * Check that relation filter works again
   */
  
  /**
   * \todo Check also submiting in view directly
   */
  
  /*
   * Play
   */
//   clientModel.setHeaderData(0, Qt::Horizontal, "Id");
//   clientModel.setHeaderData(1, Qt::Horizontal, "Name");
//   clientView.setWindowTitle("Client");
//   clientView.resizeColumnsToContents();
//   clientView.show();
//   addressView.setWindowTitle("Address");
//   addressView.resizeColumnsToContents();
//   addressView.show();
//   while(clientView.isVisible()){
//     QTest::qWait(500);
//   }
  
  QFAIL("Not complete");
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  FilterControllerRelationTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
