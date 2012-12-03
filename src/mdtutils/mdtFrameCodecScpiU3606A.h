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
#ifndef MDT_FRAME_CODEC_SCPI_U3606A_H
#define MDT_FRAME_CODEC_SCPI_U3606A_H

#include "mdtFrameCodecScpi.h"

/*! \brief Helper defines for some mdtDevice() subclasses
 */
#define MDT_FC_SCPI_CONF  (MDT_FC_SCPI_LAST_TYPE + 1)   /*!< CONFigure query or response type */

/*! \brief Decode Agilent U3606A SCPI data
 */
class mdtFrameCodecScpiU3606A : public mdtFrameCodecScpi
{
 public:

  /*! \brief Measure type
   */
  enum measure_type_t {
    MT_UNKNOW,          /*!< Unknow measure type */
    MT_VOLTAGE_DC,      /*!< Measure type DC voltage */
    MT_VOLTAGE_AC,      /*!< Measure type AC voltage */
    MT_CURRENT_DC,      /*!< Measure type DC current */
    MT_CURRENT_AC       /*!< Measure type AC current */
  };

  mdtFrameCodecScpiU3606A();

  /*! \brief Decode the CONFigure and FUNCtion answer
   *
   */
  bool decodeConfFunc(QByteArray &frame);

  /*! \brief Decode a signle value in double floating format
   *
   * This method returns a QVariant.
   * QVariant's internal data is converted to double.
   * If value is not valid, the QVariant is cleared,
   * so it is possible to check this with QVariant::isValid().
   * When decode was done, the values
   * are available with values()
   */
  QVariant decodeSingleValueDouble(QByteArray &frame);

  /*! \brief Get the measure type
   */
  measure_type_t measureType();

  /*! \brief Get the measure range
   */
  QVariant range();

  /*! \brief Get the measure resolution
   */
  QVariant resolution();

 private:

  // Decode values for a node
  // Each value will be stored in pvValues
  bool decodeNodeValues(QString data);

  // Convert value to double with U3606A limits check (OL, -OL)
  QVariant convertToDouble(QVariant value);

  measure_type_t pvMeasureType;
  QVariant pvRange;
  QVariant pvResolution;
};

#endif  // #ifndef MDT_FRAME_CODEC_SCPI_U3606A_H
