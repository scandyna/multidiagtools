#include <QApplication>
#include <QDebug>
#include "mdtErrorOut.h"
#include "mdtSerialPortTest.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  int retVal = 0;
  // Init error system
  if(!mdtErrorOut::init("mdtserialporttest.log")){
    qDebug() << "main(): unable to init the error system";
    return 1;
  }
  mdtErrorOut::setDialogLevelsMask(0);
  // Test objetcs
  mdtSerialPortTest serialPortTest;
  
  retVal = QTest::qExec(&serialPortTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }

  // Free the error system
  mdtErrorOut::destroy();
  return 1;
}
