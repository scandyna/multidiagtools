#ifndef MDT_FRAME_CODEC_H
#define MDT_FRAME_CODEC_H

#include <QByteArray>
#include <QVariant>
#include <QList>

class mdtFrameCodec
{
 public:

  mdtFrameCodec();
  ~mdtFrameCodec();

  /*! \brief Get the decoded values
   */
  QList<QVariant> values();

 protected:

  // Decode IEEE block of data
  // NOTE: not implemented yet
  bool decodeIEEEdata(QByteArray data);

  QList<QVariant> pvValues;
};

#endif  // #ifndef MDT_FRAME_CODEC_H
