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
#ifndef MDT_VALUE_H
#define MDT_VALUE_H

#include <QMetaType>

/*! \brief Contains a floating (double) value with some attributes
 *
 * Most of cases, using standard C++ types are sufficent.
 *
 * When dealing with instruments, some additionnal flags are needed:
 *  - +OL, -OL: to tell, for example, that a measure is out of instrument's range.
 *  - Null: for example, if a error occured, a Null value could be returned (from a function).
 *
 * Note: Null and NaN is not the same.
 *       A Null mdtValueDouble contains a NaN value, but a non Null mdtValue can also contain a NaN value.
 *       For example, if a communication error occured, a function should return a Null value.
 */
class mdtValueDouble
{
 public:

  /*! \brief Construct a null value
   */
  mdtValueDouble() noexcept;

  /*! \brief Construct a value
   *
   * \param value Value to store
   * \param isOl Force OL flag to be set. If value is >= 0.0 , +OL is set, else -OL will be set
   */
  mdtValueDouble(double value, bool isOl = false) noexcept;

  /*! \brief Clear value
   *
   * After this call, value will be null
   */
  void clear() noexcept;

  /*! \brief Set value
   *
   * \param value Value to store
   * \param isOl Force OL flag to be set. If value is >= 0.0 , +OL is set, else -OL will be set
   */
  void setValue(double value, bool isOl = false) noexcept;

  /*! \brief Check if value is null
   */
  inline bool isNull() const noexcept
  {
    return pvIsNull;
  }

  /*! \brief Get value
   */
  inline double value() const noexcept
  {
    return pvValue;
  }

  /*! \brief Check if value is infinity (+infinity or -infinity)
   */
  bool isInfinity() const noexcept;

  /*! \brief Check if value is -infinity
   */
  bool isMinusInfinity() const noexcept;

  /*! \brief Check if value is +infinity
   */
  bool isPlusInfinity() const noexcept;

  /*! \brief Check if value is -OL
   */
  inline bool isMinusOl() const noexcept
  {
    return isMinusInfinity();
  }

  /*! \brief Check if value is +OL
   */
  inline bool isPlusOl() const noexcept
  {
    return isPlusInfinity();
  }

  /*! \brief Check if value is NaN
   */
  bool isNaN() const noexcept;

  /*! \brief Comparaison operator
   *
   * Notes:
   *  - Both values are equal if value and flags are equal.
   *  - if one (or both) value is null, they are not considered equal (Same rule as IEEE 754 NaN)
   */
  bool operator==(const mdtValueDouble & other) const noexcept;

  /*! \brief Comparaison operator
   */
  inline bool operator!=(const mdtValueDouble & other) const noexcept
  {
    return !(*this == other);
  }

  /*! \brief Comparison operator
   *
   * Note: if one (or both) value is null, comparison will allways be false
   *       (Same rule as IEEE 754 NaN)
   */
  /// \todo Use functions from cmath !
  bool operator<(const mdtValueDouble & other) const noexcept
  {
    if((pvIsNull)||(other.pvIsNull)){
      return false;
    }
    return (pvValue < other.pvValue);
  }

  /*! \brief Comparison operator
   *
   * Note: if one (or both) value is null, comparison will allways be false
   *       (Same rule as IEEE 754 NaN)
   */
  /// \todo Use functions from cmath !
  bool operator<=(const mdtValueDouble & other) const noexcept
  {
    if((pvIsNull)||(other.pvIsNull)){
      return false;
    }
    if(*this == other){
      return true;
    }
    return (pvValue <= other.pvValue);
  }

  /*! \brief Comparison operator
   *
   * Note: if one (or both) value is null, comparison will allways be false
   *       (Same rule as IEEE 754 NaN)
   */
  /// \todo Use functions from cmath !
  bool operator>(const mdtValueDouble & other) const noexcept
  {
    if((pvIsNull)||(other.pvIsNull)){
      return false;
    }
    return (pvValue > other.pvValue);
  }

  /*! \brief Comparison operator
   *
   * Note: if one (or both) value is null, comparison will allways be false
   *       (Same rule as IEEE 754 NaN)
   */
  /// \todo Use functions from cmath !
  bool operator>=(const mdtValueDouble & other) const noexcept
  {
    if((pvIsNull)||(other.pvIsNull)){
      return false;
    }
    if(*this == other){
      return true;
    }
    return (pvValue >= other.pvValue);
  }

  /*! \brief Addition operator
   *
   * Note:
   *  - If one (or both) value is null, a null value will be returned (Same rule as IEEE 754 NaN)
   *  - For all other cases, including OL flags (wich are stored as infinity values), follows standard floating point arithmetic rules
   * \todo Review if really noexcept
   */
  mdtValueDouble operator+(const mdtValueDouble & other) const noexcept
  {
    if((pvIsNull)||(other.pvIsNull)){
      return mdtValueDouble();
    }
    return mdtValueDouble(pvValue + other.pvValue);
  }

