#ifndef MDT_FRAME_CODEC_SCPI_H
#define MDT_FRAME_CODEC_SCPI_H

#include "mdtFrameCodecAscii.h"
#include <QList>
#include <QVariant>
#include <QString>

/*! \brief Decode SCPI data
 */
class mdtFrameCodecScpi : public mdtFrameCodecAscii
{
 public:

  mdtFrameCodecScpi();
  ~mdtFrameCodecScpi();

  /*! \brief Decode the IDN answer
   *
   * When decode was done, the values
   * are available with values()
   */
  bool decodeIdn(QByteArray &frame);

  /*! \brief Decode a signle value in integer format
   *
   * This method returns a QVariant.
   * QVariant's internal data is converted to int.
   * If value is not valid, the QVariant is cleared,
   * so it is possible to check this with QVariant::isValid().
   * When decode was done, the values
   * are available with values()
   */
  QVariant decodeSingleValueInt(QByteArray &frame);

  /*! \brief Decode error answer
   *
   * When decode was done, the values
   * are available with values()
   */
  bool decodeError(QByteArray &frame);

  /*! \brief Get the decoded values
   */
  QList<QVariant> values();
  
  /*! \brief Decode a IEEE block
   *
   * NOTE: not implemented yet !
   */
  bool decodeIEEEblock(QString data);
};

#endif  // #ifnef MDT_FRAME_CODEC_SCPI_H
