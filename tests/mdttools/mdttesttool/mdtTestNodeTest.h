/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_TEST_NODE_TEST_H
#define MDT_TEST_NODE_TEST_H

#include "mdtTest.h"
#include "mdtSqlDatabaseManager.h"
#include "mdtTtTestNodeRouteData.h"
#include <QString>
#include <QVariant>

class mdtTestNodeTest : public mdtTest
{
 Q_OBJECT

 private slots:

  // Will create database schema (see createDatabaseSchema() )
  void initTestCase();
  void cleanupTestCase();

  void testNodeUnitTest();

  void getRouteRelayIdTest();
  void getRouteRelaysTest();
  void testNodeRouteDataTest();
  void routeBuildTest();
  void routeAddRemoveTest();

  void shortDetectionTest();

  void testNodeManagerTest();

 private:

  // Helper function to get a connection ID
  QVariant getConnectionId(const QString & testNodeAlias, const QString & schemaPosition, const QString & contact, bool & ok);
  // Helper function to build a route
  mdtTtTestNodeRouteData builRoute(const QString & testNodeAlias, const QString & schemaPositionA, const QString & contactA, const QString & schemaPositionB, const QString & contactB, bool & ok);
  // Helper function to get a route from database
  mdtTtTestNodeRouteData getRoute(const QString & testNodeAlias, const QString & schemaPositionA, const QString & contactA, const QString & schemaPositionB, const QString & contactB, bool & ok);
  // Create test database schema - Will FAIL on problem
  void createDatabaseSchema();

  mdtSqlDatabaseManager pvDatabaseManager;
};

#endif // #ifndef MDT_TEST_NODE_TEST_H
