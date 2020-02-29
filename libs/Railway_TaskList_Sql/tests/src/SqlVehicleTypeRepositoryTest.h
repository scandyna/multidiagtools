/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef RW_TEST_MAIN_H
#define RW_TEST_MAIN_H

#include "TestBase.h"

class SqlVehicleTypeRepositoryTest : public TestBase
{
 Q_OBJECT

 private slots:

  void initTestCase();

  void sandbox();

  void addGetUpdateRemoveTest();
  void getAllAsyncTest();
  void getAllAsyncTest_data();
};

#endif // #ifndef RW_TEST_MAIN_H
