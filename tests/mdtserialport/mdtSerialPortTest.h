#ifndef MDT_SERIAL_PORT_TEST_H
#define MDT_SERIAL_PORT_TEST_H

#include "mdtTest.h"

class mdtSerialPortTest : public mdtTest
{
 Q_OBJECT
 
 private slots:
  
  void essais();
  
  // Serial port manager tests
  void mdtSerialPortManagerTest();
  
  // Test the mdtSerialPortConfig class
  void mdtSerialPortConfigTest();
  void mdtSerialPortConfigTest_data();

  // Test start and stop
  void mdtSerialPortStartStopTest();

  // Test the control signals (modem lines)
  void mdtSerialPortCtlSignalsTest();
  
  // Test the data transmission
  void mdtSerialPortTxRxTest();
  
signals:
  void testSignal(bool on);
};

#endif  // #ifndef MDT_SERIAL_PORT_TEST_H
