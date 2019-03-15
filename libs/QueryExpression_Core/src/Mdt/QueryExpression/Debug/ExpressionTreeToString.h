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
#ifndef MDT_QUERY_EXPRESSION_DEBUG_EXPRESSION_TREE_TO_STRING_H
#define MDT_QUERY_EXPRESSION_DEBUG_EXPRESSION_TREE_TO_STRING_H

#include "../ComparisonOperator.h"
#include "../LogicalOperator.h"
#include "../ExpressionTree.h"
#include "../FilterExpression.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{ namespace Debug{

  /*! \brief Get the string representation of a comparison operator
   */
  QString MDT_QUERYEXPRESSION_CORE_EXPORT comparisonOperatorToString(ComparisonOperator op);

  /*! \brief Get the string representation of a logical operator
   */
  QString MDT_QUERYEXPRESSION_CORE_EXPORT logicalOperatorToString(LogicalOperator op);

  /*! \brief Get a string representation of a expression tree
   *
   * This function can be used to see a expression tree as a string.
   *
   * Internally, it is used for unit tests.
   *
   * \note this function is not provided to create
   *   real query strings. For this, see if a library can help (like Mdt::Sql),
   *   or create transfoms using AbstractExpressionTreeVisitor
   *
   * \pre \a tree must not be null
   * \sa filterExpressionToInfixString()
   */
  QString MDT_QUERYEXPRESSION_CORE_EXPORT expressionTreeToInfixString(const ExpressionTree & tree);

  /*! \brief Get a string representation of a filter expression
   *
   * This function can be used to see a filter expression as a string.
   *
   * Internally, it is used for unit tests.
   *
   * A expression like:
   * \code
   * using namespace Mdt::QueryExpression;
   *
   * QueryEntity person("Person");
   * QueryEntity address( "Address", EntityAlias("ADR") );
   *
   * QueryField personId(person, "id");
   * QueryField addressPersonId(address, "personId");
   * QueryField addressStreet(address, "street");
   *
   * FilterExpression filter;
   * filter.setFilter( (addressPersonId == personId) && (addressStreet == "Some Street") );
   * QString str = filterExpressionToInfixString(filter);
   * \endcode
   *
   * Above code will return string like:
   * \code
   * (ADR.id==Person.id)&&(ADR.street==Some Street)
   * \endcode
   *
   * \note this function is not provided to create
   *   real query strings. For this, see if a library can help (like Mdt::Sql),
   *   or create transfoms using AbstractExpressionTreeVisitor
   *
   * \pre \a filter must not be null
   */
  QString MDT_QUERYEXPRESSION_CORE_EXPORT filterExpressionToInfixString(const FilterExpression & filter);

}}} // namespace Mdt{ namespace QueryExpression{ namespace Debug{

#endif // #ifndef MDT_QUERY_EXPRESSION_DEBUG_EXPRESSION_TREE_TO_STRING_H
