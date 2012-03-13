#include <QApplication>
#include <QDebug>
#include "mdtDeviceTest.h"
#include "mdtError.h"
#include "mdtErrorOut.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  int retVal = 0;

#ifdef Q_OS_UNIX
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

  // Init error system
  if(!mdtErrorOut::init("mdtdevicetest.log")){
    qDebug() << "main(): unable to init the error system";
    return 1;
  }
  mdtErrorOut::setDialogLevelsMask(0);
  // Test objetcs
  mdtDeviceTest deviceTest;

  // Run test classes
  retVal = 0;
  retVal = QTest::qExec(&deviceTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }

  // Enable this warning if a test is temporary diseabled
  //qDebug() << "*!!* WWW Some tests are diseabled !!!!! WWW *!!*";

  // Free the error system
  mdtErrorOut::destroy();
  return 0;
}
