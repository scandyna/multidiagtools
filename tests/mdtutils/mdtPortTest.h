#ifndef MDT_PORT_TEST_H
#define MDT_PORT_TEST_H

#include "mdtTest.h"

class mdtPortTest : public mdtTest
{
 Q_OBJECT

 private slots:

  // Test start and stop
  void startStopTest();

  // Check that write works
  void writeTest();
  void writeTest_data();

  // Check that read works
  void readTest();
  void readTest_data();

  // Check that recovery works whenn a frame pool was empty for some time
  void emptyQueueRecoveryTest();

  // Test port manager
  void portManagerTest();

#ifdef Q_OS_UNIX
  // Check USBTMC module (needs a device attached)
  void usbtmcPortTest();
#endif
};

#endif  // #ifndef MDT_PORT_TEST_H
