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
#ifndef MDT_TT_TEST_NODE_TEST_DATA_H
#define MDT_TT_TEST_NODE_TEST_DATA_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QString>

/*
 * Populate given database with data for test node A
 * Database schema must allready exist
 */
class mdtTtTestNodeATestData
{
 public:

  // Constructor
  mdtTtTestNodeATestData(QSqlDatabase db)
   : pvDatabase(db)
  {
  }
  // Destructor - will also remove created data
  ~mdtTtTestNodeATestData()
  {
    clear();
  }
  // Create data
  bool populate();
  // Clear craeted data
  void clear();

 private:

  // Populate VehicleType_tbl and TestNode_tbl
  bool createTestNodeData();
  // Clear created data in VehicleType_tbl and TestNode_tbl
  void removeTestNodeData();
  /*
   * Create relays - will also create realted unit connections
   */
  bool createRelays();
  /*
   * Create measure and test connections
   */
  bool createMeasureAndTestConnections();
  // Helper function to populate Unit_tbl and VehicleType_Unit_tbl part
  QVariant addUnit(const QString & schemaPos);
  // Helper function to populate TestNodeUnit_tbl part
  bool addTestNodeUnit(const QVariant & unitId, const QVariant & testNodeId, const QString & type, const QVariant & ioPos);
  bool addTestNodeUnit(const QVariant & unitId, const QVariant & testNodeId, const QString & type);
  // Helper function to populate UnitConnection_tbl part
  QVariant addUnitConnection(const QVariant & unitId, const QString & contactName, const QVariant & resistance);
  // Helper function to populate TestNodeUnitConnection_tbl part
  bool addTestNodeUnitConnection(const QVariant & unitId, const QVariant & unitConnectionId);
  // Remove relays - also removes related connections
  void removeRelays();
  /*
   * Add links
   */
  bool createsLinks();
  // Helper function to add a link
  bool addLink(const QString & identification, const QVariant & R, const QString & startPos, const QString & startConnection, const QString & endPos, const QString & endConnection);
  // Remove created links
  void removeLinks();
  // Helper function to remove a link
  void removeLink(const QVariant & startConnectionId, const QVariant & endConnectionId);

  bool beginTransaction();
  bool commitTransaction();
  bool rollbackTransaction();
  void debugSqlError(const QSqlError & error);
  void debugSqlError(const QSqlQuery & query);

  Q_DISABLE_COPY(mdtTtTestNodeATestData);

  QSqlDatabase pvDatabase;
  QVariant pvTestNodeId;
};

#endif // #ifndef MDT_TT_TEST_NODE_TEST_DATA_H
