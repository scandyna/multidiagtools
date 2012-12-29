/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_FRAME_CODEC_SCPI_H
#define MDT_FRAME_CODEC_SCPI_H

#include "mdtFrameCodecAscii.h"
#include <QVariant>
#include <QString>

/*! \brief Helper defines for some mdtDevice subclasses
 */
#define MDT_FC_SCPI_UNKNOW 0        /*!< Initial or unknown query/response type */
#define MDT_FC_SCPI_VALUE  1        /*!< Device returns a value (int, double, date or string) */
#define MDT_FC_SCPI_IDN    2        /*!< IDN query or response */
#define MDT_FC_SCPI_ERR    3        /*!< ERRor query or response */
#define MDT_FC_SCPI_LAST_TYPE  127  /*!< Offset used by subclasses (f.ex. see mdtFrameCodecScpiU3606A) */

/*! \brief Decode SCPI data
 */
class mdtFrameCodecScpi : public mdtFrameCodecAscii
{
 public:

  /*! \brief Format of waveform data
   */
  enum waveform_format {
                        WORD, /*!< 16 bit data resolution */
                        BYTE, /*!< 8 bit data resolution */
                        ASCII /*!< ASCII float represented data */
                        };

  mdtFrameCodecScpi();
  virtual ~mdtFrameCodecScpi();

  /*! \brief Decode SCPI value(s)
   *
   * After decode is done, values are available
   *  with values().
   *
   * \param data String that contains one value, or many separated by sep.
   * \param sep Values separator.
   *
   * \return True if decode could be done.
   *
   * Errors are logged with the mdtError system.
   */
  bool decodeValues(const QByteArray &data, QString sep = ":");

  /*! \brief Decode the IDN answer
   *
   * When decode was done, the values
   * are available with values()
   */
  bool decodeIdn(const QByteArray &data);

  /*! \brief Decode error answer
   *
   * When decode was done successfully,
   *  error is available with values():
   *  - Item 0 is the error number
   *  - Item 1 is the error message string
   *  - After item 1 are device's specific messages (if exists)
   */
  bool decodeError(const QByteArray &data);

  /*! \brief Decode a IEEE block
   *
   * NOTE: not implemented yet !
   */
  bool decodeIEEEblock(const QByteArray &data, waveform_format format);

 protected:

  /*! \brief Decode a array of ASCII values
   *
   * \todo Not implemented yet
   */
  bool decodeIEEEdataAscii(const QByteArray &data);

  /*! \brief Decode a array of BYTE values
   *
   * \todo Not implemented yet
   */
  bool decodeIEEEdataByte(const QByteArray &data);

 private:

  // Invalid floating/limits values
  double pvInfinity;
  double pvNinfinity;
  double pvNan;
};

#endif  // #ifnef MDT_FRAME_CODEC_SCPI_H
