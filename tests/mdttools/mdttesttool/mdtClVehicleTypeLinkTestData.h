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
#ifndef MDT_CL_VEHICLE_TYPE_LINK_TEST_DATA_H
#define MDT_CL_VEHICLE_TYPE_LINK_TEST_DATA_H

#include "mdtClVehicleTypeLinkKeyData.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QString>
#include <QList>

/*
 * Helper class to create temporary test data
 */
class mdtClVehicleTypeLinkTestData
{
 public:

  // Constructor
  mdtClVehicleTypeLinkTestData(QSqlDatabase db)
   : pvDatabase(db)
  {
  }
  // Destructor - will also remove created data
  ~mdtClVehicleTypeLinkTestData()
  {
    clear();
  }
  // Clear craeted data
  void clear();
  // Create a vehicle type
  bool createVehicleType(int id, const QString & type, const QString & subType, const QString & seriesNumber);
  // Remove created vehicle types
  void removeCreatedVehicleTypes();
  // Add vehicle type - unit assignation
  bool addVehicleTypeUnitAssignation(int vtId, int unitId);
  // Remove created VTU
  void removeCreatedVehicleTypeUnitAssignations();
  // Add vehicle type - link assignation
  bool addVehicleTypeLinkAssignation(int vtId, int ucsId, int uceId);
  // Remove created vehicle type - link assignations
  void removeCreatedVehicleTypeLinkAssignations();

 private:

  void debugSqlError(const QSqlError & error);
  void debugSqlError(const QSqlQuery & query);

  Q_DISABLE_COPY(mdtClVehicleTypeLinkTestData);

  QSqlDatabase pvDatabase;
  QList<int> pvCreatedVehicleTypeIdList;
  struct vtuData{
    QVariant vtId;
    QVariant unitId;
  };
  QList<vtuData> pvCreatedVtuList;
  QList<mdtClVehicleTypeLinkKeyData> pvCreatedVtlList;
};

#endif // #ifndef MDT_CL_VEHICLE_TYPE_LINK_TEST_DATA_H
