/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_QUERY_EXPRESSION_FILTER_EXPRESSION_H
#define MDT_QUERY_EXPRESSION_FILTER_EXPRESSION_H

#include "ExpressionTree.h"
#include "ExpressionGrammar.h"
#include "TypeTraits/FilterExpression.h"
#include "BuildExpressionTree/Transform.h"
#include "MdtQueryExpression_CoreExport.h"

namespace Mdt{ namespace QueryExpression{

  /*! \brief Filter expression
   *
   * Example of usage:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * SelectField name( FieldName("name") );
   * FilterExpression filter;
   *
   * filter.setFilter( (name == "A") || (name == "B") );
   * \endcode
   *
   * \sa SelectQuery
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT FilterExpression
  {
   public:

    /*! \brief Construct a null filter expression
     */
    FilterExpression() = default;

    /*! \brief Copy construct a filter expression from \a other
     */
    FilterExpression(const FilterExpression & other) = default;

    /*! \brief Copy assign \a other to this filter expression
     */
    FilterExpression & operator=(const FilterExpression & other) = default;

    /*! \brief Move construct a filter expression from \a other
     */
    FilterExpression(FilterExpression && other) = default;

    /*! \brief Move assign \a other to this filter expression
     */
    FilterExpression & operator=(FilterExpression && other) = default;

    /*! \brief Set \a expr to this filter expression
     */
    template<typename Expr>
    void setFilter(const Expr & expr)
    {
      static_assert(TypeTraits::isFilterExpression<Expr>() , "expr is not a valid filter expression");

      mTree.clear();
      BuildExpressionTree::LogicalTransform transform;
      transform(expr, 0, mTree);
    }

    /*! \brief Check if this filter expression is null
     */
    bool isNull() const noexcept
    {
      return mTree.isNull();
    }

    /*! \brief Clear this filter expression
     */
    void clear();

    /*! \brief Access the internal tree of this filter expression
     *
     * This is typically used to transform this expression
     *  (for example to a SQL string)
     */
    const ExpressionTree & internalTree() const
    {
      return mTree;
    }

   private:

    ExpressionTree mTree;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_FILTER_EXPRESSION_H
