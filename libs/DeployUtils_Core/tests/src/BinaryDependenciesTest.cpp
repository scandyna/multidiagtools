/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "BinaryDependenciesTest.h"
#include "Mdt/DeployUtils/BinaryDependencies.h"
#include <QCoreApplication>

using namespace Mdt::DeployUtils;

void BinaryDependenciesTest::initTestCase()
{
}

void BinaryDependenciesTest::cleanupTestCase()
{
}

// /** Sandbox
//  */
// 
// #include <QLatin1String>
// 
// constexpr char excludeStr[] = "excluse0";
// 
// constexpr int arraySize = 3;
// constexpr char excludeArray[arraySize][9] =
// {
//   "excluse0",
//   "excluse1",
//   "other"
// };
// 
// void BinaryDependenciesTest::sandbox()
// {
//   constexpr QLatin1String str(excludeStr);
// 
//   qDebug() << "excludeStr: " << excludeStr << " , str: " << str;
//   
//   const QString lib = "excluse1";
//   
//   for(int i = 0; i < arraySize; ++i){
//     qDebug() << "Arr[" << i << "]: " << excludeArray[i];
//     if(lib == QLatin1String(excludeArray[i])){
//       qDebug() << "-> is excluded !";
//     }
//   }
// }


/*
 * Tests
 */

void BinaryDependenciesTest::runTest()
{
  BinaryDependencies deps;

  QVERIFY(deps.isValid());
  QVERIFY(deps.findDependencies( QCoreApplication::applicationFilePath() ));
  QVERIFY(!deps.dependencies().isEmpty());
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  BinaryDependenciesTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
