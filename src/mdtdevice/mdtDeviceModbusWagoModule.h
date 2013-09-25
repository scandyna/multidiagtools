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
#ifndef MDT_DEVICE_MODBUS_WAGO_MODULE_H
#define MDT_DEVICE_MODBUS_WAGO_MODULE_H

#include <QString>
#include <QVariant>
#include <QPair>
#include <QList>

class mdtDeviceModbusWago;
class mdtAnalogIo;
class mdtDigitalIo;

/*! \brief Helper class for Wago modules detection and setup
 */
class mdtDeviceModbusWagoModule
{
 public:

  /*! \brief Module type
   */
  enum type_t {
                Unknown ,         /*!< Unknown module type */
                AnalogInputs ,    /*!< Analog inputs module */
                AnalogOutputs ,   /*!< Analog outputs module */
                DigitalInputs ,   /*!< Digital inputs module */
                DigitalOutputs    /*!< Digital outputs module */
              };

  /*! \brief Constructor
   *
   * \param autoDeleteIos If true, internally created I/O's are deleted when this object is destroyed
   * \param device Pointer to device (used for register service access)
   * \pre device must be a valid pointer
   */
  mdtDeviceModbusWagoModule(bool autoDeleteIos, mdtDeviceModbusWago *device);

  /*! \brief Destructor
   *
   * If autoDeleteIos is true, internal I/O's are deleted
   */
  virtual ~mdtDeviceModbusWagoModule();

  /*! \brief Clear I/O module
   *
   * If autoDeleteIos is set, memory is freed for internal I/O
   *  (mdtAbstractIo pointers will also become invalid)
   *
   * \param forceDeleteIos Will delete I/O's undependant of autoDeleteIos flag.
   */
  void clear(bool forceDeleteIos = false);

  /*! \brief Setup module
   *
   * During I/O detection, the fieldbus coupler give a word for each module attached on its KBUS.
   *  This word contains several informations such as a part number (for analog I/O module)
   *  or flag bits (for digital I/O module).
   *
   * This method will use this word do create internally I/O's based on mdtAbstractIo
   *  and setup them on the base of known parameters (range, type, etc...).
   *  Note that no absolute addressing is done at this step.
   *  Call setFirstAddresse() for this.
   *
   * \param word Word that contains I/O informations.
   */
  bool setupFromRegisterWord(quint16 word);

  /*! \brief Set module process image I/O range
   *
   * Setup must be done before addressing can be done.
   *  Sett setupFromRegisterWord() .
   *
   * Note: if address of mdtAbstractIo based objects are updated,
   *        the new given address will not be reflected here.
   *
   * If module is of type AnalogInputs or DigitalInputs, and has no control bytes
   *  addressWrite is ignored.
   *
   * \pre addressRead must be >= 0 .
   * \pre if module contains outputs and/or control bytes, addressWrite must be >= 0 .
   */
  void setFirstAddress(int addressRead, int addressWrite = -1);

  /*! \brief Get first address (for read access) of module's process image
   *
   * Returns a value < 0 if addressing was not done
   */
  int firstAddressRead() const;

  /*! \brief Get first address (for write access) of module's process image
   *
   * Returns a value < 0 if addressing was not done or module has no outputs and no control bytes
   */
  int firstAddressWrite() const;

  /*! \brief Get last address (for read access) of module's process image
   *
   * Returns a value < 0 if addressing was not done
   */
  int lastAddressRead() const;

  /*! \brief Get last address (for write access) of module's process image
   *
   * Returns a value < 0 if addressing was not done or module has no outputs and no control bytes
   */
  int lastAddressWrite() const;

  /*! \brief Get number of internal I/O's
   *
   * Note: special registers are not considered as I/O,
   *        only analog or digital are counted.
   */
  int ioCount() const;

  /*! \brief Access internal analog I/O's
   */
  const QList<mdtAnalogIo*> & analogIos();

  /*! \brief Access internal digital I/O's
   */
  const QList<mdtDigitalIo*> & digitalIos();

  /*! \brief Get module type
   */
  type_t type() const;

  ///QString typeText() const;

  /*! \brief Get module's part number
   *
   * Part number is the 3 last digits of wago article number.
   *  F.ex. : for the 750-457 , this method will return 457.
   * Note: part number is only available for analog I/O module.
   */
  int partNumber() const;

  /*! \brief Get text version of part number
   *
   * \sa partNumber()
   */
  QString partNumberText() const;

