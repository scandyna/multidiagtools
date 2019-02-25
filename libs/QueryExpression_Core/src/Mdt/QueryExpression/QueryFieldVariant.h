/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_QUERY_EXPRESSION_QUERY_FIELD_VARIANT_H
#define MDT_QUERY_EXPRESSION_QUERY_FIELD_VARIANT_H

#include "EntityAndField.h"
#include "SelectAllField.h"
#include "MdtQueryExpression_CoreExport.h"
#include <boost/variant.hpp>

namespace Mdt{ namespace QueryExpression{

  /*! \internal Null field tag for QueryFieldVariant
   */
  struct NullQueryField
  {
  };

  /*! \internal Data variant for QueryField
   *
   * \note Previously, this class was named SelectFieldVariant,
   *   and its first version was a simple alias:
   * \code
   * using SelectFieldVariant = boost::variant<SelectAllField, EntityAndField>;
   * \endcode
   *
   * This worked well until overload ambiguities came while passing SelectFieldVariant as arguments.
   * See: https://stackoverflow.com/questions/24791319/boostvariant-and-function-overload-resolution
   *
   * This is why QueryFieldVariant became a class using boost::variant
   */
  class QueryFieldVariant
  {
   public:

    using Variant = boost::variant<NullQueryField, SelectAllField, EntityAndField>;

    QueryFieldVariant() noexcept = default;

    QueryFieldVariant(const QueryFieldVariant &) = default;
    QueryFieldVariant & operator=(const QueryFieldVariant &) = default;

    QueryFieldVariant(QueryFieldVariant &&) noexcept = default;
    QueryFieldVariant & operator=(QueryFieldVariant &&) noexcept = default;

    QueryFieldVariant(const SelectAllField & field)
     : mVariant(field)
    {
    }

    QueryFieldVariant(SelectAllField && field) noexcept
     : mVariant(field)
    {
    }

    QueryFieldVariant(const EntityAndField & field)
     : mVariant(field)
    {
    }

    QueryFieldVariant(EntityAndField && field) noexcept
     : mVariant(field)
    {
    }

    bool isNull() const noexcept
    {
      return (boost::get<NullQueryField>(&mVariant) != nullptr);
    }

    const Variant & internalVariant() const
    {
      return mVariant;
    }

  private:

    Variant mVariant;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_QUERY_FIELD_VARIANT_H
