/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtPortLockTest.h"
#include "mdtPort.h"
#include "mdtApplication.h"
#include "linux/mdtPortLock.h"
#include <QDir>

//#include <QDebug>

void mdtPortLockTest::lckFilesTest()
{
  QDir dir;
  int fd;
  mdtPortLock *lck;

  // Check that LCK.. files initially not exists
  QVERIFY(!dir.exists("/tmp/LCK..ttyS0"));
  QVERIFY(!dir.exists("/var/lock/LCK..ttyS0"));

  // Create a portLock object and check initial states
  lck = new mdtPortLock;
  QVERIFY(!lck->isLocked());
  QVERIFY(!lck->isLockedByAnother());

  // Open a port and check that LCK.. files exists
  fd = lck->openLocked("/dev/ttyS0" , O_RDWR | O_NOCTTY | O_NONBLOCK);
  QVERIFY(fd >= 0);
  QVERIFY(dir.exists("/tmp/LCK..ttyS0"));
  QVERIFY(dir.exists("/var/lock/LCK..ttyS0"));
  // Check flags
  QVERIFY(lck->isLocked());
  QVERIFY(!lck->isLockedByAnother());

  // Unlock port and check flags + LCK.. files cleanup
  lck->unlock();
  QVERIFY(!lck->isLocked());
  QVERIFY(!lck->isLockedByAnother());
  QVERIFY(!dir.exists("/tmp/LCK..ttyS0"));
  QVERIFY(!dir.exists("/var/lock/LCK..ttyS0"));
  close(fd);
  delete lck;

  // Check that unlock and cleanup works by destructor
  lck = new mdtPortLock;
  fd = lck->openLocked("/dev/ttyS0" , O_RDWR | O_NOCTTY | O_NONBLOCK);
  QVERIFY(fd >= 0);
  QVERIFY(dir.exists("/tmp/LCK..ttyS0"));
  QVERIFY(dir.exists("/var/lock/LCK..ttyS0"));
  delete lck;
  close(fd);
  QVERIFY(!dir.exists("/tmp/LCK..ttyS0"));
  QVERIFY(!dir.exists("/var/lock/LCK..ttyS0"));
}

void mdtPortLockTest::lockTest()
{
  mdtPortLock lck, lck2;
  int fd;

  // Initial states
  QVERIFY(!lck.isLocked());
  QVERIFY(!lck.isLockedByAnother());
  QVERIFY(!lck2.isLocked());
  QVERIFY(!lck2.isLockedByAnother());

  // Try to open a file
  fd = lck.openLocked("/dev/ttyS0" , O_RDWR | O_NONBLOCK);
  QVERIFY(fd >= 0);
  QVERIFY(lck.isLocked());
  QVERIFY(!lck.isLockedByAnother());
  // Try to reopen locked file with same object
  QVERIFY(lck.openLocked("/dev/ttyS0", O_RDWR | O_NONBLOCK) < 0);
  QVERIFY(lck.isLocked());
  QVERIFY(!lck.isLockedByAnother());

  // Try to open with second object
  QVERIFY(lck2.openLocked("/dev/ttyS0", O_RDWR | O_NONBLOCK) < 0);
  QVERIFY(!lck2.isLocked());
  QVERIFY(lck2.isLockedByAnother());

  // Free first lock
  lck.unlock();
  QVERIFY(!lck.isLocked());
  QVERIFY(!lck.isLockedByAnother());
  close(fd);

  // Try to open with second object
  fd = lck2.openLocked("/dev/ttyS0", O_RDWR | O_NONBLOCK);
  QVERIFY(fd >= 0);
  QVERIFY(lck2.isLocked());
  QVERIFY(!lck2.isLockedByAnother());
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtPortLockTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
