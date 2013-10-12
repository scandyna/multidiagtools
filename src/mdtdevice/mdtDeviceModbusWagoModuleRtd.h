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
#ifndef MDT_DEVICE_MODBUS_WAGO_MODULE_RTD_H
#define MDT_DEVICE_MODBUS_WAGO_MODULE_RTD_H

#include "mdtDeviceModbusWagoModule.h"
#include <QList>

class mdtDeviceModbusWago;

/*! \brief Helper class for RTD modules
 *
 * Current version implements following modules:
 *  - 750-464 2/4 AI RTD
 */
class mdtDeviceModbusWagoModuleRtd : public mdtDeviceModbusWagoModule
{
 public:

  /*! \brief PSRR mode
   */
  enum psrrMode_t {
                    Psrr50Hz ,    /*!< 50Hz */
                    Psrr60Hz ,    /*!< 60Hz */
                    Psrr5060Hz ,  /*!< 50/60Hz */
                    PsrrUnknown   /*!< Unknow/illegal */
                  };

  /*! \brief Sensor type
   */
  enum sensorType_t {
                      Unknown ,       /*!< Unknown (for reserved bits) */
                      Pt100 ,         /*!< Pt100 (IEC 751) */
                      Ni100 ,         /*!< Ni100 (DIN 43760) */
                      Pt1000 ,        /*!< Pt1000 (IEC 751) */
                      Pt500 ,         /*!< Pt500 (IEC 751) */
                      Pt200 ,         /*!< Pt200 (IEC 751) */
                      Ni1000 ,        /*!< Ni1000 (DIN 43760) */
                      Ni120 ,         /*!< Ni120 (Minco) */
                      Ni1000TK ,      /*!< Ni1000 (TK 5000) */
                      Potentiometer , /*!< Potentiometer */
                      R10R5000 ,      /*!< 10 to 5000 Ohm */
                      R10R1200        /*!< 10 to 1200 Ohm */
                    };

  /*! \brief Process calue representation
   */
  enum processValueRepresentation_t {
                                      C2 ,          /*!< Two's complement representation */
                                      AmountSign ,  /*!< Amount / sign representation */
                                      C2S5 ,        /*!< S5-FB250 with two's complement representation */
                                      AmountSignS5  /*!< S5-FB250 with amount / sign representation */
                                     };

  /*! \brief Constructor
   *
   * \param autoDeleteIos If true, internally created I/O's are deleted when this object is destroyed
   * \param device Pointer to device (used for register service access)
   * \pre device must be a valid pointer
   */
  mdtDeviceModbusWagoModuleRtd(bool autoDeleteIos, mdtDeviceModbusWago *device);

  /*! \brief Destructor
   *
   * If autoDeleteIos is true, internal I/O's are deleted
   */
  ~mdtDeviceModbusWagoModuleRtd();

  /*! \brief Set the I/O count
   *
   * Note that analog inputs (mdtAnalogIo objects) are not created here.
   *  They will be created by mdtDeviceModbusWagoModule::setupFromRegisterWord() .
   *
   * \pre count must be 2 or 4
   */
  void setIoCount(int count);

  /*! \brief Set the PSRR mode
   */
  void setPsrrMode(psrrMode_t mode);

  /*! \brief Get the PSRR mode
   */
  psrrMode_t psrrMode() const;

  /*! \brief Set the sensor type for given channel
   *
   * \pre Channel must be in a valid range
   */
  void setSensorType(int channel, sensorType_t type);

  /*! \brief Get the sensor type of given channel
   *
   * \pre Channel must be in a valid range
   */
  sensorType_t sensorType(int channel) const;

  /*! \brief
   */
  void setConnectionTypeWiresCount(int channel, int wiresCount);

  /*! \brief
   */
  int connectionTypeWiresCount(int channel) const;

  /*! \brief
   */
  void setWatchdogTimerEnabled(int channel, bool enable);

  /*! \brief
   */
  bool watchdogTimerIsEnabled(int channel) const;

  /*! \brief
   */
  void setAverageValueFilterEnabled(int channel, bool enable);

  /*! \brief
   */
  bool averageValueFilterIsEnabled(int channel) const;

