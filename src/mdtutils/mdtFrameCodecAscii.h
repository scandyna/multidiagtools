#ifndef MDT_FRAME_CODEC_ASCII_H
#define MDT_FRAME_CODEC_ASCII_H

#include "mdtFrameCodec.h"
#include <QString>
#include <QStringList>

class mdtFrameCodecAscii : public mdtFrameCodec
{
 public:

  mdtFrameCodecAscii();
  ~mdtFrameCodecAscii();

  /*! \brief Set the Eond Of Frame Sequence
   * 
   * When using mdtLibrary's communication classes,
   * the endOfFrame sequence is allready removed.
   * So, this setup is optional.
   * \sa mdtFrame
   * \sa mdtPort
   * \sa mdtPortManager
   */
  void setEofSeq(QByteArray &eofSeq);

  /*! \brief Remove whitespaces at begin and end of data frame
   */
  void trim();

  /*! \brief Set frame's data to upper case
   */
  void setCaseUpper();

 protected:

  // Remove the End Of Frame Sequence
  // Note: this method does nothing if EofSeq was not set
  bool removeEofSeq();

  QString pvAsciiData;
  QByteArray pvEofSeq;  // End Of Frame Squeunce
  QStringList pvNodes;
};

#endif  // #ifndef MDT_FRAME_CODEC_ASCII_H
