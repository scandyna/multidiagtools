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
#ifndef MDT_TT_DUT_01_TEST_DATA_H
#define MDT_TT_DUT_01_TEST_DATA_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QString>

/*
 * Populate given database with data for DUT 01
 * Database schema must allready exist
 */
class mdtTtDut01TestData
{
 public:

  // Constructor
  mdtTtDut01TestData(QSqlDatabase db)
   : pvDatabase(db)
  {
  }
  // Destructor - will also remove created data
  ~mdtTtDut01TestData()
  {
    clear();
  }
  // Create data
  bool populate();
  // Clear craeted data
  void clear();

 private:

  // Create vehicle type + schema position (and vehicle type - unit association)
  bool createVehicleTypeAndUnit();
  // Remove created unit and vehicle type
  void removeUnitAndVehicleType();

  // Create X1 connector and its contacts
  bool createConnectorX1();
  // Remove created connections and connectors
  void removeConnections();
  // Helper function to add a unit connection
  bool addUnitConnection(const QVariant & unitConnectorId, const QString & contactType, const QString & contactName);
  // Create /remove links
  bool createsLinks();
  void removeLinks();
  // Helper function to add a link - Will populate Link_tbl and VehicleType_Link_tbl
  bool addLink(const QString & identification, const QVariant & R, const QString & startConnectorName, const QString & startConnection, const QString & endConnectorName, const QString & endConnection);
  // Helper function to remove a link
  void removeLink(const QVariant & startConnectionId, const QVariant & endConnectionId);

  bool beginTransaction();
  bool commitTransaction();
  bool rollbackTransaction();
  void debugSqlError(const QSqlError & error);
  void debugSqlError(const QSqlQuery & query);

  Q_DISABLE_COPY(mdtTtDut01TestData);

  QSqlDatabase pvDatabase;
  QVariant pvVehicleTypeId;
  QVariant pvUnitId;
};

#endif // #ifndef MDT_TT_DUT_01_TEST_DATA_H
