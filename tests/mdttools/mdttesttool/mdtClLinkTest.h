/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_CL_LINK_TEST_H
#define MDT_CL_LINK_TEST_H

#include "mdtTest.h"
#include <QTemporaryFile>
#include <QSqlDatabase>

class mdtClLinkTest : public mdtTest
{
 Q_OBJECT

 private slots:

  /*
   * Will create database schema (see createDatabaseSchema() )
   * No data is inserted in tables by this method.
   * Each test also has a empty schema at startup,
   *  and must assure that they finish with a empty schema at end.
   */
  void initTestCase();
  void cleanupTestCase();

  void linkTypeDataTest();
  void linkTypeModelTest();

  void linkDirectionDataTest();
  void linkDirectionModelTest();

  void linkVersionDataTest();
  void linkVersionAddGetRemoveTest();
  void linkVersionModelTest();

//   void linkModificationDataTest();
//   void linkModificationAddGetRemoveTest();

  void vehicleTypeStartEndKeyDataTest();
  void vehicleTypeLinkKeyDataTest();
  void vehicleTypeLinkAddGetRemoveTest();
  void vehicleTypeLinkUpdateTest();
  
  void vehicleTypeCheckBoxTest();
  void vehicleTypeLinkAssignationWidgetTest();

  void linkDataTest();
  void linkAddGetRemoveTest();
  void linkAndVehicleTypeAddGetRemoveTest();
  void linkFromArticleLinkAddTest();

 private:

  // Create test database schema - Will FAIL on problem
  void createDatabaseSchema();

  QTemporaryFile pvTempFile;  // We keep it as member, so file is destroyed automatically
  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_CL_LINK_TEST_H
