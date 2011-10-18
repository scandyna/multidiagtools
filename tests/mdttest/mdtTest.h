#ifndef MDT_TEST_H
#define MDT_TEST_H

#include <QObject>
#include <QtTest/QtTest>
#include <iostream>
#include <string>
#include "config.h"

// QVERIFY variant macro
#define VERIFY(statement, description) mdtTest::verify((statement), #statement, description, __FILE__, __LINE__);

using namespace std;

class mdtTest : public QObject
{
 Q_OBJECT
 
 public:
   
  mdtTest();
  ~mdtTest();
  
  /*! \brief Verify a statement
   *
   */
  static bool verify(bool statement, const char *statementStr, const char *description, const char *file, int line);

};

#endif  // ifdef MDT_TEST_H
