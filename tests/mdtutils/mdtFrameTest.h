#ifndef MDT_FRAME_TEST_H
#define MDT_FRAME_TEST_H

#include "mdtTest.h"

class mdtFrameTest : public mdtTest
{
 Q_OBJECT
 
 private slots:

  // Verify that initial parameters are correct,
  // such as capacity, ...
  void initTest();

  // Verify putData() function
  void putDataTest();

  // Verify state and values by ASCII frames
  void asciiReceptionTest();
  void asciiReceptionIgnoreNullValuesTest();
  // Same. but the EOF is a string
  void asciiReceptionEofStrTest();
  void asciiReceptionEofStrIgnoreNullValuesTest();

  // Verify states and values by taking data
  void takeDataTest();

  // Check mdtFrameModbusTcp encoding
  void modbusTcpEncodeTest();

  // Check mdtFrameModbusTcp decoding
  void modbusTcpDecodeTest();
};
#endif // idnfef MDT_FRAME_TEST_H