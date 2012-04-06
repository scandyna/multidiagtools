#include <QApplication>
#include <QDebug>
#include <QTextCodec>
#include "mdtErrorOut.h"
#include "mdtSerialPortTest.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  int retVal = 0;

#ifdef Q_OS_UNIX
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

  // Init error system
  if(!mdtErrorOut::init("mdtserialporttest.log")){
    qDebug() << "main(): unable to init the error system";
    return 1;
  }
  mdtErrorOut::setDialogLevelsMask(0);
  mdtErrorOut::setLogLevelsMask(mdtError::Info | mdtError::Warning | mdtError::Error);
  // Test objetcs
  mdtSerialPortTest serialPortTest;

  retVal = QTest::qExec(&serialPortTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }

  // Free the error system
  mdtErrorOut::destroy();
  return 0;
}
