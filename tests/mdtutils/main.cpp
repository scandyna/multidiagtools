#include <QApplication>
#include "mdtBufferTest.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  int retVal;
  // Test objetcs
  mdtBufferTestRun bufferTest;
  
  // Run test classes
  retVal = QTest::qExec(&bufferTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }
  
  return 0;
}
