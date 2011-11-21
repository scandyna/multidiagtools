#ifndef MDT_SERIAL_PORT_TEST_H
#define MDT_SERIAL_PORT_TEST_H

#include "mdtTest.h"

class mdtSerialPortTest : public mdtTest
{
 Q_OBJECT
 
 private slots:
  
  void essais();
  
  // Test the mdtSerialPortConfig class
  void mdtSerialPortConfigTest();
  void mdtSerialPortConfigTest_data();
  
  // Test the control signals (modem lines)
  void mdtSerialPortCtlSignalsTest();
  
signals:
  void testSignal(bool on);
};

#endif  // #ifndef MDT_SERIAL_PORT_TEST_H
