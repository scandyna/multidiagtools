
#include "mdtSerialPortTest.h"
#include "mdtSerialPortInterfaceManager.h"

#include <QTest>

void mdtSerialPortTest::essais()
{
  mdtSerialPortInterfaceManager m;
  
  m.scan();
}
