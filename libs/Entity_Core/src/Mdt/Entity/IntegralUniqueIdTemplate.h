/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ENTITY_INTEGRAL_UNIQUE_ID_TEMPLATE_H
#define MDT_ENTITY_INTEGRAL_UNIQUE_ID_TEMPLATE_H

#include <QtGlobal>
#include <QVariant>
#include <type_traits>

namespace Mdt{ namespace Entity{

  /*! \brief Class template to create a integral unique identifier
   *
   * \code
   * #include "Mdt/Entity/IntegralUniqueIdTemplate.h"
   *
   * class ArticleId : public Mdt::Entity::IntegralUniqueIdTemplate<>
   * {
   * public:
   *
   *   using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
   * };
   * \endcode
   */
  template<typename IntegralType = qulonglong>
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
    constexpr IntegralUniqueIdTemplate(IntegralType id) noexcept
     : mId(id)
    {
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
    constexpr bool operator==(const IntegralUniqueIdTemplate & a, const IntegralUniqueIdTemplate & b) noexcept
    {
      return a.value() == b.value();
    }

    /*! \brief Compare id a and b
     */
    friend
    constexpr bool operator!=(IntegralUniqueIdTemplate a, IntegralUniqueIdTemplate b) noexcept
    {
      return a.value() != b.value();
    }

    /*! \brief Compare id a and b
     */
    friend
    constexpr bool operator<(IntegralUniqueIdTemplate a, IntegralUniqueIdTemplate b) noexcept
    {
      return a.value() < b.value();
    }

    /*! \brief Compare id a and b
     */
    friend
    constexpr bool operator<=(IntegralUniqueIdTemplate a, IntegralUniqueIdTemplate b) noexcept
    {
      return a.value() <= b.value();
    }

    /*! \brief Compare id a and b
     */
    friend
    constexpr bool operator>(IntegralUniqueIdTemplate a, IntegralUniqueIdTemplate b) noexcept
    {
      return a.value() > b.value();
    }

    /*! \brief Compare id a and b
     */
    friend
    constexpr bool operator>=(IntegralUniqueIdTemplate a, IntegralUniqueIdTemplate b) noexcept
    {
      return a.value() >= b.value();
    }

    /*! \brief Construct a id from \a value
     */
    static IntegralUniqueIdTemplate fromQVariant(const QVariant & value)
    {
      Q_ASSERT(value.canConvert<value_type>());

      return IntegralUniqueIdTemplate( value.value<value_type>() );
    }

   private:

    IntegralType mId = 0;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_INTEGRAL_UNIQUE_ID_TEMPLATE_H
