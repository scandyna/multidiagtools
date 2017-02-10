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
#ifndef MDT_ITEM_MODEL_RELATION_FILTER_EXPRESSION_H
#define MDT_ITEM_MODEL_RELATION_FILTER_EXPRESSION_H

#include "Expression/RelationFilterExpressionGrammar.h"
#include "Expression/FilterExpressionContainer.h"
#include "RelationKey.h"
#include "RelationColumnPair.h"
#include <Qt>
#include <boost/proto/matches.hpp>
#include <memory>

class QAbstractItemModel;

namespace Mdt{ namespace ItemModel{

  namespace Expression {
    class ContainerInterface;
  }

  /*! \brief Filter expression used in RelationFilterProxyModel
   *
   * Example of usage:
   * \code
   * #include "Mdt/ItemModel/RelationFilterExpression.h"
   *
   * using Mdt::ItemModel::RelationFilterExpression;
   * using Mdt::ItemModel::FilterColumn;
   * using Mdt::ItemModel::ParentModelColumn;
   * using Like = Mdt::ItemModel::LikeExpression;
   *
   * ParentModelColumn clientId(0);
   * FilterColumn addressClientId(1);
   *
   * RelationFilterExpression filter( addressClientId == clientId );
   * \endcode
   */
  class RelationFilterExpression
  {
   public:

    /*! \brief Construct a empty expression
     */
    RelationFilterExpression() = default;

    /*! \brief Construct a expression
     *
     * \tparam Expr Type of the expression.
     * \param expr Expression to hold.
     * \pre Expr must be a relation filter expression type.
     *       A relation filter expression is based on FilterColumn, ParentModelColumn,
     *       comparison operators, logical AND, logical OR.
     *       For example (see example code above for details):
     *       \code
     *       // Example of valid relation filter expression
     *       addressClientId == cliendId
     *       (addressClientId == cliendId) && (cliendId > 25)
     *
     *       // Example of invalid relation filter expression
     *       cliendId + 5
     *       \endcode
     */
    template<typename Expr>
    RelationFilterExpression(const Expr & expr)
     : mContainer( new Expression::FilterExpressionContainer< typename boost::proto::result_of::deep_copy<Expr>::type >(expr) )
    {
      static_assert( boost::proto::matches<Expr, Expression::RelationFilterExpressionGrammar>::value , "Type of expr is not a valid relation filter expression." );
      setGreatestColumns();
    }

    // Destructor must have its implementation in cpp file
    ~RelationFilterExpression();

    /*! \brief Construct a expression as copy of other
     */
    RelationFilterExpression(const RelationFilterExpression & other) = default;

    /*! \brief Construct a expression by moving other
     */
    RelationFilterExpression(RelationFilterExpression && other) = default;

    /*! \brief Assign other expression to this
     */
    RelationFilterExpression & operator=(const RelationFilterExpression & other) = default;

    /*! \brief Assign other expression to this
     */
    RelationFilterExpression & operator=(RelationFilterExpression &&) = default;

    /*! \brief Check if this expression is null
     */
    bool isNull() const
    {
      return !mContainer;
    }

    /*! \brief Evaluate if row matches stored expression in model
     *
     * \pre this expression must not be null
     * \pre \a model must be a valid pointer (not null)
     * \pre \a row must be in valid range ( 0 <= row < model->rowCount() )
     * \pre \a parentModelData must not be null
     */
    bool eval(const QAbstractItemModel*const model, int row, const Expression::ParentModelEvalData & parentModelData, Qt::CaseSensitivity caseSensitivity) const;

    /*! \brief Get a relation key that contains pairs of equly compared columns of this expression
     *
     * For example, this expression:
     * \code
     * ParentModelColumn clientId(0);
     * ChildModelColumn addressClientId(1);
     * addressClientId == cliendId;
     * \endcode
     * The resulting relation key will contain 1 column pair
     *  with parent model column equal to 0
     *  and child model column equal to 1
     *
     * Lets take a more complex expression:
     * \code
     * ChildModelColumn(1) <= ParentModelColumn(11) && ChildModelColumn(2) < ParentModelColumn(12);
     * \endcode
     * The resulting relation key will only contain 1 column pair,
     *  with parent model column equal to 11
     *  and child model column equal to 1,
     *  because ChildModelColumn(2) < ParentModelColumn(12) is not a equality.
     *
     * \pre this expression must not be null
     */
    RelationKey getRelationKeyForEquality() const;

    /*! \brief Get a relation filter expression from a relation key
     *
     * \pre \a relationKey must not be null
     * \pre \a relationKey must contain max. 4 pairs of columns
     */
    static RelationFilterExpression fromRelationKey(const RelationKey & relationKey);

    /*! \brief Get the greatest column in this expression
     */
    int greatestColumn() const
    {
      return mGreatestColumn;
    }

    /*! \brief Get the greatest parent model column in this expression
     */
    int greatestParentModelColumn() const
    {
      return mGreatestParentModelColumn;
    }

   private:

    void setGreatestColumns();

    static RelationFilterExpression fromColumnPairs(RelationColumnPair pair1);
    static RelationFilterExpression fromColumnPairs(RelationColumnPair pair1, RelationColumnPair pair2);
    static RelationFilterExpression fromColumnPairs(RelationColumnPair pair1, RelationColumnPair pair2, RelationColumnPair pair3);
    static RelationFilterExpression fromColumnPairs(RelationColumnPair pair1, RelationColumnPair pair2, RelationColumnPair pair3, RelationColumnPair pair4);

    std::shared_ptr<const Expression::ContainerInterface> mContainer;
    int mGreatestColumn = -1;
    int mGreatestParentModelColumn = -1;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_RELATION_FILTER_EXPRESSION_H
