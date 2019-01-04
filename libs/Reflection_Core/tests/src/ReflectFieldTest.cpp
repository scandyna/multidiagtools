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
#include "ReflectFieldTest.h"
#include "Mdt/Reflection/ReflectField.h"
#include <QString>

using namespace Mdt::Reflection;

/*
 * Person struct
 */

struct PersonDataStruct
{
  int id;
  QString firstName;
  QString lastName;
};


/*
 * Address struct
 */

struct AddressDataStruct
{
  int id;
  QString street;
  int personId;
};

/*
 * Helpers
 */


/*
 * Type traits tests
 */

/*
 * Tests
 */



/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::CoreApplication app(argc, argv);
  ReflectFieldTest test;

//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
