/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_NUMERIC_DOUBLE_H
#define MDT_NUMERIC_DOUBLE_H

#include <limits>
#include <cmath>

namespace Mdt{ namespace Numeric{

  /*! \brief Wraps floating (double) value with some helper functions
   */
  class Double
  {
  public:

    /*! \brief Construct a null Double
     */
    constexpr Double() noexcept
     : mValue(std::numeric_limits<double>::quiet_NaN()) {}

    /*! \brief Construct a Double with a value
     */
    constexpr Double(double x) noexcept
     : mValue(x) {}

    /*! \brief Get Double value
     *
     * Returns the value in standard double format.
     *  The name was choosen to be explicit,
     *  but no convertion is done.
     */
    constexpr double toDouble() const noexcept
    {
      return mValue;
    }

    /*! \brief Check if value is infinity (+infinity or -infinity)
     */
    bool isInfinity() const noexcept
    {
      return std::isinf(mValue);
    }

    /*! \brief Set -infinity
     */
    constexpr void setMinusInfinity() noexcept
    {
      mValue = -std::numeric_limits<double>::infinity();
    }

    /*! \brief Check if value is -infinity
     */
    bool isMinusInfinity() const noexcept
    {
      if(mValue > 0.0){
        return false;
      }
      return isInfinity();
    }

    /*! \brief Set +infinity
     */
    constexpr void setPlusInfinity() noexcept
    {
      mValue = std::numeric_limits<double>::infinity();
    }

    /*! \brief Check if value is +infinity
     */
    bool isPlusInfinity() const noexcept
    {
      if(mValue < 0.0){
        return false;
      }
      return isInfinity();
    }

    /*! \brief Check if value is NaN
     *
     * Internally, std::isnan() is used
     */
    bool isNaN() const noexcept
    {
      return std::isnan(mValue);
    }

    /*! \brief Check if Double is null
     *
     * \sa isNaN()
     */
    bool isNull() const noexcept
    {
      return isNaN();
    }

    /*! \brief Clear
     */
    constexpr void clear() noexcept
    {
      mValue = std::numeric_limits<double>::quiet_NaN();
    }

    /*! \brief Equality comparison operator
     *
     * The comparison is a approximation.
     *  x and y are considered equal if \f$ |x-y| < \epsilon \f$
     * 
     * Notes:
     *  - if one (or both) value is null, they are not considered equal (Same rule as IEEE 754 NaN)
     */
    friend
    bool operator==(const Double & x, const Double & y) noexcept
    {
      if(x.isNull() || y.isNull()){
        return false;
      }
      return (std::abs(x.toDouble() - y.toDouble()) < std::numeric_limits<double>::epsilon());
    }

    /*! \brief Inequality comparison operator
     *
     * \sa operator==()
     */
    friend
    bool operator!=(const Double & x, const Double & y) noexcept
    {
      return !(x == y);
    }

    /*! \brief Comparison operator
     *
     * Note: if one (or both) value is null, comparison will allways be false
     *       (Same rule as IEEE 754 NaN)
     */
    friend
    bool operator<(const Double & x, const Double & y) noexcept
    {
      return std::isless(x.toDouble(), y.toDouble());
    }

    /*! \brief Comparison operator
     *
     * Note: if one (or both) value is null, comparison will allways be false
     *       (Same rule as IEEE 754 NaN)
     */
    friend
    bool operator<=(const Double & x, const Double & y) noexcept
    {
      return std::islessequal(x.toDouble(), y.toDouble());
    }

    /*! \brief Comparison operator
     *
     * Note: if one (or both) value is null, comparison will allways be false
     *       (Same rule as IEEE 754 NaN)
     */
    friend
    bool operator>(const Double & x, const Double & y) noexcept
    {
      return std::isgreater(x.toDouble(), y.toDouble());
    }

    /*! \brief Comparison operator
     *
     * Note: if one (or both) value is null, comparison will allways be false
     *       (Same rule as IEEE 754 NaN)
     */
    friend
    bool operator>=(const Double & x, const Double & y) noexcept
    {
      return std::isgreaterequal(x.toDouble(), y.toDouble());
    }

    /*! \brief Addition operator
     *
     * Note:
     *  - If one (or both) value is null, a null value will be returned (Same rule as IEEE 754 NaN)
     *  - For all other cases, standard floating point arithmetic rules apply
     */
    friend
    constexpr Double operator+(const Double & x, const Double & y)
    {
      return x.toDouble() + y.toDouble();
    }

    /*! \brief Substraction operator
     *
     * Note:
     *  - If one (or both) value is null, a null value will be returned (Same rule as IEEE 754 NaN)
     *  - For all other cases, standard floating point arithmetic rules apply
     */
    friend
    constexpr Double operator-(const Double & x, const Double & y)
    {
      return x.toDouble() - y.toDouble();
    }

    /*! \brief Negation operator
     *
     * Note:
     *  - If current value is null, a null value will be returned (Same rule as IEEE 754 NaN)
     *  - For all other cases, standard floating point arithmetic rules apply
     */
    constexpr Double operator-() const
    {
      return -mValue;
    }

    /*! \brief Multiplication operator
     *
     * Note:
     *  - If one (or both) value is null, a null value will be returned (Same rule as IEEE 754 NaN)
     *  - For all other cases, standard floating point arithmetic rules apply
     */
    friend
    constexpr Double operator*(const Double & x, const Double & y)
    {
      return x.toDouble() * y.toDouble();
    }

    /*! \brief Division operator
     *
     * Note:
     *  - If one (or both) value is null, a null value will be returned (Same rule as IEEE 754 NaN)
     *  - For all other cases, standard floating point arithmetic rules apply
     */
    friend
    constexpr Double operator/(const Double & x, const Double & y)
    {
      return x.toDouble() / y.toDouble();
    }

  private:

    double mValue;
  };

}} // namespace Mdt{ namespace Numeric{

class QDebug;

/*! \brief Stream operator for QDebug
 */
QDebug operator<<(QDebug dbg, const Mdt::Numeric::Double &);

#endif // #ifndef MDT_NUMERIC_DOUBLE_H
