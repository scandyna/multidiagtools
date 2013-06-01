/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
///#include <QVariant>
#include <QString>

/*! \brief Decode SCPI data
 */
class mdtFrameCodecScpi : public mdtFrameCodecAscii
{
 public:

  /*! \brief Query type
   */
  enum query_type_t {
                      QT_UNKNOW = 0,        /*!< Initial or unknown query/response type */
                      QT_VALUE,             /*!< Device returns one or more value(s) (int, double, date or string) */
                      QT_SINGLE_VALUE_FLT,  /*!< Device returns a floating value */
                      QT_IDN,               /*!< IDN query or response */
                      QT_ERR,               /*!< ERRor query or response */
                      QT_CONF               /*!< CONFigure query or response type */
                    };

  /*! \brief Format of waveform data
   */
  enum waveform_format_t {
                        WORD,   /*!< 16 bit data resolution */
                        BYTE,   /*!< 8 bit data resolution */
                        ASCII   /*!< ASCII float represented data */
                        };

  mdtFrameCodecScpi();
  virtual ~mdtFrameCodecScpi();

  /*! \brief Decode SCPI value(s)
   *
   * After decode is done, values are available
   *  with values().
   *
   * Internally, mdtFrameCodecAscii::convertData() is used for conversion.
   *  Additionnaly, if decoded type is double, the validity is checked
   *  with checkFloatingValueValidity().
   *
   * \param data String that contains one value, or many separated by sep.
   * \param sep Values separator.
   *
   * \return True if decode could be done.
   *
   * Errors are logged with the mdtError system.
   */
  bool decodeValues(const QByteArray &data, QString sep = ":");

  /*! \brief Decode a signle value in double floating format
   *
   * This method returns a QVariant.
   *  QVariant's internal data is converted to double.
   *  If value is not valid, the QVariant is cleared,
   *  so it is possible to check this with QVariant::isValid().
   *
   * \return A mdtValue that contains data of type Double if decode and conversion to double could be done,
   *          else a invalid mdtValue.
   */
  ///QVariant decodeSingleValueDouble(const QByteArray &data);
  mdtValue decodeSingleValueDouble(const QByteArray &data);

  /*! \brief Check the validity of a floating value
   *
   * Check validity regarding SCPI-99, sections 7.2.1.4 (-infinity, +infinity) and 7.2.1.5 (NaN).
   *
   * \return A mdtValue that contains data of type Double. If x is NaN, a invalid mdtValue is returned.
   */
  ///QVariant checkFloatingValueValidity(double x) const;
  mdtValue checkFloatingValueValidity(double x) const;

  /*! \brief Decode reply of type function/parameter
   *
   * Typical query that returns \<function\> \<parameters\> form is CONFigure?.
   *  See SCPI-99 , chap. 3.
   *
   * After successfull decode, values() contains:
   *  - Index 0: Function as string
   *  - Index >1 (if available): parameters in converted form (double, int, ...).
   *               Note: if a parameter is a floating value, it's possible that it is invalid (+/-inf, Nan).
   */
  bool decodeFunctionParameters(const QByteArray &data);

  /*! \brief Decode the IDN answer
   *
   * When decode was done, the values
   * are available with values()
   */
  bool decodeIdn(const QByteArray &data);

  /*! \brief Decode the SCPI version returned by SYSTem:VERSion? query
   *
   * After successfull decode, values will be stored as follow:
   *  - Index 0: SCPI year as integer
   *  - Index 1: SCPI revision
   */
  bool decodeScpiVersion(const QByteArray &data);

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
  bool decodeIEEEblock(const QByteArray &data, waveform_format_t format);

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
