/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include <QApplication>
#include <QDebug>
#include "mdtAlgorithmsTest.h"
#include "mdtBufferTest.h"
#include "mdtFrameTest.h"
#include "mdtErrorOut.h"
#include "mdtErrorTest.h"
#include "mdtFrameCodecTest.h"
#include "mdtFileTest.h"
#include "mdtUicNumberTest.h"
#include "mdtDatabaseTest.h"

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
  mdtFrameCodecTest frameCodecTest;
  mdtFileTest fileTest;
  mdtUicNumberTest uicNumberTest;
  mdtDatabaseTest databaseTest;

  // Run test classes
  retVal = 0;

  retVal = QTest::qExec(&databaseTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }
  retVal = QTest::qExec(&uicNumberTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }
  retVal = QTest::qExec(&fileTest, argc, argv);
  if(retVal!=0){
    return retVal;
  }
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

  // Enable this warning if a test is temporary diseabled
  ///qDebug() << "*!!* WWW Some tests are diseabled !!!!! WWW *!!*";

  // Free the error system
  mdtErrorOut::destroy();
  return 0;
}
