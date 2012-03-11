#include <QApplication>
#include <QDebug>
#include "mdtAlgorithmsTest.h"
#include "mdtBufferTest.h"
#include "mdtFrameTest.h"
#include "mdtErrorOut.h"
#include "mdtErrorTest.h"
#include "mdtPortTest.h"
#include "mdtFrameCodecTest.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  int retVal = 0;

#ifdef Q_OS_UNIX
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

  // Init error system
  if(!mdtErrorOut::init("mdtutilstest.log")){
    qDebug() << "main(): unable to init the error system";
    return 1;
  }
  mdtErrorOut::setDialogLevelsMask(0);
  // Test objetcs
  mdtAlgorithmsTest algorithmsTest;
  mdtBufferTestRun bufferTest;
  mdtFrameTest frameTest;
  mdtErrorTest errorTest;
  mdtPortTest portTest;
  mdtFrameCodecTest frameCodecTest;

  // Run test classes
  retVal = 0;
  retVal = QTest::qExec(&algorithmsTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }
  retVal = QTest::qExec(&bufferTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }
  retVal = QTest::qExec(&frameTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }
  retVal = QTest::qExec(&errorTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }
  retVal = QTest::qExec(&frameCodecTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }
  // Device dependent tests
  retVal = QTest::qExec(&portTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }

  // Enable this warning if a test is temporary diseabled
  //qDebug() << "*!!* WWW Some tests are diseabled !!!!! WWW *!!*";

  // Free the error system
  mdtErrorOut::destroy();
  return 0;
}