  /*! \brief Subtraction operator
   *
   * Note:
   *  - If one (or both) value is null, a null value will be returned (Same rule as IEEE 754 NaN)
   *  - For all other cases, including OL flags (wich are stored as infinity values), follows standard floating point arithmetic rules
   * \todo Review if really noexcept
   */
  mdtValueDouble operator-(const mdtValueDouble & other) const noexcept
  {
    if((pvIsNull)||(other.pvIsNull)){
      return mdtValueDouble();
    }
    return mdtValueDouble(pvValue - other.pvValue);
  }

  /*! \brief Negation operator
   *
   * Will return a new value witch is the negate version of this.
   *
   * Note:
   *  - If current value is null, a null value will be returned (Same rule as IEEE 754 NaN)
   *  - For all other cases, including OL flags (wich are stored as infinity values), follows standard floating point arithmetic rules
   * \todo Review if really noexcept
   */
  mdtValueDouble operator-() const noexcept
  {
    if(pvIsNull){
      return mdtValueDouble();
    }
    return mdtValueDouble(-pvValue);
  }

  /*! \brief Multiplication operator
   *
   * Note:
   *  - If one (or both) value is null, a null value will be returned (Same rule as IEEE 754 NaN)
   *  - For all other cases, including OL flags (wich are stored as infinity values), follows standard floating point arithmetic rules
   * \todo Review if really noexcept
   */
  mdtValueDouble operator*(const mdtValueDouble & other) const noexcept
  {
    if((pvIsNull)||(other.pvIsNull)){
      return mdtValueDouble();
    }
    return mdtValueDouble(pvValue * other.pvValue);
  }

  /*! \brief Division operator
   *
   * Note:
   *  - If one (or both) value is null, a null value will be returned (Same rule as IEEE 754 NaN)
   *  - For all other cases, including OL flags (wich are stored as infinity values), follows standard floating point arithmetic rules
   * \todo Review if really noexcept
   */
  mdtValueDouble operator/(const mdtValueDouble & other) const noexcept
  {
    if((pvIsNull)||(other.pvIsNull)){
      return mdtValueDouble();
    }
    return mdtValueDouble(pvValue / other.pvValue);
  }

private:

  double pvValue;
  bool pvIsNull;
};

/*! \brief Integral type with Null attribute
 *
 * Most of cases, using standard C++ types are sufficent.
 *  But, in some cases, its needed to test if a function
 *  returned a valid value or not.
 *  Here, the Null flag is used for that.
 */
template<typename T>
// requires std::integral_type<T>{}
class mdtValueIntegral
{
 public:

  /*! \brief Construct a null value
   */
  mdtValueIntegral()
   : pvValue(0),
     pvIsNull(true)
   {
   }

  /*! \brief Construct value
   */
  mdtValueIntegral(T value)
   : pvValue(value),
     pvIsNull(false)
   {
   }

  /*! \brief Clear value
   */
  void clear()
  {
    pvValue = 0;
    pvIsNull = true;
  }

  /*! \brief Check if value is null
   */
  bool isNull() const
  {
    return pvIsNull;
  }

  /*! \brief Set value
   */
  void setValue(T value)
  {
    pvIsNull = false;
    pvValue = value;
  }

  /*! \brief Get value
   */
  T value() const
  {
    return pvValue;
  }

  /*! \brief Comparaison operator
   *
   * Note: if one (or both) value is null, they are not considered equal
   *       (Same rule as IEEE 754 NaN)
   */
  bool operator==(const mdtValueIntegral & other) const
  {
    if((pvIsNull)||(other.pvIsNull)){
      return false;
    }
    return (pvValue == other.pvValue);
  }

  /*! \brief Comparaison operator
   */
  inline bool operator!=(const mdtValueIntegral & other) const
  {
    return !(*this == other);
  }

  /*! \brief Comparison operator
   *
   * Note: if one (or both) value is null, comparison will allways be false
   *       (Same rule as IEEE 754 NaN)
   */
  bool operator<(const mdtValueIntegral & other) const
  {
    if((pvIsNull)||(other.pvIsNull)){
      return false;
    }
    return (pvValue < other.pvValue);
  }

  /*! \brief Comparison operator
   *
   * Note: if one (or both) value is null, comparison will allways be false
   *       (Same rule as IEEE 754 NaN)
   */
  bool operator<=(const mdtValueIntegral & other) const
  {
    if((pvIsNull)||(other.pvIsNull)){
      return false;
    }
    return (pvValue <= other.pvValue);
  }

