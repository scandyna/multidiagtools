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
    Range750,         /*!< 750 range */
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
   * Will connect to the instrument and send a device clear (USBTMC level clear).
   *  As recommanded in programmers guide, this function will also wait 2 seconds
   *  once connected before returning.
   *
   * \param serialNumber Device serial number. Will be ignored if empty
   * \param alias Alias to give. Will be ignored if empty
   */
  bool connectToDevice(const QString & serialNumber, const QString & alias)
  {
    if(!mdtDeviceScpi::connectToDevice(0x0957, 0x4d18, serialNumber, alias)){
      return false;
    }
    wait(2000);
    return true;
  }

  /*! \brief Connect to device
   *
   * Will connect to the instrument and send a device clear (USBTMC level clear).
   *  As recommanded in programmers guide, this function will also wait 2 seconds
   *  once connected before returning.
   *
   * \param alias Alias to give. Will be ignored if empty
   */
  bool connectToDevice(const QString & alias)
  {
    if(!mdtDeviceScpi::connectToDevice(0x0957, 0x4d18, QString(), alias)){
      return false;
    }
    wait(2000);
    return true;
  }

  /*! \brief Reset device
   *
   * \param rst If true, the *RST command will be sent to device,
   *            which place it to factory default state.
   * \param cls If true, the *CLS command will be sent to device,
   *           which clears event registers and error queue.
   */
  bool reset(bool rst, bool cls);

  /*! \brief Setup DC voltage measure
   *
   * \param range If range is supported by instrument, it will be set,
   *               else AUTO range is used.
   * \param resolution Resolution to use
   */
  bool setupVoltageDcMeasure(Range_t range, Resolution_t resolution);

  /*! \brief Setup AC voltage measure
   *
   * \param range If range is supported by instrument, it will be set,
   *               else AUTO range is used.
   * \param resolution Resolution to use
   */
  bool setupVoltageAcMeasure(Range_t range, Resolution_t resolution);

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
   * \sa setupVoltageDcMeasure()
   * \sa setupVoltageAcMeasure()
   * \sa setupResistanceMeasure()
   * \sa setupLowResistanceMeasure()
   *
   * \param timeout Timeout [ms]. If -2 is given, defaultTimeout will be used (see mdtDeviceScpi::defaultTimeout() ).
   */
  mdtValueDouble getMeasureValue(int timeout = -2)
  {
    return getValueDouble("READ?\n", timeout);
  }

  /*! \brief Start continuous measurement
   *
   * Internally, this will turn instrument to free run mode
   *  using INITiate:CONTinuous command.
   *  Then, FETCh? query is sent.
   *  Once instrument reurned a value,
   *  measureValueReceived() signal is emitted,
   *  and FETCh? query is sent again after given interval.
   *
   * Note that instrument must be configured before
   *  starting continuous measurement.
   *
   * \sa setupVoltageDcMeasure()
   * \sa setupVoltageAcMeasure()
   * \sa setupResistanceMeasure()
   * \sa setupLowResistanceMeasure()
   *
   * \param internal Interval before requery instrument [ms]
   * \pre interval must be >= 0
   */
  void startContinuousMeasurement(int interval);

  /*! \brief Stop continuous measurement
   */
  void stopContinuousMeasurement();

  /*! \brief Set source output state (On/Off)
   *
   * Refers to OUTPut[:STATe] in programmers reference guide.
   */
  bool setSourceOutputState(bool state);

  /*! \brief Get source output state (On/Off)
   *
   * Refers to OUTPut[:STATe]? in programmers reference guide.
   *
   * \return The state of source output.
   *         On error, a null value is returned and lastError() contains the error.
   */
  mdtValueBool getSourceOutputState();

  /*! \brief Set source range for constant voltage usage
   *
   * Refers to SOURce:VOLTage:RANGe in programmers reference guide.
   * Note: source range can be changed only whenn output is OFF.
   *
   * \sa setOutputState()
   */
  bool setSourceVoltageRange(SourceRange_t range);

  /*! \brief Set source range for constant current usage
   *
   * Refers to SOURce:CURRent:RANGe in programmers reference guide.
   * Note: source range can ba changed only whenn output is OFF.
   *
   * \sa setOutputState()
   */
  bool setSourceCurrentRange(SourceRange_t range);

  /*! \brief Set source voltage limit
   *
   * This voltage limit is used for constant current source mode.
   * Refers to [SOURce:]VOLTage:LIMit in programmers reference guide.
   */
  bool setSourceVoltageLimit(double v);

  /*! \brief Get source voltage limit
   *
   * This voltage limit is used for constant current source mode.
   * Refers to [SOURce:]VOLTage:LIMit? in programmers reference guide.
   */
  mdtValueDouble getSourceVoltageLimit()
  {
    return getValueDouble("SOUR:VOLT:LIM?\n");
  }

  /*! \brief Set source current limit
   *
   * Current limit is used constant voltage source mode.
   * Refers to [SOURce:]CURRent:LIMit in programmers reference guide.
   */
  bool setSourceCurrentLimit(double x);

  /*! \brief Get source current limit
   *
   * This current limit is used for constant voltage source mode.
   * Refers to [SOURce:]CURRent:LIMit? in programmers reference guide.
   */
  mdtValueDouble getSourceCurrentLimit()
  {
    return getValueDouble("SOUR:CURR:LIM?\n");
  }

  /*! \brief Set source over voltage protection (OVP)
   *
   * OVP is used for constant current source mode.
   * Refers to [SOURce:]VOLTage:PROTection in programmers reference guide.
   *
   * \param ovp OVP value. Minimum value is 0.
   *            Maximum value depends on source range:
   *            for S1 (30V/1A) the max is 33V and
   *            for S2 (8V/3A) the max is 8.8V.
   * \pre ovp must be >= 0 and <= 33
   */
  bool setSourceVoltageProtection(double ovp);

  /*! \brief Get source over voltage protection (OVP)
   *
   * OVP is used for constant current source mode.
   * Refers to [SOURce:]VOLTage:PROTection? in programmers reference guide.
   */
  mdtValueDouble getSourceVoltageProtection()
  {
    return getValueDouble("SOUR:VOLT:PROT?\n");
  }

  /*! \brief Set source over current protection (OCP)
   *
   * OCP is used for constant voltage source mode.
   * Refers to [SOURce:]CURRent:PROTection in programmers reference guide.
   *
   * \param ocp OCP value. Minimum value is 0.
   *            Maximum value depends on source range:
   *            for S1 (30V/1A) the max is 1.1A and
   *            for S2 (8V/3A) the max is 3.3A.
   * \pre ocp must be >= 0 and <= 3.3
   */
  bool setSourceCurrentProtection(double ocp);

  /*! \brief Get source over current protection (OCP)
   *
   * OCP is used for constant voltage source mode.
   * Refers to [SOURce:]CURRent:PROTection? in programmers reference guide.
   */
  mdtValueDouble getSourceCurrentProtection()
  {
    return getValueDouble("SOUR:CURR:PROT?\n");
  }

  /*! \brief Set source voltage
   *
   * Refers to [SOURce:]VOLTage[:LEVel][:IMMediate][:AMPLitude] in programmers reference guide.
   */
  bool setSourceVoltage(double x);

  /*! \brief Get configured source voltage
   *
   * Refers to [SOURce:]VOLTage[:LEVel][:IMMediate][:AMPLitude]? in programmers reference guide.
   * Note: to get the real source voltage, use getSourceSenseVoltage()
   */
  mdtValueDouble getSourceVoltage()
  {
    return getValueDouble("SOUR:VOLT?\n");
  }

  /*! \brief Get source sense voltage
   *
   * Refers to [SOURce:]SENSe:VOLTage[:LEVel]? in programmers reference guide.
   */
  mdtValueDouble getSourceSenseVoltage()
  {
    return getValueDouble("SENS:VOLT?\n");
  }

  /*! \brief Set source current
   *
   * Refers to [SOURce:]CURRent[:LEVel][:IMMediate][:AMPLitude] in programmers reference guide.
   */
  bool setSourceCurrent(double x);

  /*! \brief Get configured source current
   *
   * Refers to [SOURce:]CURRent[:LEVel][:IMMediate][:AMPLitude]? in programmers reference guide.
   * Note: to get the real source current, use getSourceSenseCurrent()
   */
  mdtValueDouble getSourceCurrent()
  {
    return getValueDouble("SOUR:CURR?\n");
  }

  /*! \brief Get source sense current
   *
   * Refers to [SOURce:]SENSe:CURRent[:LEVel]? in programmers reference guide.
   */
  mdtValueDouble getSourceSenseCurrent()
  {
    return getValueDouble("SENS:CURR?\n");
  }

  /*! \brief Setup source as constant current source
   *
   * Will set range reagrding given current, voltage limit and OVP.
   * Note: during setup, source output will be disabled. After setup, it must be explicitly re-enabled.
   *
   * \param current Constant current value [A]
   * \param voltageLimit Voltage limit [V]. If null, it will be ignored. \sa setSourceVoltageLimit()
   * \param ovp OVP voltage [V]. If null, it will be ignored. \sa setSourceVoltageProtection()
   * \pre current must not be null
   */
  bool setupSourceCurrent(const mdtValueDouble & current, const mdtValueDouble & voltageLimit = mdtValueDouble(), const mdtValueDouble & ovp = mdtValueDouble());

  /*! \brief Setup source as constant voltage source
   *
   * Will set range regarding given voltage, current limit and OCP.
   * Note: during setup, source output will be disabled. After setup, it must be explicitly re-enabled.
   *
   * \param voltage Constant voltage value [V]
   * \param currentLimit Current limit [A]. If null, it will be ignored. \sa setSourceCurrentLimit()
   * \param ocp OCP current [A]. If null, it will be ignored. \sa setSourceCurrentProtection()
   * \pre voltage must not be null
   */
  bool setupSourceVoltage(const mdtValueDouble & voltage, const mdtValueDouble & currentLimit = mdtValueDouble(), const mdtValueDouble & ocp = mdtValueDouble());

 signals:

  /*! \brief Emitted when a measure value was received from instrument
   */
  void measureValueReceived(const mdtValueDouble & x);

 private slots:

  /*! \brief Send FETCh query to device
   *
   * Will send the FETCh query to device.
   *  Once device returned data, measureValueReceived() signal is emitted.
   */
  void sendFetchQuery();

 private:

  Q_DISABLE_COPY(mdtDeviceU3606A);

  // Flags used for continous measurement
  bool pvContinousMeasurementActive;
  int  pvContinousMeasurementIntervall;
};

#endif  // #ifndef MDT_DEVICE_U3606A_H
