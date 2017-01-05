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
#include "ItemViewTestEdit.h"
#include "Mdt/Application.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include "Mdt/ItemModel/RelationFilterProxyModel.h"
#include "Mdt/ItemEditor/TableViewController.h"
#include "Mdt/ItemEditor/WidgetMapperController.h"
#include "Mdt/ItemEditor/ControllerRelation.h"
#include "Mdt/ItemEditor/ControllerRelationImpl.h"
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

namespace ItemModel = Mdt::ItemModel;
namespace ItemEditor = Mdt::ItemEditor;
using ItemModel::VariantTableModel;
using ItemModel::FilterColumn;
using ItemModel::ParentModelColumn;
using ItemEditor::TableViewController;
using ItemEditor::AbstractController;
using ItemEditor::WidgetMapperController;
using ItemEditor::ControllerRelationImpl;
using ItemEditor::ControllerRelation;
using ItemEditor::ControllerRelationList;
using ItemEditor::ControllerState;

void ControllerRelationTest::initTestCase()
{
}

void ControllerRelationTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void ControllerRelationTest::enableDisableRelationFilterTest()
{
  VariantTableModel model;
  QTableView view;
  /*
   * Initial state
   */
  TableViewController controller;
  QVERIFY(!controller.isRelationFilterEnabled());
  /*
   * Setup
   */
  model.resize(3, 2);
  model.populateColumn(0, {1,2,3});
  model.populateColumn(1, {"A","B","C"});
  controller.setModel(&model);
  controller.setView(&view);
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  QVERIFY(!controller.isRelationFilterEnabled());
  /*
   * Enable / disable
   */
  /// \todo Check what should be correct behaviour when no parent model was set, etc...
  // Enable
  controller.setRelationFilterEnabled(true);
  QVERIFY(controller.isRelationFilterEnabled());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Disable
  controller.setRelationFilterEnabled(false);
  QVERIFY(!controller.isRelationFilterEnabled());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);
  // Enable
  controller.setRelationFilterEnabled(true);
  QVERIFY(controller.isRelationFilterEnabled());
  QCOMPARE(controller.rowCount(), 3);
  QCOMPARE(controller.currentRow(), 0);

  QFAIL("Not complete");
}

void ControllerRelationTest::setRelationParentModelTest()
{
  VariantTableModel parentModel;
  TableViewController controller;

  QVERIFY(!controller.isRelationFilterEnabled());
  controller.setRelationFilterParentModel(&parentModel);
  QVERIFY(controller.isRelationFilterEnabled());
  
  QFAIL("Not complete");
}

void ControllerRelationTest::setControllersTest()
{
  TableViewController parentController;
  TableViewController childController;
  /*
   * Create relation
   */
  ControllerRelationImpl relation(&parentController);
//   ControllerRelation<AbstractController, ControllerRelationImpl> relation(&parentController);
//   ControllerRelation relation(&parentController);
//   QVERIFY(relation.proxyModel() != nullptr);
//   QVERIFY(relation.proxyModel()->parentModel() == parentController.modelForView());

  QFAIL("Not complete");
}

void ControllerRelationTest::changeModelTest()
{
  /*
   * Change model in parent controller
   */
  
  
  /*
   * Enable filter in parent controller
   */
  
  /*
   * Disable filter in parent controller
   */
  
  
  
  QFAIL("Not complete");
}

void ControllerRelationTest::relationSetModelToControllersFirstTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  TableViewController parentController;
  parentController.setModel(&parentModel);
  QCOMPARE(parentController.sourceModel(), &parentModel);
  /*
   * Setup child model and controller
   */
  VariantTableModel childModel;
  TableViewController childController;
  childController.setModel(&childModel);
  QCOMPARE(childController.sourceModel(), &childModel);
  /*
   * Setup relation
   */
//   ControllerRelation relation(&parentController);
//   ControllerRelation<AbstractController, ControllerRelationImpl> relation(&parentController);
  ControllerRelationImpl relation(&parentController);
  QCOMPARE(parentController.sourceModel(), &parentModel);
  QVERIFY(!parentController.isRelationFilterEnabled());
  QCOMPARE(childController.sourceModel(), &childModel);
  QVERIFY(!childController.isRelationFilterEnabled());
  relation.setChildController(&childController, FilterColumn(0) == ParentModelColumn(0));
  QCOMPARE(parentController.sourceModel(), &parentModel);
  QVERIFY(!parentController.isRelationFilterEnabled());
  QCOMPARE(childController.sourceModel(), &childModel);
  QVERIFY(childController.isRelationFilterEnabled());
  QCOMPARE(childController.relationFilterModel()->sourceModel(), &childModel);
  QCOMPARE(childController.relationFilterModel()->parentModel(), &parentModel);
}