  /*! \brief Comparison operator
   *
   * Note: if one (or both) value is null, comparison will allways be false
   *       (Same rule as IEEE 754 NaN)
   */
  bool operator>(const mdtValueIntegral & other) const
  {
    if((pvIsNull)||(other.pvIsNull)){
      return false;
    }
    return (pvValue > other.pvValue);
  }

  /*! \brief Comparison operator
   *
   * Note: if one (or both) value is null, comparison will allways be false
   *       (Same rule as IEEE 754 NaN)
   */
  bool operator>=(const mdtValueIntegral & other) const
  {
    if((pvIsNull)||(other.pvIsNull)){
      return false;
    }
    return (pvValue >= other.pvValue);
  }

  /*! \brief Addition operator
   *
   * Note: if one (or both) value is null, a null value will be returned
   *       (Same rule as IEEE 754 NaN)
   */
  mdtValueIntegral operator+(const mdtValueIntegral & other) const
  {
    if((pvIsNull)||(other.pvIsNull)){
      return mdtValueIntegral();
    }
    return mdtValueIntegral(pvValue + other.pvValue);
  }

  /*! \brief Soustraction operator
   *
   * Note: if one (or both) value is null, a null value will be returned
   *       (Same rule as IEEE 754 NaN)
   */
  mdtValueIntegral operator-(const mdtValueIntegral & other) const
  {
    if((pvIsNull)||(other.pvIsNull)){
      return mdtValueIntegral();
    }
    return mdtValueIntegral(pvValue - other.pvValue);
  }

  /*! \brief Multiplication operator
   *
   * Note: if one (or both) value is null, a null value will be returned
   *       (Same rule as IEEE 754 NaN)
   */
  mdtValueIntegral operator*(const mdtValueIntegral & other) const
  {
    if((pvIsNull)||(other.pvIsNull)){
      return mdtValueIntegral();
    }
    return mdtValueIntegral(pvValue * other.pvValue);
  }

 private:

  T pvValue;
  bool pvIsNull;
};

/*! \brief mdtValueIntegral with int type value
 */
typedef mdtValueIntegral<int> mdtValueInt;

/*! \brief Boolean type with Null attribute
 *
 * Most of cases, using standard C++ types are sufficent.
 *  But, in some cases, its needed to test if a function
 *  returned a valid value or not.
 *  Here, the Null flag is used for that.
 */
class mdtValueBool
{
 public:

  /*! \brief Construct a null boolean
   */
  mdtValueBool()
   : pvValue(false),
     pvIsNull(true)
   {
   }

  /*! \brief Construct boolean
   */
  mdtValueBool(bool value)
   : pvValue(value),
     pvIsNull(false)
   {
   }

  /*! \brief Clear boolean
   */
  void clear()
  {
    pvValue = false;
    pvIsNull = true;
  }

  /*! \brief Check if boolean is null
   */
  bool isNull() const
  {
    return pvIsNull;
  }

  /*! \brief Set value
   */
  void setValue(bool value)
  {
    pvIsNull = false;
    pvValue = value;
  }

  /*! \brief Get boolean's value
   */
  inline bool value() const
  {
    return pvValue;
  }

  /*! \brief Check equality
   *
   * If this object is null,
   *  this function returns allways false.
   */
  bool operator==(bool b) const
  {
    if(pvIsNull){
      return false;
    }
    return (pvValue == b);
  }

  /*! \brief Check inequality
   *
   * If this object is null,
   *  this function returns allways true.
   */
  bool operator!=(bool b) const
  {
    if(pvIsNull){
      return true;
    }
    return (pvValue != b);
  }

  /*! \brief Check equality
   *
   * If this one of the value is null,
   *  this function returns allways false.
   */
  bool operator==(const mdtValueBool & other) const
  {
    if( (pvIsNull) || (other.pvIsNull) ){
      return false;
    }
    return (pvValue == other.pvValue);
  }

  /*! \brief Check inequality
   *
   * If this one of the value is null,
   *  this function returns allways false.
   */
  bool operator!=(const mdtValueBool & other) const
  {
    return !(*this == other);
  }

 private:

  bool pvValue;
  bool pvIsNull;
};

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

Q_DECLARE_METATYPE(mdtValueDouble)
Q_DECLARE_METATYPE(mdtValue)

QDebug operator<<(QDebug dbg, const mdtValueBool & value);
QDebug operator<<(QDebug dbg, const mdtValueInt & value);
QDebug operator<<(QDebug dbg, const mdtValueDouble & value);
QDebug operator<<(QDebug dbg, const mdtValue & value);

#endif  // #ifndef MDT_VALUE_H
