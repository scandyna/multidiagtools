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
#ifndef MDT_ITEM_MODEL_FILTER_EXPRESSION_H
#define MDT_ITEM_MODEL_FILTER_EXPRESSION_H

#include "Expression/FilterExpressionGrammar.h"
#include "Expression/FilterExpressionContainer.h"
#include <Qt>
#include <boost/proto/matches.hpp>
#include <memory>

class QAbstractItemModel;

namespace Mdt{ namespace ItemModel{

  namespace Expression {
    class ContainerInterface;
  }

  /*! \brief Filter expression used in FilterProxyModel
   *
   * Example of usage:
   * \code
   * #include "Mdt/ItemModel/FilterExpression.h"
   *
   * using Mdt::ItemModel::FilterExpression;
   * using Mdt::ItemModel::FilterColumn;
   * using Like = Mdt::ItemModel::LikeExpression;
   *
   * FilterExpression filter;
   * FilterColumn clientId(0);
   * FilterColumn clientName(2);
   *
   * filter.setExpression( clientId > 12 && clientName == Like("M?t") );
   * \endcode
   */
  class FilterExpression
  {
   public:

    /*! \brief Construct a empty expression
     */
    FilterExpression() = default;

    // Destructor must have its implementation in cpp file
    ~FilterExpression();

    /*! \brief Construct a expression as copy of other
     */
    FilterExpression(const FilterExpression & other) = default;

    /*! \brief Construct a expression by moving other
     */
    FilterExpression(FilterExpression && other) = default;

    /*! \brief Assign other expression to this
     */
    FilterExpression & operator=(const FilterExpression & other) = default;

    /*! \brief Assign other expression to this
     */
    FilterExpression & operator=(FilterExpression &&) = default;

    /*! \brief Set expression
     *
     * \tparam Expr Type of the expression.
     * \param expr Expression to hold.
     * \pre Expr must be a filter expression type.
     *       A filter expression is based on FilterColumn,
     *       comparison operators, logical AND, logical OR.
     *       For example (see example code above for details):
     *       \code
     *       // Example of valid filter expression
     *       cliendId == 25
     *       (cliendId == 25) && (clientName != "A")
     *
     *       // Example of invalid filter expression
     *       cliendId + 5
     *       \endcode
     */
    template<typename Expr>
    void setExpression(const Expr & expr)
    {
      static_assert( boost::proto::matches<Expr, Expression::FilterExpressionGrammar>::value , "Type of expr is not a valid filter expression." );
      mContainer.reset( new Expression::FilterExpressionContainer< typename boost::proto::result_of::deep_copy<Expr>::type >(expr) );
    }

    /*! \brief Check if this expression is null
     */
    bool isNull() const
    {
      return !mContainer;
    }

    /*! \brief Evaluate if row matches stored expression in model
     *
     * \pre this expression must not be null
     * \pre model must be a valid pointer (not null)
     * \pre must be in valid range ( 0 <= row < model->rowCount() )
     */
    bool eval(const QAbstractItemModel * const model, int row, Qt::CaseSensitivity caseSensitivity) const;

   private:

    std::shared_ptr<const Expression::ContainerInterface> mContainer;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_FILTER_EXPRESSION_H
