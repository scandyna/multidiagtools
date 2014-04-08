/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_CABLE_LIST_TEST_SCENARIO_H
#define MDT_CABLE_LIST_TEST_SCENARIO_H

#include <QSqlDatabase>

class mdtCableListTestScenario
{
 public:

  mdtCableListTestScenario(QSqlDatabase db);

  /*
   * Create some vehicle types for tests:
   *  - Id_PK : 1 , Type : Vehicle type 1
   *  - Id_PK : 2 , Type : Vehicle type 2
   */
  void createTestVehicleTypes();
  /*
   * Remove test vehicle types
   */
  void removeTestVehicleTypes();


 private:

  QSqlDatabase pvDatabase;
};

#endif //#ifndef MDT_CABLE_LIST_TEST_SCENARIO_H
