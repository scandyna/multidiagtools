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

#include "Mdt/Numeric/IntegralUniqueIdTemplate.h"
#include <QtGlobal>
#include <QVariant>
#include <type_traits>

namespace Mdt{ namespace Entity{

  /*! \brief Class template to create a integral unique identifier
   *
   * \deprecated Use Mdt::Numeric::IntegralUniqueIdTemplate
   *
   * \code
   * #include "Mdt/Entity/IntegralUniqueIdTemplate.h"
   *
   * class ArticleId : public Mdt::Entity::IntegralUniqueIdTemplate<ArticleId>
   * {
   * public:
   *
   *   using IntegralUniqueIdTemplate::IntegralUniqueIdTemplate;
   * };
   * \endcode
   */
  template<typename Derived, typename IntegralType = qulonglong>
  using IntegralUniqueIdTemplate = Mdt::Numeric::IntegralUniqueIdTemplate<Derived, IntegralType>;

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_INTEGRAL_UNIQUE_ID_TEMPLATE_H
