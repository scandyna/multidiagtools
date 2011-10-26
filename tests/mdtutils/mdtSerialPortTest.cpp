
#include "mdtSerialPortTest.h"
#include "mdtSerialPortManager.h"
#include "mdtSerialPort.h"
#include "mdtSerialPortConfig.h"

#include <QTest>

void mdtSerialPortTest::essais()
{
  mdtSerialPortManager m;
  mdtSerialPort sp;
  mdtSerialPortConfig cfg;
  
  m.setSerialPortObj(&sp);
  m.scan();
  
  cfg.setInterface("/dev/ttyS0");
  cfg.setBaudRate(9600);
  cfg.setDataBitsCount(8);
  cfg.setStopBitsCount(1);
  cfg.setParity(mdtSerialPortConfig::NoParity);
  cfg.enableFlowCtlXonXoff();
  cfg.enableFlowCtlRtsCts();
  sp.setConfig(cfg);
}
