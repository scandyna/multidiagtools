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
#include "PlatformTest.h"
#include "Mdt/DeployUtils/Platform.h"

using namespace Mdt::DeployUtils;

void PlatformTest::initTestCase()
{
}

void PlatformTest::cleanupTestCase()
{
}

/*
 * Tests
 */

void PlatformTest::simpleSetGetTest()
{
  Platform pf(OperatingSystem::Windows, Compiler::Gcc, Processor::X86_32);
  QVERIFY(pf.operatingSystem() == OperatingSystem::Windows);
  QVERIFY(pf.compiler() == Compiler::Gcc);
  QVERIFY(pf.processor() == Processor::X86_32);
}

void PlatformTest::nativePlatformTest()
{
  Platform pf;

  // Check that the correct OS was choosen
#ifdef Q_OS_LINUX
  QVERIFY(pf.operatingSystem() == OperatingSystem::Linux);
#else
  QFAIL("Current OS is not supported");
#endif

  // Check that correct compiler was choosen
#ifdef Q_CC_GNU
  QVERIFY(pf.compiler() == Compiler::Gcc);
#else
  QFAIL("Current compiler is not supported");
#endif

  // Check that correct processor was choosen
#ifdef Q_PROCESSOR_X86_32
  QVERIFY(pf.processor() == Processor::X86_32);
#elif defined Q_PROCESSOR_X86_64
  QVERIFY(pf.processor() == Processor::X86_64);
#else
  QFAIL("Current processor architecture not supported");
#endif
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  PlatformTest test;

  if(!app.init()){
    return 1;
  }
//   app.debugEnvironnement();

  return QTest::qExec(&test, argc, argv);
}
