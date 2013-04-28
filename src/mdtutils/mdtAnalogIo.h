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
#ifndef MDT_ANALOG_IO_H
#define MDT_ANALOG_IO_H

#include "mdtAbstractIo.h"
#include <QVariant>

/*! \brief Representation of a analog I/O
 *
 * On wide range of automation applications,
 *  it is usefull to display a I/O to the user.
 *  For example, a analog I/O module, with a 8 bit
 *  encoding could be displayed with this class
 *  togther with mdtAnalogInWidget or mdtAnalogOutWidget.
 */
class mdtAnalogIo : public mdtAbstractIo
{
 Q_OBJECT

 public:

  mdtAnalogIo(QObject *parent = 0);

  ~mdtAnalogIo();

  /*! \brief Set the unit (V, A, °C, ...)
   */
  void setUnit(const QString &unit);

  /*! \brief Get unit
   */
  QString unit() const;

  /*! \brief Specify the range of values and bits decode parameters
   *
   * Store the new range and set the value to the minimum.
   *  The valueChanged() signal is emitted.
   *
   * It can happen that a I/O module returns, f.ex., 
   *  a integer value in a 16 bits packet, but with
   *  some additional flags. (example: Wago 750-457).
   *  In such case, the value is, f.ex., encoded in 12 bits only.
   *
   * Take example of Wago 750-457:
   *  - Value + flags are encoded in a 16 bits word
   *  - Value is encoded in 12 bits, from b3 to b14 + 1 sign bit, b15
   *  - Value is scaled from -10 V to 10 V
   *
   * In this example, this method must be called with
   *  min = -10.0 , max = 10.0 , intValueBitsCount = 13 , intValueLsbIndex = 3 and intValueSigned = true .
   *
   * Note that some analog output hardware use a different resolution for read and write (f.ex. Wago 750-550).
   *  In such case, you can set parameters for reading here, and use setEncodeBitSettings() for alternate
   *  parameters for write.
   *
   * \param min Minimum value to display (f.ex. 0V, or 4mA)
   * \param max Maximum value to display (f.ex. 10V, or 20mA)
   * \param intValueBitsCount Number of bits used for the value, including sign bit.
   *                           (has only effect when using setValueInt() ).
   * \param intValueLsbIndex Index of first bit to use for value extraction.
   *                          (has only effect when using setValueInt() ).
   * \param intValueSigned If true, decoded value is considered as signed value, i.e. the most significant bit is the sign.
   *                        (has only effect when using setValueInt() ).
   *
   * \return true on success. Some checks are done, and if one fails, false is returned and
   *          the previous settingsare used for conversion.
   *
   * \pre max must be > min
   *
   * Note for UI developpers:
   *  - The signal rangeChangedForUi() is emited
   *  - The signal valueChangedForUi() is emited
   */
  bool setRange(double min, double max, int intValueBitsCount, int intValueLsbIndex = 0, bool intValueSigned = false);

  /*! \brief Set the bits encode parammeters
   *
   * On many analog output module, it is possible
   *  to set a value, and read it.
   * It can happen that the bitmap differs between
   *  write and read (f.ex. Wago 750-550).
   * In such case, a alternate bits count and lsb index can be set here.
   *
   * Note that setRange() will overwrite thiese settings.
   *
   * \param intValueBitsCount Number of bits used for the value, including sign bit.
   *                           (has only effect when using valueInt() ).
   * \param intValueLsbIndex Index of first bit to use for value extraction.
   *                          (has only effect when using valueInt() ).
   *
   * \return true on success. Some checks are done, and if one fails, false is returned and
   *          the previous settingsare used for conversion.
   */
  bool setEncodeBitSettings(int intValueBitsCount, int intValueLsbIndex);

  /*! \brief Get the minimum value of the range
   */
  double minimum() const;

  /*! \brief Get the maximum value of the range
   */
  double maximum() const;

  /*! \brief Get value (set with setValue() )
   * 
   * \todo Obselete this (no flags available)
   */
  double value() const;

  /*! \brief Set the integer value
   *
   * Will calculate the real value depending on resolution and update display.
   *  The resolution is set with setRange() (intValueBitsCount parameter)
   *
   * The valueChanged() signal is emitted.
   *
   * \param value The value to store
   * \param isValid The validity flag. This flag is later avaliable with mdtAbstractIo::hasValidData()
   * \param emitValueChanged If true, valueChanged() will not be emitted
   *
   * Note for UI developpers:
   *  - The signal valueChangedForUi() is emited
   */
  void setValueInt(int value, bool isValid, bool emitValueChanged);