 protected:

  /*! \brief Get the status byte for given channel
   *
   * For channel 0, status byte S0 is returned,
   *  for channel 1, status byte S1 , ...
   *
   * See Wago documentation about register communication for details.
   *
   * Note: this method returns only the cached status byte,
   *  to get value from physical module, use readStatusBytes() first.
   *
   * \pre channel must be in a valid range
   */
  quint8 statusByte(int channel) const;

  /*! \brief Set the control byte for given channel
   *
   * For channel 0, control byte C0 is set,
   *  for channel 1, control byte C1 is set , ...
   *
   * See Wago documentation about register communication for details.
   *
   * Note: this method set only the cached control byte,
   *  to set value to physical module, use writeControlBytes().
   *
   * \pre channel must be in a valid range
   */
  void setControlByte(int channel, quint8 byte);

  /*! \brief Get the control byte for given channel
   *
   * \sa setControlByte()
   * \pre channel must be in a valid range
   */
  quint8 controlByte(int channel) const;

  /*! \brief Set register data word for given channel
   *
   * For channel 0, register data bytes D0 and D1 are set,
   *  for channel 1, register data bytes D2 and D3 are set, ...
   *
   * See Wago documentation about register communication for details.
   *
   * Note: this method only set cached register data bytes,
   *  to set them to physical module, use writeRegisters() .
   *
   * \pre channel must be in a valid range
   */
  void setRegisterDataWord(int channel, quint16 word);

  /*! \brief Get register data word for given channel
   *
   * \sa setRegisterDataWord()
   */
  quint16 registerDataWord(int channel) const;

  /*! \brief Set register data bytes for given channel
   *
   * For channel 0, byteD0 is set to byte D0, byteD1 to byte D1,
   *  for channel 1, byteD0 to byte D2, byteD1 to byte D3, ...
   *
   * See Wago documentation about register communication for details.
   *
   * Note: this method only set cached register data bytes,
   *  to set them to physical module, use writeRegisters() .
   *
   * \pre channel must be in a valid range
   */
  void setRegisterDataBytes(int channel, quint8 byteD1, quint8 byteD0);

  /*! \brief Get register data byte D1 or D3 or ...
   *
   * \sa setRegisterDataBytes()
   *
   * \pre channel must be in a valid range
   */
  quint8 registerDataByteD1(int channel);

  /*! \brief Get register data byte D0 or D2 or ...
   *
   * \sa setRegisterDataBytes()
   *
   * \pre channel must be in a valid range
   */
  quint8 registerDataByteD0(int channel);

  /*! \brief Read register bytes for given range of channels
   *
   * Will read status (Sx) and data (Dx) bytes from physical module
   *  and update internally cached values.
   *
   * Note: this method will not send a specific query.
   *  For this, control byte must be setup regarding specific module and
   *  sent to device with writeControlBytes() first.
   *
   * \pre firstChannel and lastChannel must be in a valid range
   */
  bool readRegisters(int firstChannel, int lastChannel, bool needDeviceSetupState);

  /*! \brief Write register bytes for given range of channels
   *
   * Will write cached control (Cx) and data (Dx) bytes to physical module.
   */
  bool writeRegisters(int firstChannel, int lastChannel, bool needDeviceSetupState);

  /*! \brief Read status byte for given range of channels
   *
   * Will read status byte (Sx) from physical module and update internally cached value.
   *
   * \sa statusByte()
   * \pre firstChannel and lastChannel must be in a valid range
   */
  bool readStatusBytes(int firstChannel, int lastChannel, bool needDeviceSetupState);

  /*! \brief Write control byte for given range of channels
   *
   * Will write cached control byte (Cx) to physical module.
   *
   * \sa setControlByte()
   */
  bool writeControlBytes(int firstChannel, int lastChannel, bool needDeviceSetupState);

  /*! \brief Get the min value of a range of special (analog) module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  virtual QVariant specialModuleValueMin();

  /*! \brief Get the max value of a range of special (analog) module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  virtual QVariant specialModuleValueMax();

  /*! \brief Get the number of bits (including sign bit) used to represent a value of special (analog) module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  virtual QVariant specialModuleValueBitsCount();

  /*! \brief Get index of the first bit (LSB) that represents the value of special (analog) module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  virtual QVariant specialModuleValueLsbIndex();

  /*! \brief Check if a special (analog) module returns a signed value or not
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  virtual QVariant specialModuleValueSigned();

  /*! \brief Check if a special module returns is a inputs module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  virtual QVariant specialModuleIsInput();

  /*! \brief Get I/O's count of a special module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  virtual QVariant specialModuleIosCount();

 private:

  /*! \brief Get the min value of a range of a analog I/O module
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return The min. value in floating format. If partNumber is unknow, a invalid QVariant is returned.
   */
  QVariant analogIoModuleValueMin(int partNumber) const;

