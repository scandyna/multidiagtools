
#include "mdtFrameAscii.h"
#include "mdtError.h"

mdtFrameAscii::mdtFrameAscii()
{
  // Assign EOT char as end sequence
  pvEofIsChar = true;
  pvEofChar = 0x04;
}

mdtFrameAscii::~mdtFrameAscii()
{
}

void mdtFrameAscii::setEofSeq(const QByteArray &eof)
{
  if(eof.size() < 1){
    pvEofIsChar = true;
    pvEofChar = '\0';
  }else if(eof.size() == 1){
    pvEofIsChar = true;
    pvEofChar = eof.at(0);
  }else{
    pvEofIsChar = false;
    pvEofSeq = eof;
  }
}

int mdtFrameAscii::eofSeqLen()
{
  if(pvEofIsChar){
    return 1;
  }
  return pvEofSeq.size();
}

int mdtFrameAscii::putData(char *data, int maxLen)
{
  if(pvEofIsChar){
    return putUntil(data, pvEofChar, maxLen);
  }
  return putUntil(data, pvEofSeq, maxLen);
}