void ControllerRelationTest::relationSetModelToControllersAfterTest()
{
  /*
   * Setup parent and child controllers
   */
  TableViewController parentController;
  TableViewController childController;
  /*
   * Setup relation
   */
//   ControllerRelation relation(&parentController);
//   ControllerRelation<AbstractController, ControllerRelationImpl> relation(&parentController);
  ControllerRelationImpl relation(&parentController);
  relation.setChildController(&childController, FilterColumn(0) == ParentModelColumn(0));
  QVERIFY(parentController.sourceModel() == nullptr);
  QVERIFY(!parentController.isRelationFilterEnabled());
  QVERIFY(childController.sourceModel() == nullptr);
  QVERIFY(childController.isRelationFilterEnabled());
  QVERIFY(childController.relationFilterModel()->sourceModel() == nullptr);
  QVERIFY(childController.relationFilterModel()->parentModel() == nullptr);
  /*
   * Setup parent model
   */
  VariantTableModel parentModel;
  parentController.setModel(&parentModel);
  relation.setParentControllerModelToChildController();
  QCOMPARE(parentController.sourceModel(), &parentModel);
  QVERIFY(!parentController.isRelationFilterEnabled());
  QVERIFY(childController.sourceModel() == nullptr);
  QVERIFY(childController.isRelationFilterEnabled());
  QVERIFY(childController.relationFilterModel()->sourceModel() == nullptr);
  QVERIFY(childController.relationFilterModel()->parentModel() == &parentModel);
  /*
   * Setup child model
   */
  VariantTableModel childModel;
  childController.setModel(&childModel);
  QCOMPARE(parentController.sourceModel(), &parentModel);
  QVERIFY(!parentController.isRelationFilterEnabled());
  QCOMPARE(childController.sourceModel(), &childModel);
  QVERIFY(childController.isRelationFilterEnabled());
  QCOMPARE(childController.relationFilterModel()->sourceModel(), &childModel);
  QCOMPARE(childController.relationFilterModel()->parentModel(), &parentModel);
}

void ControllerRelationTest::parentTableChildTableTest()
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
  addressController.setModel(&addressModel);
  addressController.setView(&addressView);
  /*
   * Setup relation
   */
  ParentModelColumn addressClientId(0);
  FilterColumn clientId(1);
//   ControllerRelation relation(&clientController);
//   ControllerRelation<AbstractController, ControllerRelationImpl> relation(&clientController);
  ControllerRelationImpl relation(&clientController);
  relation.setChildController(&addressController, clientId == addressClientId);
  /*
   * Play
   */
  clientModel.setHeaderData(0, Qt::Horizontal, "Id");
  clientModel.setHeaderData(1, Qt::Horizontal, "Name");
  clientView.setWindowTitle("Client");
  clientView.resizeColumnsToContents();
  clientView.show();
  addressView.setWindowTitle("Address");
  addressView.resizeColumnsToContents();
  addressView.show();
//   while(clientView.isVisible()){
//     QTest::qWait(500);
//   }

  QFAIL("Not complete");
}

void ControllerRelationTest::relationListSetModelToControllersFirstTest()
{
  /*
   * Setup parent model and controller
   */
  VariantTableModel parentModel;
  TableViewController parentController;
  parentController.setModel(&parentModel);
  /*
   * Setup first child model and controller
   */
  VariantTableModel childModel1;
  TableViewController childController1;
  childController1.setModel(&childModel1);
  /*
   * Setup second child model and controller
   */
  VariantTableModel childModel2;
  TableViewController childController2;
  childController2.setModel(&childModel2);
  /*
   * Setup relation list
   */
//   ControllerRelationList relationList(&parentController);
  ControllerRelationList<AbstractController, ControllerRelationImpl> relationList(&parentController);
  QCOMPARE(relationList.childControllerCount(), 0);
  // Add first child controller
  relationList.addChildController(&childController1, FilterColumn(0) == ParentModelColumn(0));
  QCOMPARE(relationList.childControllerCount(), 1);
  QVERIFY(childController1.isRelationFilterEnabled());
  QCOMPARE(childController1.relationFilterModel()->sourceModel(), &childModel1);
  QCOMPARE(childController1.relationFilterModel()->parentModel(), &parentModel);
  // Add second child controller
  relationList.addChildController(&childController2, FilterColumn(0) == ParentModelColumn(0));
  QCOMPARE(relationList.childControllerCount(), 2);
  QVERIFY(childController2.isRelationFilterEnabled());
  QCOMPARE(childController2.relationFilterModel()->sourceModel(), &childModel2);
  QCOMPARE(childController2.relationFilterModel()->parentModel(), &parentModel);
}

void ControllerRelationTest::relationListSetModelToControllersAfterTest()
{

  QFAIL("Not complete");
}

void ControllerRelationTest::relationListBasicSetGetTest()
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
   * Setup relation list
   */
//   ControllerRelationList relationList(&parentController);
  ControllerRelationList<AbstractController, ControllerRelationImpl> relationList(&parentController);
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


/*
 * Helpers
 */

void ControllerRelationTest::beginEditing(QAbstractItemView& view, const QModelIndex& index, BeginEditTrigger trigger)
{
  ItemViewTestEdit::beginEditing(view, index, trigger);
}

void ControllerRelationTest::editText(QAbstractItemView& view, const QModelIndex& editingIndex, const QString& str)
{
  ItemViewTestEdit::editText(view, editingIndex, str);
}

void ControllerRelationTest::endEditing(QAbstractItemView& view, const QModelIndex& editingIndex, EndEditTrigger trigger)
{
  ItemViewTestEdit::endEditing(view, editingIndex, trigger);
}

void ControllerRelationTest::edit(QAbstractItemView& view, const QModelIndex& index, const QString& str, BeginEditTrigger beginEditTrigger, EndEditTrigger endEditTrigger)
{
  ItemViewTestEdit::edit(view, index, str, beginEditTrigger, endEditTrigger);
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
