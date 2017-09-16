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
#include "ControllerStatePermissionProxyModelTest.h"
#include "ItemModelControllerTester.h"
#include "Mdt/ItemEditor/ControllerState.h"
#include "Mdt/ItemEditor/ControllerEvent.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include "Mdt/ItemEditor/AbstractControllerStateTable.h"
#include "Mdt/ItemEditor/AbstractControllerStatePermission.h"
#include "Mdt/ItemModel/VariantTableModel.h"
#include <QScopedPointer>
#include <QModelIndex>
#include <QSignalSpy>

using namespace Mdt::ItemEditor;
using namespace Mdt::ItemModel;

/*
 * Init/Cleanup
 */

void ControllerStatePermissionProxyModelTest::initTestCase()
{
}

void ControllerStatePermissionProxyModelTest::cleanupTestCase()
{
}

/*
 * Implementation classes
 */

class StateTableTestClass : public AbstractControllerStateTable
{
 public:

  void createTable() override
  {
    addTransition(ControllerState::Visualizing, ControllerEvent::DataEditionStarted, ControllerState::Editing);
    addTransition(ControllerState::Visualizing, ControllerEvent::InsertStarted, ControllerState::Inserting);
    addTransition(ControllerState::Visualizing, ControllerEvent::EditionStartedFromParent, ControllerState::ParentEditing);
    addTransition(ControllerState::Visualizing, ControllerEvent::EditionStartedFromChild, ControllerState::ChildEditing);
    addTransition(ControllerState::Editing, ControllerEvent::SubmitDone, ControllerState::Visualizing);
    addTransition(ControllerState::Editing, ControllerEvent::RevertDone, ControllerState::Visualizing);
    addTransition(ControllerState::Inserting, ControllerEvent::SubmitDone, ControllerState::Visualizing);
    addTransition(ControllerState::Inserting, ControllerEvent::RevertDone, ControllerState::Visualizing);
    addTransition(ControllerState::ParentEditing, ControllerEvent::EditionDoneFromParent, ControllerState::Visualizing);
    addTransition(ControllerState::ChildEditing, ControllerEvent::EditionDoneFromChild, ControllerState::Visualizing);
  }
};

class StatePermissionTestClass : public AbstractControllerStatePermission
{
 public:

  bool canEdit(const AbstractControllerStateTable & st) const override
  {
    switch(st.currentState()){
      case ControllerState::Visualizing:
      case ControllerState::Editing:
      case ControllerState::EditingItem:
      case ControllerState::Inserting:
        return true;
      case ControllerState::ParentEditing:
      case ControllerState::ChildEditing:
      case ControllerState::Disabled:
        return false;
    }
    return false;
  }
};

/*
 * Tests
 */

void ControllerStatePermissionProxyModelTest::flagsTest()
{
  /*
   * Setup model and proxy model
   */
  VariantTableModel model;
  model.populate(1, 1);
  ControllerStatePermissionProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  /*
   * Check without state machine
   */
  QVERIFY(!containsFlag(proxyModel, 0, 0, Qt::ItemIsEditable));
  /*
   * Set state machine
   */
  QScopedPointer<ControllerStateMachine> stateMachine( ControllerStateMachine::makeNew<StateTableTestClass, StatePermissionTestClass>() );
  proxyModel.setStateMachine( stateMachine.data() );
  /*
   * Visualizing state
   */
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  QVERIFY( containsFlag(proxyModel, 0, 0, Qt::ItemIsEditable));
  /*
   * ChildEditing state
   */
  stateMachine->forceCurrentState(ControllerState::ChildEditing);
  QCOMPARE(stateMachine->currentState(), ControllerState::ChildEditing);
  QVERIFY(!containsFlag(proxyModel, 0, 0, Qt::ItemIsEditable));
}

void ControllerStatePermissionProxyModelTest::flagsSignalTest()
{
  /*
   * Setup model and proxy model
   */
  VariantTableModel model;
  model.populate(2, 3);
  ControllerStatePermissionProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  QSignalSpy flagsChangedSpy(&proxyModel, &ControllerStatePermissionProxyModel::flagsChanged);
  QVERIFY(flagsChangedSpy.isValid());
  /*
   * Set state machine
   */
  QScopedPointer<ControllerStateMachine> stateMachine( ControllerStateMachine::makeNew<StateTableTestClass, StatePermissionTestClass>() );
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  proxyModel.setStateMachine( stateMachine.data() );
  QCOMPARE(flagsChangedSpy.count(), 1);
  flagsChangedSpy.clear();
  /*
   * ChildEditing state
   */
  stateMachine->forceCurrentState(ControllerState::ChildEditing);
  QCOMPARE(stateMachine->currentState(), ControllerState::ChildEditing);
  QCOMPARE(flagsChangedSpy.count(), 1);
  flagsChangedSpy.clear();
  // Go same state again
  stateMachine->forceCurrentState(ControllerState::ChildEditing);
  QCOMPARE(stateMachine->currentState(), ControllerState::ChildEditing);
  QCOMPARE(flagsChangedSpy.count(), 0);
  /*
   * Visualizing state
   */
  stateMachine->forceCurrentState(ControllerState::Visualizing);
  QCOMPARE(stateMachine->currentState(), ControllerState::Visualizing);
  QCOMPARE(flagsChangedSpy.count(), 1);
  flagsChangedSpy.clear();
}

void ControllerStatePermissionProxyModelTest::sourceModelFlagsTest()
{
  /*
   * Setup model and proxy model
   */
  VariantTableModel model;
  model.populate(1, 1);
  QScopedPointer<ControllerStateMachine> stateMachine( ControllerStateMachine::makeNew<StateTableTestClass, StatePermissionTestClass>() );
  ControllerStatePermissionProxyModel proxyModel;
  proxyModel.setSourceModel(&model);
  proxyModel.setStateMachine( stateMachine.data() );
  /*
   * Model has ItemIsEditable flag
   */
  model.setItemEditable(0, 0, true);
  stateMachine->forceCurrentState(ControllerState::Visualizing);
  QVERIFY( containsFlag(proxyModel, 0, 0, Qt::ItemIsEditable));
  stateMachine->forceCurrentState(ControllerState::ChildEditing);
  QVERIFY(!containsFlag(proxyModel, 0, 0, Qt::ItemIsEditable));
  /*
   * Model do not have ItemIsEditable flag
   */
  model.setItemEditable(0, 0, false);
  stateMachine->forceCurrentState(ControllerState::Visualizing);
  QVERIFY(!containsFlag(proxyModel, 0, 0, Qt::ItemIsEditable));
  stateMachine->forceCurrentState(ControllerState::ChildEditing);
  QVERIFY(!containsFlag(proxyModel, 0, 0, Qt::ItemIsEditable));
}

/*
 * Helpers
 */

bool ControllerStatePermissionProxyModelTest::containsFlag(const ControllerStatePermissionProxyModel& proxyModel, int row, int column, Qt::ItemFlag flag)
{
  const auto index = proxyModel.index(row, column);
  Q_ASSERT(index.isValid());

  return ( proxyModel.flags(index) & flag );
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ControllerStatePermissionProxyModelTest test;

  return QTest::qExec(&test, argc, argv);
}
