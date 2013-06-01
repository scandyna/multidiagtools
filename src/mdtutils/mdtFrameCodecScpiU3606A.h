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
    MT_CURRENT_AC,      /*!< Measure type AC current */
    MT_RESISTANCE,      /*!< Measure type resistance */
    MT_CONTINUITY,      /*!< Measure type continuity */
    MT_LRESISTANCE,     /*!< Measure type low value reistance */
    MT_CAPACITANCE,     /*!< Measure type capacitance */
    MT_DIODE,           /*!< Measure type diode */
    MT_FREQUENCY,       /*!< Measure type frequency */
    MT_PWIDTH,          /*!< Measure type pulse width */
    MT_DCYCLE           /*!< Measure type duty cycle */
  };

  mdtFrameCodecScpiU3606A();

  /*! \brief Decode CONFigure? reply
   */
  bool decodeConfigure(const QByteArray &data);

  /*! \brief Get the measure type
   */
  measure_type_t measureType() const;

  /*! \brief Get the measure range
   */
  mdtValue range() const;

  /*! \brief Get the measure resolution
   */
  mdtValue resolution() const;

 private:

  measure_type_t pvMeasureType;
  mdtValue pvRange;
  mdtValue pvResolution;
};

#endif  // #ifndef MDT_FRAME_CODEC_SCPI_U3606A_H
