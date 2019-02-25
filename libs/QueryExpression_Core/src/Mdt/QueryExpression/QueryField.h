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
#ifndef MDT_QUERY_EXPRESSION_QUERY_FIELD_H
#define MDT_QUERY_EXPRESSION_QUERY_FIELD_H

#include "FieldAlias.h"
#include "EntityAlias.h"
#include "QueryEntity.h"
#include "SelectAllField.h"
#include "QueryFieldVariant.h"
#include "MdtQueryExpression_CoreExport.h"
#include <boost/proto/expr.hpp>
#include <boost/proto/extends.hpp>
#include <boost/proto/operators.hpp>
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \internal Tag for QueryField
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT QueryFieldTag
  {
  };

  /*! \brief Represents a field for a query expression
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT QueryField : public boost::proto::extends<
                                                        boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<QueryFieldVariant> >,
                                                        QueryField,
                                                        boost::proto::default_domain
                                                      >
  {
    using Domain = boost::proto::default_domain;
    using Expression = boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<QueryFieldVariant> >;
    using BaseClass = boost::proto::extends< Expression, QueryField, Domain >;

   public:

    using value_type = QueryFieldVariant;
    using reference = value_type &;
    using const_reference = const value_type &;

    /*! \brief Construct a null query field
     */
    QueryField() = default;

    /*! \brief Copy construct a query field from \a other
     */
    QueryField(const QueryField & other) = default;

    /*! \brief Copy assign \a other to this query field
     */
    QueryField & operator=(const QueryField & other) = default;

    /*! \brief Move construct a query field from \a other
     */
    QueryField(QueryField && other) noexcept = default;

    /*! \brief Move assign \a other to this query field
     */
    QueryField & operator=(QueryField && other) noexcept = default;

    /*! \brief Construct a select all field
     */
    QueryField(const SelectAllField & field)
     : BaseClass( Expression::make(field) )
    {
    }

    /*! \brief Construct a query field
     *
     * \pre \a fieldName must not be empty
     */
    QueryField(const QString & fieldName, const FieldAlias & fieldAlias = FieldAlias())
     : BaseClass( Expression::make( EntityAndField(fieldName, fieldAlias) ) )
    {
    }

    /*! \brief Construct a query field
     *
     * \pre \a entity must not be null
     * \pre \a fieldName must not be empty
     */
    QueryField(const QueryEntity & entity, const QString & fieldName, const FieldAlias & fieldAlias = FieldAlias())
     : BaseClass( Expression::make( EntityAndField(entity, fieldName, fieldAlias) ) )
    {
    }

    /*! \brief Check if this query entity is null
     */
    bool isNull() const
    {
      return internalVariant().isNull();
    }

    /*! \internal Access internal variant of this query field
     */
    const QueryFieldVariant & internalVariant() const
    {
      return boost::proto::value(*this);
    }

  };
}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_QUERY_FIELD_H
