/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_NUMERIC_PHYSICS_TYPE_H
#define MDT_NUMERIC_PHYSICS_TYPE_H

#include "Double.h"

namespace Mdt{ namespace Numeric{

  /*! \brief Base class for physics type
   *
   * PhysicsType offers some common methods
   *  to create value types
   *  using static polymorphism (CRTP).
   *
   */
  template<typename Derived>
  class PhysicsType
  {
   public:

    /*! \brief Construct a null value
     */
    constexpr PhysicsType() noexcept = default;

    /*! \brief Construct a value
     */
    explicit constexpr PhysicsType(Double value) noexcept
     : pvValue(value) {}

    /*! \brief Get value
     */
    constexpr Double value() const noexcept
    {
      return pvValue;
    }

    /*! \brief Check if value is null
     */
    constexpr bool isNull() const noexcept
    {
      return pvValue.isNull();
    }

    /*! \brief Equality comparison operator
     */
    friend
    constexpr bool operator==(const Derived & x, const Derived & y) noexcept
    {
      return (x.value() == y.value());
    }

    /*! \brief Inequality comparison operator
     *
     * \sa operator==()
     */
    friend
    constexpr bool operator!=(const Derived & x, const Derived & y) noexcept
    {
      return !(x == y);
    }

    /*! \brief Comparison operator
     */
    friend
    constexpr bool operator<(const Derived & x, const Derived & y) noexcept
    {
      return (x.value() < y.value());
    }

    /*! \brief Comparison operator
     */
    friend
    constexpr bool operator<=(const Derived & x, const Derived & y) noexcept
    {
      return (x.value() <= y.value());
    }

    /*! \brief Comparison operator
     */
    friend
    constexpr bool operator>(const Derived & x, const Derived & y) noexcept
    {
      return (x.value() > y.value());
    }

    /*! \brief Comparison operator
     */
    friend
    constexpr bool operator>=(const Derived & x, const Derived & y) noexcept
    {
      return (x.value() >= y.value());
    }

    /*! \brief Addition operator
     */
    friend
    constexpr Derived operator+(const Derived & x, const Derived & y) noexcept
    {
      return Derived( x.value() + y.value() );
    }

    /*! \brief Substraction operator
     */
    friend
    constexpr Derived operator-(const Derived & x, const Derived & y) noexcept
    {
      return Derived( x.value() - y.value() );
    }

    /*! \brief Negation operator
     */
    constexpr Derived operator-() const
    {
      return Derived( -pvValue );
    }

    /*! \brief Multiplication operator
     */
    friend
    constexpr Derived operator*(const Derived & x, const Derived & y) noexcept
    {
      return Derived( x.value() * y.value() );
    }

    /*! \brief Division operator
     */
    friend
    constexpr Derived operator/(const Derived & x, const Derived & y) noexcept
    {
      return Derived( x.value() / y.value() );
    }

    /*! \brief Clear value
     */
    constexpr void clear() noexcept
    {
      pvValue.clear();
    }

   private:

    Double pvValue;
  };

}} // namespace Mdt{ namespace Numeric{

#endif // #ifndef MDT_NUMERIC_PHYSICS_TYPE_H