  /*! \brief Get the integer value
   *
   * The integer value depend on value and resolution.
   *  The resolution is set with setRange() (steps parameter)
   *
   * \todo Obselete this (no flags available)
   */
  int valueInt() const;

  /*! \brief Set the value to update display
   *
   * Store the value and emit valueChanged() if
   *  new value is different from current.
   *
   * \param value The value to store.
   *               If value is of type int, conversion is done. (see setEncodeBitSettings() ).
   *               If value is invalid, the validity flag is updated.
   *               See setValue(double, bool, bool) for details.
   * \param emitValueChanged If true, valueChanged(int, int) and valueChanged(double) will be emitted.
   *
   * Note for UI developpers:
   *  - The signal valueChangedForUi() is emited
   */
  ///void setValue(QVariant value, bool emitValueChanged);

 public slots:

  /*! \brief Set the value to update display
   *
   * Store the value and emit valueChanged() if
   *  new value is different from current.
   *
   * \param value The value to store
   * \param isValid The validity flag. This flag is later avaliable with mdtAbstractIo::hasValidData()
   * \param emitValueChanged If true, valueChanged(int, int) and valueChanged(double) will be emitted.
   *
   * Note for UI developpers:
   *  - The signal valueChangedForUi() is emited
   */
  ///void setValue(double value, bool isValid, bool emitValueChanged = true);

  /*! \brief Set the value to update display
   *
   * Store the value and emit valueChanged() if
   *  new value is different from current.
   *
   * \param value The value to store
   * \param emitValueChanged If true, valueChanged(const mdtValue&) will be emitted.
   *
   * Note for UI developpers:
   *  - The signal valueChangedForUi() is emited
   */
  void setValue(const mdtValue &value, bool emitValueChanged = false);

  /*! \brief Set the value to update display
   *
   * Overloaded method that calls setValue(double, bool)
   *  with isValid = true.
   */
  ///void setValue(double value);

 signals:

  /*! \brief This signal is emitted whenever the value is changed
   */
  ///void valueChanged(double newValue);

  /*! \brief This signal is emitted whenever the value is changed
   *
   * This signal is used by mdtDeviceIos to notify mdtDevice.
   */
  ///void valueChanged(int addressWrite);

  /*
   * These signals are emited every time
   *  a member is set with a setter method.
   * Usefull to update the UI (should not be used for other purpose)
   */
  void unitChangedForUi(const QString &unit);
  void rangeChangedForUi(double min, double max);

  /// \todo remove
  ///void valueChangedForUi(double newValue);

 private slots:

  // Used from UI to update internal value.
  //  The valueChangedForUi() signal will not be emitted with this call.
  void setValueFromUi(double value);
  ///void setValueFromUi(const mdtValue &value);

 private:

  /*! \brief Store double part, calculate integer part and store it
   *
   * If given value is different than stored one:
   *  - Conversion and storage are done
   *  - valueChangedForUi() is emitted
   *  - If emitValueChanged is true, valueChanged() is emitted
   */
  void setValueFromDouble(const mdtValue &value, bool emitValueChanged);

  /*! \brief Store integer part, calculate double part and store it
   *
   * If given value is different than stored one:
   *  - Conversion and storage are done
   *  - valueChangedForUi() is emitted
   *  - If emitValueChanged is true, valueChanged() is emitted
   */
  void setValueFromInt(const mdtValue &value, bool emitValueChanged);

  Q_DISABLE_COPY(mdtAnalogIo);

  QString pvUnit;
  ///double pvValue;
  double pvMinimum;
  double pvMaximum;
  double pvStep;
  double pvStepInverse;
  ///bool pvUpdatingUi;
  bool pvNotifyUi;
  // Members used for integer <-> float value conversion
  int pvIntValueLsbIndex;     // Index of least significant bit, used in setValueInt()
  int pvIntValueLsbIndexEnc;  // Index of least significant bit, used in valueInt()
  int pvIntValueMask;         // Mask to extract the correct bits count, used in setValueInt()
  int pvIntValueMaskEnc;      // Mask to extract the correct bits count, used in valueInt()
  int pvIntValueSignMask;
  bool pvIntValueSigned;
};

#endif  // #ifndef MDT_ANALOG_IO_H
