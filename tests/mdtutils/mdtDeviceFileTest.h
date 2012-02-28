#ifndef MDT_DEVICE_FILE_TEST_H
#define MDT_DEVICE_FILE_TEST_H

#include "mdtTest.h"

class mdtDeviceFileTest : public mdtTest
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

  // Check that write and read works same time
  void writeReadTest();

  // Check that recovery works whenn a frame pool was empty for some time
  void emptyQueueRecoveryTest();
  
  void essais();
};

#endif  // #ifndef MDT_DEVICE_FILE_TEST_H
