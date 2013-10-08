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
                Unknown ,             /*!< Unknown module type */
                AnalogInputs ,        /*!< Analog inputs module */
                AnalogOutputs ,       /*!< Analog outputs module */
                DigitalInputs ,       /*!< Digital inputs module */
                DigitalOutputs ,      /*!< Digital outputs module */
                DigitalInputsOutputs  /*!< Module with digital inputs and outputs */
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
   *
   *  Note that no absolute addressing is done at this step.
   *  Call updateAddresses() for this.
   *
   * Some special modules need to communicate with fieldbus coupler to access register service.
   *  To get setup during I/O detection, use getSpecialModuleSetup() instead of this method. \todo Adapt
   *
   * \param word Word that contains I/O informations.
   */
  bool setupFromRegisterWord(quint16 word);

  /*! \brief Update addresses of the module
   *
   * If no I/O was set, this method will store given addresses.
   *  This can be usefull for some special modules witch needs to communicate
   *  with fieldbus during setup.
   *
   * However, the normal way is to stup the module with setupFromRegisterWord() first,
   *  then call this method to address I/Os.
   *
   * This default implementation will address all present I/Os in a linear way.
   *
   * \param firstAiAddress First address in the analog inputs process image.
   * \param firstAoAddressRead  First address in the analog outputs process image for read access.
   * \param firstAoAddressWrite First address in the analog outputs process image for write access.
   * \param firstDiAddress First address in the digital inputs process image.
   * \param firstDoAddressRead  First address in the digital outputs process image for read access.
   * \param firstDoAddressWrite First address in the digital outputs process image for write access.
   */
  virtual void updateAddresses(int firstAiAddress, int firstAoAddressRead, int firstAoAddressWrite, int firstDiAddress, int firstDoAddressRead, int firstDoAddressWrite);

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
   * 
   * \todo Obselete. Should become a virtual protected method, like updateAddresses() or something so..
   */
  void setFirstAddress(int addressRead, int addressWrite = -1);

  /*! \brief Setup a special module
   *
   * Use this method during I/O detection. It has same goal as setupFromRegisterWord() ,
   *  but is used for special modules.
   *
   * Because some special module needs to communicate with fieldbus coupler,
   *  first address is needed.
   *
   * Note: special module subclass must implement this method.
   *  This default implementation does nothing and allways returns false.
   * 
   * \todo Obselete. Should become a protected method that will read setup from module.
   */
  virtual bool getSpecialModuleSetup(quint16 word, int firstAddressRead, int firstAddressWrite);

  /*! \brief Get first address of module's analog inputs process image
   */
  int firstAiAddress() const;

  /*! \brief Get last address of module's analog inputs process image
   */
  virtual int lastAiAddress() const;

  /*! \brief Get first address for next module in analog inputs process image
   */
  virtual int nextModuleFirstAiAddress() const;

  /*! \brief Get first address of module's analog outputs process image for read access
   */
  int firstAoAddressRead() const;

  /*! \brief Get last address of module's analog outputs process image for read access
   */
  virtual int lastAoAddressRead() const;

  /*! \brief Get first address for next module in analog outputs process image for read access
   */
  virtual int nextModuleFirstAoAddressRead() const;

  /*! \brief Get first address of module's analog outputs process image for write access
   */
  int firstAoAddressWrite() const;

  /*! \brief Get last address of module's analog outputs process image for write access
   */
  virtual int lastAoAddressWrite() const;

  /*! \brief Get first address for next module in digital outputs process image for write access
   */
  virtual int nextModuleFirstAoAddressWrite() const;

  /*! \brief Get first address of module's digital inputs process image
   */
  int firstDiAddress() const;

  /*! \brief Get last address of module's digital inputs process image
   */
  virtual int lastDiAddress() const;

  /*! \brief Get first address for next module in digital inputs process image
   */
  virtual int nextModuleFirstDiAddress() const;

  /*! \brief Get first address of module's digital outputs process image for read access
   */
  int firstDoAddressRead() const;

  /*! \brief Get last address of module's digital outputs process image for read access
   */
  virtual int lastDoAddressRead() const;

  /*! \brief Get first address for next module in digital outputs process image for read access
   */
  virtual int nextModuleFirstDoAddressRead() const;

  /*! \brief Get first address of module's digital outputs process image for write access
   */
  int firstDoAddressWrite() const;

  /*! \brief Get last address of module's digital outputs process image for write access
   */
  virtual int lastDoAddressWrite() const;

  /*! \brief Get first address for next module in digital outputs process image for write access
   */
  virtual int nextModuleFirstDoAddressWrite() const;


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

  
  /*! \brief Access internal analog inputs
   */
  const QList<mdtAnalogIo*> & analogInputs();

  /*! \brief Access internal analog outputs
   */
  const QList<mdtAnalogIo*> & analogOutputs();

  /*! \brief Access internal digital inputs
   */
  const QList<mdtDigitalIo*> & digitalInputs();

  /*! \brief Access internal digital outputs
   */
  const QList<mdtDigitalIo*> & digitalOutputs();

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

  /*! \brief Get device name
   */
  QString deviceName();

 protected:

  /*! \brief Add a analog input
   *
   * \pre aio must be a valid pointer
   */
  void addAnalogInput(mdtAnalogIo *aio);

  /*! \brief Add a analog output
   *
   * \pre aio must be a valid pointer
   */
  void addAnalogOutput(mdtAnalogIo *aio);

  /*! \brief Add a digital input
   *
   * \pre dio must be a valid pointer
   */
  void addDigitalInput(mdtDigitalIo *dio);

  /*! \brief Add a digital output
   *
   * \pre dio must be a valid pointer
   */
  void addDigitalOutput(mdtDigitalIo *dio);

  /*! \brief Add registers
   *
   * Will add control byte, status byte and register data for given channelCount
   */
  void addRegisters(int channelCount, int firstAddressRead, int firstAddressWrite);

  /*! \brief Clear registers
   *
   * Will remove control bytes, status bytes and data registers.
   */
  void clearRegisters();

  /*! \brief Get number of channels regarding registers
   *
   * During I/O detection or channel count setup,
   *  register count can be incoherent with I/O count.
   *
   * This method will return channel count only regarding
   *  number of control/status/data register sizes.
   */
  int channelCountRegisters() const;

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
   *
   * \pre firstChannel and lastChannel must be in a valid range
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
   * \pre firstChannel and lastChannel must be in a valid range
   */
  bool writeControlBytes(int firstChannel, int lastChannel, bool needDeviceSetupState);

  /*! \brief Wait some time
   *
   * \sa mdtDevice::wait()
   */
  void wait(int ms);

  /*! \brief Get the min value of a range of special (analog) module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  ///virtual QVariant specialModuleValueMin();

  /*! \brief Get the max value of a range of special (analog) module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  ///virtual QVariant specialModuleValueMax();

  /*! \brief Get the number of bits (including sign bit) used to represent a value of special (analog) module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  ///virtual QVariant specialModuleValueBitsCount();

  /*! \brief Get index of the first bit (LSB) that represents the value of special (analog) module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  ///virtual QVariant specialModuleValueLsbIndex();

  /*! \brief Check if a special (analog) module returns a signed value or not
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  ///virtual QVariant specialModuleValueSigned();

  /*! \brief Check if a special module returns is a inputs module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  ///virtual QVariant specialModuleIsInput();

  /*! \brief Get I/O's count of a special module
   *
   * Subclass that implement special module should re-implement this method.
   *
   * Default implementation returns a invalid QVariant
   */
  ///virtual QVariant specialModuleIosCount();
  

  /*! \brief Get the min value of a range of analog inputs
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return The min. value in floating format. If partNumber is unknow, a invalid QVariant is returned.
   */
  virtual QVariant analogInputsValueMin(int partNumber) const;

  /*! \brief Get the min value of a range of analog outputs
   *
   * \param partNumber The right part of Wago part number (f.ex. 550 if module is a 750-550).
   * \return The min. value in floating format. If partNumber is unknow, a invalid QVariant is returned.
   */
  virtual QVariant analogOutputsValueMin(int partNumber) const;

  /*! \brief Get the max value of a range of analog inputs
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return The max. value in floating format. If partNumber is unknow, a invalid QVariant is returned.
   */
  virtual QVariant analogInputsValueMax(int partNumber) const;

  /*! \brief Get the max value of a range of analog outputs
   *
   * \param partNumber The right part of Wago part number (f.ex. 550 if module is a 750-550).
   * \return The max. value in floating format. If partNumber is unknow, a invalid QVariant is returned.
   */
  virtual QVariant analogOutputsValueMax(int partNumber) const;

  /*! \brief Get the number of bits (including sign bit) used to represent a value of analog inputs
   *
   * The resolution of each analog module is not the same.
   *  For example, the 750-457 has a resolution of 13 bits.
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return bits count used for value, or -1 for a unknown module.
   */
  virtual int analogInputsValueBitsCount(int partNumber) const;

  /*! \brief Get the number of bits (including sign bit) used to represent a value of analog outputs
   *
   * The resolution of each analog module is not the same.
   *  For example, the 750-550 has a resolution of 12 bits.
   *
   * \param partNumber The right part of Wago part number (f.ex. 550 if module is a 750-550).
   * \return bits count used for value, or -1 for a unknown module.
   */
  virtual int analogOutputsValueBitsCount(int partNumber) const;

  /*! \brief Get index of the first bit (LSB) that represents the value of analog inputs
   *
   * Some modules have bits used for other purprose than value.
   *  At exemple, for module 750-457, the LSB index is 3.
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return Index of LSB of value, or -1 for unknown module.
   */
  virtual int analogInputsValueLsbIndex(int partNumber) const;

  /*! \brief Get index of the first bit (LSB) that represents the value of analog outputs
   *
   * Some modules have bits used for other purprose than value.
   *  At exemple, for module 750-550, the LSB index is 3.
   *
   * \param partNumber The right part of Wago part number (f.ex. 550 if module is a 750-550).
   * \return Index of LSB of value, or -1 for unknown module.
   */
  virtual int analogOutputsValueLsbIndex(int partNumber) const;

  /*! \brief Check if a analog inputs returns a signed value or not
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return True if module's representation is signed, false if not.
   *          For a unknown module, a invalid QVariant is returned.
   */
  virtual QVariant analogInputsValueSigned(int partNumber) const;

  /*! \brief Check if a analog outputs returns a signed value or not
   *
   * \param partNumber The right part of Wago part number (f.ex. 550 if module is a 750-550).
   * \return True if module's representation is signed, false if not.
   *          For a unknown module, a invalid QVariant is returned.
   */
  virtual QVariant analogOutputsValueSigned(int partNumber) const;

  /*! \brief Check if a analog I/O module is a input
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return True if module is a input, false if module is a output.
   *          For a unknown module, a invalid QVariant is returned.
   */
  ///QVariant analogIoModuleIsInput(int partNumber) const;

  /*! \brief Get number of analog inputs
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return Number of I/O's or -1 for unknown module.
   */
  virtual int analogInputsCount(int partNumber) const;

  /*! \brief Get number of analog outputs
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return Number of I/O's or -1 for unknown module.
   */
  virtual int analogOutputsCount(int partNumber) const;

  /*! \brief Get unit of a analog inputs (V, A, ...)
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return Unit of module, or a empty string for unknown module.
   */
  virtual QString analogInputsUnit(int partNumber) const;

  /*! \brief Get unit of a analog outputs (V, A, ...)
   *
   * \param partNumber The right part of Wago part number (f.ex. 550 if module is a 750-550).
   * \return Unit of module, or a empty string for unknown module.
   */
  virtual QString analogOutputsUnit(int partNumber) const;

  /*! \brief Get size of digital inputs process image
   *
   * For common digital inputs modules, this will match with the number of digital inputs.
   *  For special modules that have status/iformation mapped in porecess image,
   *   these are also include in the result.
   *
   * \param word The identification word (see Wago fieldbus coupler doc, part of register 0x2030 for details).
   * \return Digital inputs process image size [bits] or -1 on unknown description.
   */
  int digitalInputsProcessImageSize(quint16 word) const;

  /*! \brief Get the number of digital inputs
   *
   * This implementation returns value from digitalInputsProcessImageSize() .
   *  This method can be re-implemented for special modules.
   */
  virtual int digitalInputsCount(quint16 word) const;

  /*! \brief Get size of digital outputs process image
   *
   * For common digital outputs modules, this will match with the number of digital outputs.
   *  For special modules that have status/iformation mapped in porecess image,
   *   these are also include in the result.
   *
   * \param word The identification word (see Wago fieldbus coupler doc, part of register 0x2030 for details).
   * \return Digital outputs process image size [bits] or -1 on unknown description.
   */
  int digitalOutputsProcessImageSize(quint16 word) const;

  /*! \brief Get the number of digital outputs
   *
   * This implementation returns value from digitalOutputsProcessImageSize() .
   *  This method can be re-implemented for special modules.
   */
  virtual int digitalOutputsCount(quint16 word) const;

  
  

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

  /*! \brief Build a new analog input
   *
   * Will get parameters reagrding part number if known.
   *  The new created I/O is configured.
   *
   * \param partNumber The right part of Wago part number (f.ex. 457 if module is a 750-457).
   * \return A new configured analog I/O, or a null pointer if module part number is unknown or other error.
   *          Note: the returned object must be deleted by user whenn not used anymore.
   */
  mdtAnalogIo *getNewAnalogInput(int partNumber) const;

  /*! \brief Build a new analog output
   *
   * Will get parameters reagrding part number if known.
   *  The new created I/O is configured.
   *
   * \param partNumber The right part of Wago part number (f.ex. 550 if module is a 750-550).
   * \return A new configured analog I/O, or a null pointer if module part number is unknown or other error.
   *          Note: the returned object must be deleted by user whenn not used anymore.
   */
  mdtAnalogIo *getNewAnalogOutput(int partNumber) const;

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
   *  No addressing is done here.
   *
   * \return True on success.
   */
  bool addDigitalIos(quint16 word);

  Q_DISABLE_COPY(mdtDeviceModbusWagoModule);

  ///QList<mdtAnalogIo*> pvAnalogIos;
  QList<mdtAnalogIo*> pvAnalogInputs;
  QList<mdtAnalogIo*> pvAnalogOutputs;
  ///QList<mdtDigitalIo*> pvDigitalIos;
  QList<mdtDigitalIo*> pvDigitalInputs;
  QList<mdtDigitalIo*> pvDigitalOutputs;
  bool pvAutoDeleteIos;
  type_t pvType;    // Module type
  ///QString TypeText;
  int pvPartNumber;
  // List for register communication. QPair::first is address, QPair::second is data part
  QList<QPair<int, quint8> > pvStatusBytes;
  QList<QPair<int, quint8> > pvControlBytes;
  ///QList<QPair<int, quint16> > pvRegisterDataWords;  /// \todo Don't store address here (is not possible becaus of read/write address)
  QList<quint16> pvRegisterDataWords;
  // Addresses
  int pvFirstAiAddress;
  int pvFirstAoAddressRead;
  int pvFirstAoAddressWrite;
  int pvFirstDiAddress;
  int pvFirstDoAddressRead;
  int pvFirstDoAddressWrite;
  // Device
  mdtDeviceModbusWago *pvDevice;

  ///<mdtDeviceModbusWagoModuleAttribute> pvAttributes;
};

#endif  // #ifndef MDT_DEVICE_MODBUS_WAGO_MODULE_H
