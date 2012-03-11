#ifndef MDT_FRAME_CODEC_TEST_H
#define MDT_FRAME_CODEC_TEST_H


#include "mdtTest.h"

class mdtFrameCodecTest : public mdtTest
{
 Q_OBJECT
 
 private slots:

  // Check SCPI
  void mdtFrameCodecScpiTest();

  // Check Agilent U3606A SCPI
  void mdtFrameCodecScpiTestU3606ATest();
};

#endif  // #ifndef MDT_FRAME_CODEC_TEST_H
