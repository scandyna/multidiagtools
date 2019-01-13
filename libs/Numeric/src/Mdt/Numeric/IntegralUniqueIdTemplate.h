/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_NUMERIC_INTEGRAL_UNIQUE_ID_TEMPLATE_H
#define MDT_NUMERIC_INTEGRAL_UNIQUE_ID_TEMPLATE_H

#include <QtGlobal>
#include <QVariant>
#include <type_traits>

namespace Mdt{ namespace Numeric{

  /*! \brief Class template to create a integral unique identifier
   *
   * \code
   * #include "Mdt/Numeric/IntegralUniqueIdTemplate.h"
   *
   * class ArticleId : public Mdt::Numeric::IntegralUniqueIdTemplate<ArticleId>
   * {
   *  public:
   *
   *   using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
   * };
   * \endcode
   */
  template<typename Derived, typename IntegralType = qlonglong>
  class IntegralUniqueIdTemplate
  {
    static_assert( std::is_integral<IntegralType>::value, "IntegralType must be a integral type" );

   public:

    /*! \brief STL compatible value type this unique id holds
     */
    using value_type = IntegralType;

    /*! \brief Construct a null id
     */
    constexpr IntegralUniqueIdTemplate() noexcept = default;

    /*! \brief Copy construct a id from \a other
     */
    constexpr explicit IntegralUniqueIdTemplate(const IntegralUniqueIdTemplate & other) noexcept = default;

    /*! \brief Copy assign \a other to this unique id
     */
    constexpr IntegralUniqueIdTemplate & operator=(const IntegralUniqueIdTemplate & other) noexcept = default;

    /*! \brief Move construct a id from \a other
     */
    constexpr IntegralUniqueIdTemplate(IntegralUniqueIdTemplate && other) noexcept = default;

    /*! \brief Move assign \a other to this unique id
     */
    constexpr IntegralUniqueIdTemplate & operator=(IntegralUniqueIdTemplate && other) noexcept = default;

    /*! \brief Construct a id
     */
    constexpr explicit IntegralUniqueIdTemplate(IntegralType id) noexcept
     : mId(id)
    {
      static_assert( std::is_base_of< IntegralUniqueIdTemplate, Derived >::value, "Derived must derive from IntegralUniqueIdTemplate" );
    }

    /*! \brief Check if this id is null
     *
     * This id is null if its value equals 0
     */
    constexpr bool isNull() const noexcept
    {
      return (mId == 0);
    }

    /*! \brief Get the value of this id
     */
    constexpr IntegralType value() const noexcept
    {
      return mId;
    }

    /*! \brief Compare id a and b
     */
    friend
    constexpr bool operator==(const Derived & a, const Derived & b) noexcept
    {
      return a.value() == b.value();
    }

    /*! \brief Compare id a and b
     */
    friend
    constexpr bool operator!=(Derived a, Derived b) noexcept
    {
      return a.value() != b.value();
    }

    /*! \brief Compare id a and b
     */
    friend
    constexpr bool operator<(Derived a, Derived b) noexcept
    {
      return a.value() < b.value();
    }

    /*! \brief Compare id a and b
     */
    friend
    constexpr bool operator<=(Derived a, Derived b) noexcept
    {
      return a.value() <= b.value();
    }

    /*! \brief Compare id a and b
     */
    friend
    constexpr bool operator>(Derived a, Derived b) noexcept
    {
      return a.value() > b.value();
    }

    /*! \brief Compare id a and b
     */
    friend
    constexpr bool operator>=(Derived a, Derived b) noexcept
    {
      return a.value() >= b.value();
    }

    /*! \brief Get a QVariant from this id
     */
    QVariant toQVariant() const
    {
      return QVariant(mId);
    }

    /*! \brief Construct a id from \a value
     */
    static Derived fromQVariant(const QVariant & value)
    {
      if(value.isNull()){
        return Derived();
      }
      Q_ASSERT(value.canConvert<value_type>());

      return Derived( value.value<value_type>() );
    }

   private:

    IntegralType mId = 0;
  };

}} // namespace Mdt{ namespace Numeric{

#endif // #ifndef MDT_NUMERIC_INTEGRAL_UNIQUE_ID_TEMPLATE_H
