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
#include "ControllerStatePermissionTest.h"
#include "Mdt/ItemEditor/ControllerStatePermission.h"
#include "Mdt/ItemEditor/AbstractControllerStatePermission.h"

using namespace Mdt::ItemEditor;

/*
 * Init/Cleanup
 */

void ControllerStatePermissionTest::initTestCase()
{
}

void ControllerStatePermissionTest::cleanupTestCase()
{
}

/*
 * Test class
 */

class PermissionTestClass : public AbstractControllerStatePermission
{
 public:

  bool canChangeCurrentRow(ControllerState) const override
  {
    return true;
  }

  bool canInsert(ControllerState) const override
  {
    return false;
  }
};

/*
 * Tests
 */

void ControllerStatePermissionTest::constructTest()
{
  /*
   * Contruct a null controller state permission
   */
  ControllerStatePermission p0;
  QVERIFY(p0.isNull());
  QVERIFY(!p0.canChangeCurrentRow(ControllerState::Visualizing));
  QVERIFY(!p0.canInsert(ControllerState::Visualizing));
  /*
   * Controller state permission with test class implementation
   */
  auto p = ControllerStatePermission::make<PermissionTestClass>();
  QVERIFY(!p.isNull());
  QVERIFY(p.canChangeCurrentRow(ControllerState::Visualizing));
  QVERIFY(!p.canInsert(ControllerState::Visualizing));
}

void ControllerStatePermissionTest::copyTest()
{
  auto p1 = ControllerStatePermission::make<PermissionTestClass>();
  QVERIFY(p1.canChangeCurrentRow(ControllerState::Visualizing));
  QVERIFY(!p1.canInsert(ControllerState::Visualizing));
  /*
   * Copy construct
   */
  auto p2 = p1;
  QVERIFY(p2.canChangeCurrentRow(ControllerState::Visualizing));
  QVERIFY(!p2.canInsert(ControllerState::Visualizing));
  /*
   * Copy assign
   */
  ControllerStatePermission p3;
  // Make shure p3 is used before assign
  QVERIFY(!p3.canChangeCurrentRow(ControllerState::Visualizing));
  p3 = p1;
  QVERIFY(p3.canChangeCurrentRow(ControllerState::Visualizing));
  QVERIFY(!p3.canInsert(ControllerState::Visualizing));
  /*
   * Move construct
   */
  auto p4 = std::move( ControllerStatePermission::make<PermissionTestClass>() );
  QVERIFY(p4.canChangeCurrentRow(ControllerState::Visualizing));
  QVERIFY(!p4.canInsert(ControllerState::Visualizing));
  /*
   * Move assign
   */
  ControllerStatePermission p5;
  // Make shure p3 is used before assign
  QVERIFY(!p5.canChangeCurrentRow(ControllerState::Visualizing));
  p5 = std::move( ControllerStatePermission::make<PermissionTestClass>() );
  QVERIFY(p5.canChangeCurrentRow(ControllerState::Visualizing));
  QVERIFY(!p5.canInsert(ControllerState::Visualizing));
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  ControllerStatePermissionTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
