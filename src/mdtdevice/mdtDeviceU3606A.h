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
#ifndef MDT_DEVICE_U3606A_H
#define MDT_DEVICE_U3606A_H

#include "mdtDeviceScpi.h"
#include "mdtValue.h"

/*! \brief Representation of a Agilent U3606A
 */
class mdtDeviceU3606A : public mdtDeviceScpi
{
 Q_OBJECT

 public:

  /*! \brief Measure type
   */
  enum class MeasureType_t
  {
    Unknown,        /*!< Unknown measure type */
    VoltageDc,      /*!< Measure type DC voltage */
    VoltageAc,      /*!< Measure type AC voltage */
    CurrentDc,      /*!< Measure type DC current */
    CurrentAc,      /*!< Measure type AC current */
    Resistance,     /*!< Measure type resistance */
    Continuity,     /*!< Measure type continuity */
    Lresistance,    /*!< Measure type low value reistance */
    Capacitance,    /*!< Measure type capacitance */
    Diode,          /*!< Measure type diode */
    Frequency,      /*!< Measure type frequency */
    Pwidth,         /*!< Measure type pulse width */
    Dcycle          /*!< Measure type duty cycle */
  };

  /*! \brief Range for measure
   */
  enum class Range_t
  {
    Auto = -10,       /*!< AUTO range */
    Min,              /*!< MIN range */
    Max,              /*!< MAX range */
    Range20m = 1,     /*!< 20 milli range */
    Range100m,        /*!< 100 milli range */
    Range1,           /*!< 1 range */
    Range10,          /*!< 10 range */
    Range100,         /*!< 100 range */
    Range1k,          /*!< 1k range */
    Range10k,         /*!< 10k range */
    Range100k,        /*!< 100k range */
    Range1M,          /*!< 1M range */
    Range10M,         /*!< 10M range */
    Range100M         /*!< 100M range */
  };

  /*! \brief Resolution
   */
  enum class Resolution_t
  {
    Min,    /*!< 5 1/2 digit */
    Max     /*!< 4 1/2 digit */
  };

  /*! \brief Range of source subsystem
   */
  enum class SourceRange_t
  {
    S1_30V1A,  /*!< Range S1: 30V / 1A */
    S2_8V3A    /*!< Range S2: 8V / 3A */
  };

  /*! \brief Constructor
   *
   * Will create the needed codec, I/Os and configure internal port manager .
   */
  mdtDeviceU3606A(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtDeviceU3606A();

  /*! \brief Connect to device
   *
   * \param serialNumber Device serial number. Will be ignored if empty
   * \param alias Alias to give. Will be ignored if empty
   */
  bool connectToDevice(const QString & serialNumber, const QString & alias = QString())
  {
    return mdtDeviceScpi::connectToDevice(0x0957, 0x4d18, serialNumber, alias);
  }

  /*! \brief Setup DC voltage measure
   *
   * \param range If range is supported by instrument, it will be set,
   *               else AUTO range is used.
   * \param resolution Resolution to use
   */
  bool setupVoltageDcMeasure(Range_t range, Resolution_t resolution);

  /*! \brief Setup resistance measure
   *
   * \param range If range is supported by instrument, it will be set,
   *               else AUTO range is used.
   * \param resolution Resolution to use
   */
  bool setupResistanceMeasure(Range_t range, Resolution_t resolution);

  /*! \brief Setup low resistance measure (4 wires)
   *
   * \param range If range is supported by instrument, it will be set,
   *               else AUTO range is used.
   * \param resolution Resolution to use
   */
  bool setupLowResistanceMeasure(Range_t range, Resolution_t resolution);

  /*! \brief Get the measure configuration
   *
   * Will get the device current measure configuration
   *  by sending the CONF? query .
   *
   * By error, unknown measure type is returned .
   */
  MeasureType_t getMeasureConfiguration();

  /*! \brief Get measure value
   *
   * Will submit the READ? query to device.
   *  Don't forget to setup measure before.
   *
   * \param timeout Timeout [ms]
   */
  mdtValueDouble getMeasureValue(int timeout = 30000);

  /*! \brief Set source output state (On/Off)
   */
  bool setOutputState(bool state);

  /*! \brief Set source range
   *
   * Note: source range can ba changed only whenn output is OFF.
   *
   * \sa setOutputState()
   */
  bool setSourceRange(SourceRange_t range);

  /*! \brief Set source voltage limit
   *
   * This voltage limit is used for constant current source mode.
   * Refers to [SOURce:]VOLTage:LIMit in programmers reference guide.
   */
  bool setSourceVoltageLimit(double v);

  /*! \brief Set source current
   *
   * Will setup source for constant current mode,
   *  with given current.
   *
   * Refers to [SOURce:]CURRent[:LEVel][:IMMediate][:AMPLitude] in programmers reference guide.
   */
  bool setSourceCurrent(double x);

 private:

  Q_DISABLE_COPY(mdtDeviceU3606A);
};

#endif  // #ifndef MDT_DEVICE_U3606A_H
