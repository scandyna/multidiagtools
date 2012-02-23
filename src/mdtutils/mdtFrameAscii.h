#ifndef MDT_FRAME_ASCII
#define MDT_FRAME_ASCII

#include "mdtFrame.h"

class mdtFrameAscii : public mdtFrame
{
 public:

  mdtFrameAscii();
  ~mdtFrameAscii();

  /*! \brief Set the end of frame sequence
   * Note: if eof is empty, the null char ('\0') is considered
   */
  void setEofSeq(const QByteArray &eof);

  /*! \brief Get the length of the end of frame sequence
   */
  int eofSeqLen();

  int putData(char *data, int maxLen);

 private:

  QByteArray pvEofSeq;
  char pvEofChar;
  bool pvEofIsChar;
};

#endif  // #ifndef MDT_FRAME_ASCII
