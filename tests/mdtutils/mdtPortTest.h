#ifndef MDT_PORT_TEST_H
#define MDT_PORT_TEST_H

#include "mdtTest.h"
#include <QTcpServer>

class mdtPortTest : public mdtTest
{
 Q_OBJECT

 private slots:

  // Init
  void initTestCase();

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

  // Test TCP/IP socket
  void tcpSocketTest();

  // This is not a test. This method is called from tcpServer in tcpSocketTest()
  void tcpServerNewConnection();

 private:

  QTcpServer *pvTcpServer;
};

#endif  // #ifndef MDT_PORT_TEST_H