  /*! \brief Get the max value of a range of a analog I/O module
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return The max. value in floating format. If partNumber is unknow, a invalid QVariant is returned.
   */
  QVariant analogIoModuleValueMax(int partNumber) const;

  /*! \brief Get the number of bits (including sign bit) used to represent a value of a analog I/O module
   *
   * The resolution of each analog module is not the same.
   *  For example, the 750-457 has a resolution of 13 bits.
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return bits count used for value, or -1 for a unknown module.
   */
  int analogIoModuleValueBitsCount(int partNumber) const;

  /*! \brief Get index of the first bit (LSB) that represents the value of a analog I/O module
   *
   * Some modules have bits used for other purprose than value.
   *  At exemple, for module 750-457, the LSB index is 3.
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return Index of LSB of value, or -1 for unknown module.
   */
  int analogIoModuleValueLsbIndex(int partNumber) const;

  /*! \brief Check if a analog I/O module returns a signed value or not
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return True if module's representation is signed, false if not.
   *          For a unknown module, a invalid QVariant is returned.
   */
  QVariant analogIoModuleValueSigned(int partNumber) const;

  /*! \brief Check if a analog I/O module is a input
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return True if module is a input, false if module is a output.
   *          For a unknown module, a invalid QVariant is returned.
   */
  QVariant analogIoModuleIsInput(int partNumber) const;

  /*! \brief Get number of I/Os for a analog I/O module
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return Number of I/O's or -1 for unknown module.
   */
  int analogIoModuleIosCount(int partNumber) const;

  /*! \brief Get unit of a analog I/O module (V, A, ...)
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return Unit of module, or a empty string for unknown module.
   */
  QString analogIoModuleUnit(int partNumber) const;

  /*! \brief Check if a digital I/O module is a input
   *
   * \param word The identification word (see Wago doc, part of register 0x2030 for details).
   * \return True if module is a input, false if module is a output.
   *          For a unknown or uncoherent description, a invalid QVariant is returned.
   */
  QVariant digitalIoModuleIsInput(quint16 word) const;

  /*! \brief Get number of I/Os for a digital I/O module
   *
   * \param word The identification word (see Wago doc, part of register 0x2030 for details).
   * \return Number of I/O's or -1 for unknown description.
   */
  int digitalIoModuleIosCount(quint16 word) const;

  /*! \brief Build a new analog I/O
   *
   * Will get parameters reagrding part number if known.
   *  The new created I/O is configured.
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return A new configured analog I/O, or a null pointer if module part number is unknown or other error.
   *          Note: the returned object must be deleted by user whenn not used anymore.
   */
  mdtAnalogIo *getNewAnalogIo(int partNumber) const;

  /*! \brief Create, configure and add module's analog I/O's
   *
   * Will use given partNumber to set parameters of each I/O.
   *
   * \return True on success. A failure probably indicates a that module is unknown.
   */
  bool addAnalogIos(int partNumber);

  /*! \brief Create, configure and add module's analog I/O's
   *
   * Will use given word to set parameters of each I/O.
   *
   * \return True on success.
   */
  bool addDigitalIos(quint16 word);

  Q_DISABLE_COPY(mdtDeviceModbusWagoModule);

  QList<mdtAnalogIo*> pvAnalogIos;
  QList<mdtDigitalIo*> pvDigitalIos;
  bool pvAutoDeleteIos;
  type_t pvType;    // Module type
  ///QString TypeText;
  int pvPartNumber;
  // List for register communication. QPair::first is address, QPair::second is data part
  QList<QPair<int, quint8> > pvStatusBytes;
  QList<QPair<int, quint8> > pvControlBytes;
  QList<QPair<int, quint16> > pvRegisterDataWords;
  // Device
  mdtDeviceModbusWago *pvDevice;

  ///<mdtDeviceModbusWagoModuleAttribute> pvAttributes;
};

#endif  // #ifndef MDT_DEVICE_MODBUS_WAGO_MODULE_H
