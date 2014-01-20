#ifndef _MDTFRAMEASCII_H
#define _MDTFRAMEASCII_H


#include "mdtFrame.h"

class mdtFrameAscii : public mdtFrame {
  public:
    mdtFrameAscii();

    ~mdtFrameAscii();


  private:
    QByteArray pvEofSeq;

    char pvEofChar;

    bool pvEofIsChar;


  public:
    void setEofSeq(const QByteArray & eof);

    void putData(char data, int len);

};
#endif
