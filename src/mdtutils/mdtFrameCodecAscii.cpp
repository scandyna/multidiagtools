
#include "mdtFrameCodecAscii.h"
#include "mdtError.h"
#include <QString>

mdtFrameCodecAscii::mdtFrameCodecAscii()
{
  pvEofSeq = "";
}

mdtFrameCodecAscii::~mdtFrameCodecAscii()
{
}

void mdtFrameCodecAscii::setEofSeq(QByteArray &eofSeq)
{
  QString str;

  str = eofSeq;
  // Remove chars that are considered as whitespace (QChar::isSpace())
  // else, the trim() function will remove it. See QString::trimmed() for details
  str.remove('\t');
  str.remove('\n');
  str.remove('\v');
  str.remove('\f');

  pvEofSeq = str.toAscii();
}

void mdtFrameCodecAscii::trim()
{
  pvAsciiData = pvAsciiData.trimmed();
}

void mdtFrameCodecAscii::setCaseUpper()
{
  pvAsciiData = pvAsciiData.toUpper();
}

bool mdtFrameCodecAscii::removeEofSeq()
{
  // If EofSeq was not set, simply return
  if(pvEofSeq.size() < 1){
    return true;
  }
  // Check data length
  if(pvAsciiData.size() <= pvEofSeq.size()){
    mdtError e(MDT_FRAME_DECODE_ERROR, "Frame is smaller than End Of Frame Sequence" , mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtFrameCodecScpi");
    e.commit();
    return false;
  }
  // Remove the end of frame sequence
  pvAsciiData.remove(pvAsciiData.size()-pvEofSeq.size(), pvEofSeq.size());

  return true;
}
