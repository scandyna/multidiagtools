#ifndef MDT_ERROR_TEST_H
#define MDT_ERROR_TEST_H

#include "mdtTest.h"

class mdtErrorTest : public mdtTest
{
 Q_OBJECT
 
 private slots:

  void simpleTest();
  
  void errorOutInitTest();
  
  void errorOutAddTest();
};

#endif // #ifndef MDT_ERROR_TEST_H