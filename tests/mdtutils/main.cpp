#include <QApplication>
#include <QDebug>
#include "mdtAlgorithmsTest.h"
#include "mdtBufferTest.h"
#include "mdtSerialPortTest.h"


int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  int retVal = 0;
  // Test objetcs
  mdtAlgorithmsTest algorithmsTest;
  mdtBufferTestRun bufferTest;
  mdtSerialPortTest serialPortTest;

  // Run test classes
  retVal = QTest::qExec(&algorithmsTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }
  //retVal = QTest::qExec(&bufferTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }
  retVal = QTest::qExec(&serialPortTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }

  // Enable this warning if a test is temporary diseabled
  qDebug() << "*!!* WWW Some tests are diseabled !!!!! WWW *!!*";

  return 1;
}
