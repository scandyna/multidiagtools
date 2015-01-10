/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_CODEC_SCPI_H
#define MDT_CODEC_SCPI_H

#include "mdtValue.h"
#include "mdtError.h"
#include <QByteArray>
#include <QString>
#include <QVector>

/*! \brief *IDN? response
 *
 * See SCPI 1999.0 , volume 1, section 4.1.3.6
 */
struct mdtScpiIdnResponse
{
  /*! \brief Manufacturer (field 1)
   */
  QString manufacturer;

  /*! \brief Model (field 2)
   */
  QString model;

  /*! \brief Serial (field 3)
   */
  QString serial;

  /*! \brief Firmware level (field 4)
   */
  QString firmwareLevel;
};

/*! \brief \<function\> \<parameters\> response data
 *
 * See SCPI 1999.0 , volume 2, section 3
 */
struct mdtScpiFunctionFarameter
{
  /*! \brief Fundamental measurement command
   *
   * See SCPI 1999.0 , volume 2, section 3.7
   */
  enum class FundamentalMeasurementCommand_t
  {
    Unknown,      /*!< Unknown measurement command */
    Voltage,      /*!< Voltage characteristics of the signal */
    Current,      /*!< Current characteristics of the signal */
    Power,        /*!< Power characteristics of the signal */
    Resistance,   /*!< Resistance measurement */
    Fresistance,  /*!< 4 wire resistance measurement */
    Temperature   /*!< Temperature measurement */
  };

  /*! \brief Simple measurement command
   *
   * See SCPI 1999.0 , volume 2, section 3.8
   */
  enum class SimpleMeasurementCommand_t
  {
    Unknown,        /*!< Unknown measurement command */
    Ac,             /*!< AC RMS characteristics of the signal */
    Dc,             /*!< DC level of the signal */
    Frequency,      /*!< Frequency of the signal */
    FrequencyBurst, /*!< Frequency of an ON-OFF modulated signal during its ON period */
    FrequencyPrf,   /*!< Repetition frequency of the ON-OFF modulation of a signal that is periodically switched ON and OFF */
    Period,         /*!< Period (1/f) of the signal */
    Phase           /*!< Measure phase of the signal */
  };

  /*! \brief Extract fundamental measurement command part from \<function\> part
   */
  FundamentalMeasurementCommand_t fundamentalMeasurementCommand() const;

  /*! \brief Extract simple measurement command part from \<function\> part
   */
  SimpleMeasurementCommand_t simpleMeasurementCommand() const;

  /*! \brief \<function\> part as returned by device
   */
  QByteArray functionRaw;

  /*! \brief \<parameters\> part as returned from device
   */
  QByteArray parametersRaw;

  /*! \brief \<parameters\> part that could be converted to double
   */
  QVector<double> parametersDouble;
};

/*! \brief SCPI codec
 */
class mdtCodecScpi
{
 public:

  /*! \brief Constructor
   */
  mdtCodecScpi() {}

  /*! \brief Decode *IDN? response
   */
  mdtScpiIdnResponse decodeIdn(const QByteArray & data);

  /*! \brief Decode a boolean value
   *
   * Will interpret 1 as true, 0 as false
   */
  mdtValueBool decodeValueBool(const QByteArray & data);

  /*! \brief Decode a integer value
   *
   * See SCPI 1999.0 , volume 3, section 3.4.4, 3.4.5 and 3.4.6
   */
  mdtValueInt decodeValueInt(const QByteArray & data);

  /*! \brief Decode a decimal value
   *
   * See SCPI 1999.0 , volume 3, section 3.4.1, 3.4.2 3.4.4, 3.4.5 and 3.4.6
   *  Note: this function assumes decimal format, not octal and not hexadecimal.
   *
   * If data contains a value <= -9.9e37 or >= 9.9e37, returned value will have OL flag set,
   *  and also contains a infinity value.
   *  If data contains a value == 9.91e37, returned value will be NaN
   *  See SCPI 1999.0 , volume 1, section 7.2.1.4 and 7.2.1.5 for details.
   */
  mdtValueDouble decodeValueDouble(const QByteArray & data);

  /*! \brief Decode device error
   *
   * See SCPI 1999.0 , volume 2, section 21.8
   *  Note: this function supports only 1 error
   *
   * \param data data returned from device
   * \param errorText Text to set in error. If empty, a default text will be set.
   * \return Error that contains device error as system error. If parsing failed, a empty error is returned (error level will be NoError)
   */
  mdtError decodeDeviceError(const QByteArray & data, const QString & errorText = QString());

  /*! \brief Decode \<function\> \<parameters\> response
   *
   * See SCPI 1999.0 , volume 2, section 3
   */
  mdtScpiFunctionFarameter decodeFunctionParameters(const QByteArray & data);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 protected:

  mdtError pvLastError;

 private:

  Q_DISABLE_COPY(mdtCodecScpi);

};

#endif // #ifndef MDT_CODEC_SCPI_H
