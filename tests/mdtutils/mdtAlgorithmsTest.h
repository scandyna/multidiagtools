#ifndef MDT_ALGORITHMS_TEST_H
#define MDT_ALGORITHMS_TEST_H

#include "mdtTest.h"

class mdtAlgorithmsTest : public mdtTest
{
 Q_OBJECT
 
 private slots:

  void extractNumericPartAtEndTest();
  void extractNumericPartAtEndTest_data();

  void stringWithNumericEndLessThanTest();
  void stringWithNumericEndLessThanTest_data();

  void sortStringListWithNumericEndTest();
  void sortStringListWithNumericEndTest_data();
};

#endif  // #ifndef MDT_ALGORITHMS_TEST_H
