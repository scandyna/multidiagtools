#ifndef _MDTFRAMECODECASCII_H
#define _MDTFRAMECODECASCII_H


#include "mdtFrameCodec.h"

class mdtFrameCodecAscii : public mdtFrameCodec {
  protected:
    QString pvAsciiData;


  public:
     mdtFrameCodecAscii();

     ~mdtFrameCodecAscii();

    void trim();

    void setCaseUpper();

};
#endif
