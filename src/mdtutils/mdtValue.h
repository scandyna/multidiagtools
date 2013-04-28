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
#ifndef MDT_VALUE_H
#define MDT_VALUE_H

#include <QDebug>

/*! \brief Conatins a value with some attributes
 *
 * Most of cases, using standard C++ types are sufficent.
 *  If a value must be stored in different way,
 *  Qt offers QVariant, witch can convert values between
 *  different types, has a validity flag and more.
 *
 * When dealing with instruments, some other data are needed, for example:
 *  - +OL, -OL
 *  - For case of invalid data: is a convertion error occured, a communication error or something else ?
 *  - Storing data in many format, typical: double and/or int (see mdtAnalogIo for details)
 *
 * Note that this class does no convertion, but is a simple container.
 *
 * Note about -OL/+OL flags: the last setter ( setValue(double, bool, bool) or setValue(int, bool, bool) ) will overwrite this flags.
 *  The setValue(bool) does not affect this flags.
 */
class mdtValue
{
 public:

  /*! \brief Construct a invalid value
   */
  mdtValue();

  /*! \brief Construct a valid value with double format
   *
   * \param value Value to store
   * \param isMinusOl -OL flag
   * \param isPlusOl +OL flag
   */
  mdtValue(double value, bool isMinusOl = false, bool isPlusOl = false);

  /*! \brief Construct a valid value with int format
   *
   * \param value Value to store
   * \param isMinusOl -OL flag
   * \param isPlusOl +OL flag
   */
  mdtValue(int value, bool isMinusOl = false, bool isPlusOl = false);

  /*! \brief Construct a valid value with bool format
   *
   * \param value Value to store
   */
  mdtValue(bool value);

  /*! \brief Clear values (set default values) and unset validity flag
   */
  void clear();

  /*! \brief Set default value for double format
   *
   * \param value Value to store as default
   */
  void setDefaultValue(double value);

  /*! \brief Set default value for int format
   *
   * \param value Value to store as default
   */
  void setDefaultValue(int value);

  /*! \brief Set default value for bool format
   *
   * \param value Value to store as default
   */
  void setDefaultValue(bool value);

  /*! \brief Set value with double format
   *
   * \param value Value to store
   * \param isMinusOl -OL flag
   * \param isPlusOl +OL flag
   */
  void setValue(double value, bool isMinusOl = false, bool isPlusOl = false);

  /*! \brief Check if value as double format is available
   */
  bool hasValueDouble() const;

  /*! \brief Get the value stored in double format
   */
  double valueDouble() const;

  /*! \brief Set value with int format
   *
   * \param value Value to store
   * \param isMinusOl -OL flag
   * \param isPlusOl +OL flag
   */
  void setValue(int value, bool isMinusOl = false, bool isPlusOl = false);

  /*! \brief Check if value as int format is available
   */
  bool hasValueInt() const;

  /*! \brief Get the value stored in int format
   */
  int valueInt() const;

  /*! \brief Check if value is -OL
   */
  bool isMinusOl() const;

  /*! \brief Check if value is +OL
   */
  bool isPlusOl() const;

  /*! \brief Set value with bool format
   *
   * \param value Value to store
   */
  void setValue(bool value);

  /*! \brief Check if value as bool format is available
   */
  bool hasValueBool() const;

  /*! \brief Get the value stored in bool format
   */
  bool valueBool() const;

  /*! \brief Get the validity flag
   *
   * A mdtValue is considered valid if one value
   *  was set (double, int or bool).
   *  After a call of clear(), the validity flag becomes
   *  false.
   *
   * Note: if -OL or +OL flag is set,
   *  the value is considered as valid.
   */
  bool isValid() const;

  /*! \brief Comparaison operator
   *
   * If all members are equal (values and flags), true is returned.
   */
  bool operator==(const mdtValue &other) const;

  /*! \brief Comparaison operator
   */
  bool operator!=(const mdtValue &other) const;

 private:

  // Values
  double pvValueDouble;
  int pvValueInt;
  bool pvValueBool;
  // Default values
  double pvDefaultValueDouble;
  int pvDefaultValueInt;
  bool pvDefaultValueBool;
  // Flags
  bool pvIsValid;
  bool pvHasValueDouble;
  bool pvIsMinusOl;
  bool pvIsPlusOl;
  bool pvHasValueInt;
  bool pvHasValueBool;
};

QDebug operator<<(QDebug dbg, const mdtValue &value);

#endif  // #ifndef MDT_VALUE_H
