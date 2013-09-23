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
#ifndef MDT_DEVICE_MODBUS_WAGO_H
#define MDT_DEVICE_MODBUS_WAGO_H

#include "mdtDeviceModbus.h"
#include "mdtDeviceInfo.h"
#include "mdtDeviceIos.h"
#include <QList>
#include <QVariant>

class mdtDeviceModbusWagoModule;

/*! \brief
 */
class mdtDeviceModbusWago : public mdtDeviceModbus
{
 Q_OBJECT

 public:

  mdtDeviceModbusWago(QObject *parent = 0);

  ~mdtDeviceModbusWago();

  /*! \brief Search and connect to physical device.
   *
   * Will scan available ports and open the first port that
   *  has device attached maching request.
   *
   * Note: in current version, only Wago 750 system is supported.
   *
   * \param devInfo Requested device's informations (can be empty, no information is used in current version).
   * \return A error listed in mdtAbstractPort::error_t (NoError on success)
   */
  mdtAbstractPort::error_t connectToDevice(const mdtDeviceInfo &devInfo);

  /*! \brief Search and connect to physical device.
   *
   * Work as mdtDeviceModbus::connectToDevice() , but checks that device is a Wago 750 fieldbus coupler.
   */
  mdtAbstractPort::error_t connectToDevice(const QList<mdtPortInfo*> &scanResult, int hardwareNodeId, int bitsCount, int startFrom = 0);

  /*! \brief Check if device is a Wago 750 series
   */
  bool isWago750();

  /*! \brief Get number of analog outputs
   *
   * Note: calling this method will query the device
   *
   * \return Number of analog outputs, or < 0 on error
   */
  int analogOutputsCount();

  /*! \brief Get number of analog inputs
   *
   * Note: calling this method will query the device
   *
   * \return Number of analog inputs, or < 0 on error
   */
  int analogInputsCount();

  /*! \brief Get number of digital outputs
   *
   * Note: calling this method will query the device
   *
   * \return Number of digital outputs, or < 0 on error
   */
  int digitalOutputsCount();

  /*! \brief Get number of digital inputs
   *
   * Note: calling this method will query the device
   *
   * \return Number of digital inputs, or < 0 on error
   */
  int digitalInputsCount();

  /*! \brief Query device about filed bus + I/Os setup
   *
   * The detected I/Os are added to given ios,
   *  but not added to internal ios container (use setIos() for this).
   *  Goal is to be able to detect setup without alter the internal container,
   *  so a check could be made before running really, or something else.
   * 
   */
  bool detectIos(mdtDeviceIos *ios);

  /*! \brief Get the min value of a range of a analog I/O module
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return The min. value in floating format. If partNumber is unknow, a invalid QVariant is returned.
   */
  ///QVariant analogIoModuleValueMin(int partNumber) const;

  /*! \brief Get the max value of a range of a analog I/O module
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return The max. value in floating format. If partNumber is unknow, a invalid QVariant is returned.
   */
  ///QVariant analogIoModuleValueMax(int partNumber) const;

  /*! \brief Get the number of bits (including sign bit) used to represent a value of a analog I/O module
   *
   * The resolution of each analog module is not the same.
   *  For example, the 750-457 has a resolution of 13 bits.
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return bits count used for value, or -1 for a unknown module.
   */
  ///int analogIoModuleValueBitsCount(int partNumber) const;

  /*! \brief Get index of the first bit (LSB) that represents the value of a analog I/O module
   *
   * Some modules have bits used for other purprose than value.
   *  At exemple, for module 750-457, the LSB index is 3.
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return Index of LSB of value, or -1 for unknown module.
   */
  ///int analogIoModuleValueLsbIndex(int partNumber) const;

  /*! \brief Check if a analog I/O module returns a signed value or not
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return True if module's representation is signed, false if not.
   *          For a unknown module, a invalid QVariant is returned.
   */
  ///QVariant analogIoModuleValueSigned(int partNumber) const;

  /*! \brief Check if a analog I/O module is a input
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return True if module is a input, false if module is a output.
   *          For a unknown module, a invalid QVariant is returned.
   */
  ///QVariant analogIoModuleIsInput(int partNumber) const;

  /*! \brief Get number of I/Os for a analog I/O module
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return Number of I/O's or -1 for unknown module.
   */
  ///int analogIoModuleIosCount(int partNumber) const;

  /*! \brief Get unit of a analog I/O module (V, A, ...)
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return Unit of module, or a empty string for unknown module.
   */
  ///QString analogIoModuleUnit(int partNumber) const;

  /*! \brief Check if a digital I/O module is a input
   *
   * \param word The identification word (see Wago doc, part of register 0x2030 for details).
   * \return True if module is a input, false if module is a output.
   *          For a unknown or uncoherent description, a invalid QVariant is returned.
   */
  ///QVariant digitalIoModuleIsInput(quint16 word) const;

  /*! \brief Get number of I/Os for a digital I/O module
   *
   * \param word The identification word (see Wago doc, part of register 0x2030 for details).
   * \return Number of I/O's or -1 for unknown description.
   */
  ///int digitalIoModuleIosCount(quint16 word) const;

 private:

  /*! \brief Build a new analog I/O
   *
   * Will get parameters reagrding part number if known.
   *  The new created I/O is configured.
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return A new configured analog I/O, or a null pointer if module part number is unknown or other error.
   *          Note: the returned object must be deleted by user whenn not used anymore.
   */
  ///mdtAnalogIo *getNewAnalogIo(int partNumber) const;

  /*! \brief Add analog I/O(s) to I/O's container
   *
   * Will get number of analog I/Os that given module contains,
   *  get it's parameters and add it to the correct container.
   *
   * \param analogInputs Analog inputs container.
   *                      (module's I/Os will be added to this container if it is a analog inputs module).
   * \param analogOutputs Analog outputs container.
   *                      (module's I/Os will be added to this container if it is a analog outputs module).
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return True on success, false else (probably due to a unknown part number).
   */
  ///bool addAnalogIos(QList<mdtAnalogIo*> &analogInputs, QList<mdtAnalogIo*> &analogOutputs, int partNumber) const;

  /*! \brief Add digital I/O(s) to I/O's container
   *
   * Will get number of digital I/Os that given module contains,
   *  get it's parameters and add it to the correct container.
   *
   * \param digitalInputs Digital inputs container.
   *                      (module's I/Os will be added to this container if it is a digital inputs module).
   * \param digitalOutputs Digital outputs container.
   *                      (module's I/Os will be added to this container if it is a digital outputs module).
   * \param word The identification word (see Wago doc, part of register 0x2030 for details).
   * \return True on success, false else (probably due to a unknown part number).
   */
  ///bool addDigitalIos(QList<mdtDigitalIo*> &digitalInputs, QList<mdtDigitalIo*> &digitalOutputs, quint16 word) const;

  QList<mdtDeviceModbusWagoModule*> pvModules;
  
  // Helper members for Register service
  ///bool getRegisterValues(int address, int n);
  ///QList<int> pvRegisterValues;
};

#endif  // #ifndef MDT_DEVICE_MODBUS_WAGO_H
