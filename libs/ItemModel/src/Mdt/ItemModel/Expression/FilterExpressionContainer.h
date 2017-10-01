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
#ifndef MDT_ITEM_MODEL_EXPRESSION_FILTER_EXPRESSION_CONTAINER_H
#define MDT_ITEM_MODEL_EXPRESSION_FILTER_EXPRESSION_CONTAINER_H

#include "ContainerInterface.h"
#include "FilterEval.h"
#include "FilterEvalData.h"
#include "GetRelationKeyForEquality.h"
#include "GreatestColumnTransform.h"
#include <QAbstractItemModel>
#include <Qt>
#include <boost/proto/deep_copy.hpp>
#include <boost/proto/eval.hpp>

namespace Mdt{ namespace ItemModel{ namespace Expression{

  /*! \brief Implementation of container for FilterExpression
   */
  template<typename StoredExpr>
  class FilterExpressionContainer : public ContainerInterface
  {
   public:

    /*! \brief Construct a container which stores given expression
     *
     * Because expr will be copied (with boost::proto::deep_copy()),
     *  the resuling expression is not the same as input expression.
     *  But, caller has not to care about this implementation detail.
     *
     * \tparam Expr The expression to store
     */
    template<typename Expr>
    FilterExpressionContainer(const Expr & expr)
     : mExpression( boost::proto::deep_copy(expr) )
    {
    }

    // Constructors / destructors
    FilterExpressionContainer() = default;
    virtual ~FilterExpressionContainer() = default;
    // Copy disabled
    FilterExpressionContainer(const FilterExpressionContainer &) = delete;
    FilterExpressionContainer & operator=(const FilterExpressionContainer &) = delete;
    // Move disabled
    FilterExpressionContainer(FilterExpressionContainer &&) = delete;
    FilterExpressionContainer & operator=(FilterExpressionContainer &&) = delete;

    /*! \brief Evaluate if row matches stored expression in model
     *
     * \pre \a model must be a valid pointer (not null)
     * \pre \a must be in valid range ( 0 <= row < model->rowCount() )
     */
    bool eval(const QAbstractItemModel * const model, int row, Qt::CaseSensitivity cs) const override
    {
      Q_ASSERT(model != nullptr);
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < model->rowCount());

      FilterEval ev;
      return ev(mExpression, 0, FilterEvalData(model, row, cs));
    }

    /*! \brief Evaluate if row matches stored expression in model
     *
     * \pre \a model must be a valid pointer (not null)
     * \pre \a row must be in valid range ( 0 <= row < model->rowCount() )
     * \pre \a parentModelData must not be null
     */
    bool eval(const QAbstractItemModel*const model, int row, const ParentModelEvalData& parentModelData, Qt::CaseSensitivity cs) const override
    {
      Q_ASSERT(model != nullptr);
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < model->rowCount());
      Q_ASSERT(!parentModelData.isNull());

      FilterEval ev;
      return ev(mExpression, 0, FilterEvalData(model, row, parentModelData, cs));
    }

    /*! \brief Get a relation key that contains pais of equly compared columns of this expression
     */
    RelationKey getRelationKeyForEquality() const override
    {
      GetRelationKeyForEqualityContext ctx;

      boost::proto::eval(mExpression, ctx);

      return ctx.relationKey();
    }

    /*! \brief Get the greatest column in this expression
     */
    int getGreatestColumn() const override
    {
      GetGreatestColumnContext ctx;

      boost::proto::eval(mExpression, ctx);

      return ctx.greatestColumn();
    }

    /*! \brief Get the greatest parent model column in this expression
     */
    int getGreatestParentModelColumn() const override
    {
      GetGreatestColumnContext ctx;

      boost::proto::eval(mExpression, ctx);

      return ctx.greatestParentModelColumn();
    }

   private:

    StoredExpr mExpression;
  };

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{

#endif // #ifndef MDT_ITEM_MODEL_EXPRESSION_FILTER_EXPRESSION_CONTAINER_H
