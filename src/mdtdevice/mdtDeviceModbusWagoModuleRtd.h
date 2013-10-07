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

/*! \brief Get/Set setup and help for module detection for special RTD modules
 *
 * Current version implements following modules:
 *  - 750-464 2/4 AI RTD
 *
 * Several set/get methods are provided to handle settings of module.
 *  Note that these methods only work on internally cached values.
 *  To really get/set settings from/to device, .... \todo Finish comment
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

  /*! \brief Get module setup
   */
  bool getSpecialModuleSetup(quint16 word, int firstAddressRead, int firstAddressWrite);

  //Will also enter password 0x1235 in register 31
  /*! \brief Enter password 0x1235 to register 31
   */
  bool enterSetupMode();

  /*! \brief Reset password of register 31 to 0x0000
   */
  bool exitSetupMode();

  /*! \brief Set the I/O count
   *
   * Will only set I/O count to cached register 47.
   *
   * \pre count must be 2 or 4
   * \sa ioCountRegister()
   */
  void setIoCount(int count);

  /*! \brief Get I/O count
   *
   * This method only returns I/O count cached
   *  in register 47 and can return a other result
   *  than mdtDeviceModbusWagoModule::ioCount() .
   */
  int ioCountRegisters() const;

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

  /*! \brief
   */
  void setProcessValueRepresentation(int channel, processValueRepresentation_t representation);

  /*! \brief
   */
  processValueRepresentation_t processValueRepresentation(int channel);

  ///void setSiemensFormatEnabled(int channel, bool enable);

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

  /*! \brief Set a setup for tests
   *
   * This method is used by unit tests and should not be used in application.
   */
  void setInitialSetupForTests(int channelCount, int firstAddressRead, int firstAddressWrite);

 private:

  /*! \brief Update registers count
   *
   * Will get channel count using ioCountRegisters()
   *  and update mdtDeviceModbusWagoModule's control/status/data bytes count
   *  and internal channel registers count (register 32, 33, ...)
   *
   * It also will use firstAddressRead() and firstAddressWrite() to set register addresses.
   */
  void updateRegisterCount();

  /*! \brief Set a register for a given range of channels
   *
   * Will setup control byte (Cx) of given channels
   *  with registerNumber, Reg_Com bit set and write access.
   * Then, control byte (Cx) and cached data bytes (Dx) are sent to module.
   *
   * \pre firstChannel and lastChannel must be in a valid range
   * \sa mdtDeviceModbusWagoModule::setRegisterDataWord()
   * \sa mdtDeviceModbusWagoModule::setRegisterDataBytes()
   */
  bool setRegister(int registerNumber, int firstChannel, int lastChannel, bool needDeviceSetupState);

  /*! \brief Get a register for given range of channels
   *
   * Will setup control byte (Cx) of given channels
   *  with registerNumber, Reg_Com bit set and read access.
   *  Then, control byte is sent to module.
   *  Next, status bytes, register data are readen from module and cached.
   *
   * \pre firstChannel and lastChannel must be in a valid range
   */
  bool getRegister(int registerNumber, int firstChannel, int lastChannel, bool needDeviceSetupState);

  /*! \brief Set cached register 47 to device
   */
  bool setRegister47();

  /*! \brief Get register 47 from device and store it into cache
   *
   * On success, register count will be updated with updateRegisterCount()
   */
  bool getRegister47();

  /*! \brief Set cached register 32 for each existing channel to device
   */
  bool setRegister32();

  /*! \brief Get register 32 from device for each existing channel and store it into cache
   */
  bool getRegister32();

  QList<quint16> pvRegister32;
  QList<quint16> pvRegister33;
  QList<quint16> pvRegister34;
  QList<quint16> pvRegister35;
  QList<quint16> pvRegister36;
  QList<quint16> pvRegister37;
  QList<quint16> pvRegister39;
  QList<quint16> pvRegister40;
  quint16 pvRegister47;

};

#endif  // #ifndef MDT_DEVICE_MODBUS_WAGO_MODULE_RTD_H
