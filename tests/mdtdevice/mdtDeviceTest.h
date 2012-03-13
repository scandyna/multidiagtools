#ifndef _MDTDEVICETEST_H
#define _MDTDEVICETEST_H

#include "mdtTest.h"

class mdtDeviceTest : public mdtTest
{
 Q_OBJECT

 private slots:

#ifdef Q_OS_UNIX
  // Check U3606A device
  void deviceU3606Atest();
#endif

};
#endif