  /*! \brief Set process value representation for given channel
   *
   * \pre Channel must be in a valid range
   */
  void setProcessValueRepresentation(int channel, processValueRepresentation_t representation);

  /*! \brief Get process value representation for given channel
   *
   * \pre Channel must be in a valid range
   */
  processValueRepresentation_t processValueRepresentation(int channel) const;

  /*! \brief
   */
  bool siemensFormatIsEnabled(int channel) const;

  /*! \brief
   */
  void setWireBreakShortCircuitIndicationEnabled(int channel, bool enable);

  /*! \brief
   */
  bool wireBreakShortCircuitIndicationIsEnabled(int channel) const;

  /*! \brief
   */
  void setOutOfRangeIndicationEnabled(int channel, bool enable);

  /*! \brief
   */
  bool outOfRangeIndicationIsEnabled(int channel) const;

  /*! \brief
   */
  void setOutOfRangeIndicationLimits(int channel, int underrange, int overrange);

  /*! \brief
   */
  int underrangeIndicationLimit(int channel);

  /*! \brief
   */
  int overrangeIndicationLimit(int channel);

  /*! \brief
   */
  void setManufacturerCalibration(int channel);

  /*! \brief
   */
  void setUserCalibration(int channel, double offset, double gain);

  /*! \brief
   */
  double calibrationOffset() const;

  /*! \brief
   */
  double calibrationGain() const;

  /*! \brief Set scaling
   */
  void setScaling(int channel, bool useManufacturerScaling, bool useUserScaling, double userOffset, double userGain, double twoWireOffset);

  /*! \brief Check if manufacturer scaling is used
   */
  bool manufacturerScalingIsEnabled(int channel) const;

  /*! \brief Get manufacturer scaling offset
   */
  double manufacturerScalingOffset(int channel) const;

  /*! \brief Get manufacturer scaling gain
   */
  double manufacturerScalingGain(int channel) const;

  /*! \brief Check if user scaling is used
   */
  bool userScalingIsEnabled(int channel) const;

  /*! \brief Get user scaling offset
   */
  double userScalingOffset(int channel) const;

  /*! \brief Get user scaling gain
   */
  double userScalingGain(int channel) const;

  /*! \brief
   */
  double twoWireOffset(int channel) const;

 private:

  /*! \brief Get the min value of a range of analog inputs
   */
  QVariant analogInputValueMin(int partNumber, int channel) const;

  /*! \brief Get the max value of a range of analog inputs
   */
  QVariant analogInputValueMax(int partNumber, int channel) const;

  /*! \brief Get the number of bits (including sign bit) used to represent a value of analog inputs
   */
  int analogInputValueBitsCount(int partNumber, int channel) const;

  /*! \brief Get index of the first bit (LSB) that represents the value of analog inputs
   */
  int analogInputValueLsbIndex(int partNumber, int channel) const;

  /*! \brief Check if a analog inputs returns a signed value or not
   */
  QVariant analogInputValueSigned(int partNumber, int channel) const;

  /*! \brief Check if analog input's value is scaled from minimum to maximum
   */
  QVariant analogInputValueScaledFromMinToMax(int partNumber, int channel) const;

  /*! \brief Get analog input's conversion factor
   */
  QVariant analogInputValueConversionFactor(int partNumber, int channel) const;

  /*! \brief Get number of analog inputs
   */
  int analogInputsCount(int partNumber) const;

  /*! \brief Get number of analog outputs
   */
  int analogOutputsCount(int partNumber) const;

  /*! \brief Get unit of a analog inputs (V, A, ...)
   */
  QString analogInputUnit(int partNumber, int channel) const;

  /*! \brief Update registers count
   *
   * Will get channel count using ioCountRegisters()
   *  and update mdtDeviceModbusWagoModule's control/status/data bytes count
   *  and internal channel registers count (register 32, 33, ...)
   *
   * It also will use firstAddressRead() and firstAddressWrite() to set register addresses.
   */
  ///void updateRegisterCount();

  psrrMode_t pvPsrrMode;
  QList<sensorType_t> pvSensorTypes;
  QList<processValueRepresentation_t> pvProcessValueRepresentations;
};

#endif  // #ifndef MDT_DEVICE_MODBUS_WAGO_MODULE_RTD_H
