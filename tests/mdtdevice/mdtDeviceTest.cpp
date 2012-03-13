
#include "mdtDeviceTest.h"

#ifdef Q_OS_UNIX
 #include "linux/mdtDeviceU3606A.h"
#endif

#include <QString>
#include <QStringList>

#include <QDebug>

#ifdef Q_OS_UNIX
void mdtDeviceTest::deviceU3606Atest()
{
  mdtDeviceU3606A d;
  QStringList ports;
  
  qDebug() << "* A Agilent U3606A must be attached at USB port, else test will fail *";

  // Check that scan and open methods works
  ports = d.scan();
  QVERIFY(ports.size() > 0);
  QVERIFY(d.selectPort(ports, 0));

  d.essais();
}
#endif
