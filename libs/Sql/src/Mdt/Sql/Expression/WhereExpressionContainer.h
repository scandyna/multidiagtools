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
#ifndef MDT_SQL_EXPRESSION_WHERE_EXPRESSION_CONTAINER_H
#define MDT_SQL_EXPRESSION_WHERE_EXPRESSION_CONTAINER_H

#include "ExpressionContainerInterface.h"
#include "WhereExpressionSqlTransform.h"
#include <boost/proto/deep_copy.hpp>
#include <QString>
#include <QSqlDatabase>

namespace Mdt{ namespace Sql{ namespace Expression{

  /*! \brief Implementation of where expression container
   *
   * \tparam Expr Type of the expression to store
   */
  template<typename Expr>
  class WhereExpressionContainer : public ExpressionContainerInterface
  {
   public:

    /*! \brief Construct a container which stores given expression
     *
     * Because expr will be copied (with boost::proto::deep_copy()),
     *  the resuling expression is not the same as input expression.
     *  But, caller has not to care about this implementation detail.
     *
     * \tparam InExpr The expression to store
     */
    template<typename InExpr>
    WhereExpressionContainer(const InExpr & expr)
     : mExpression( boost::proto::deep_copy(expr) )
    {
    }

    // Copy disabled
    WhereExpressionContainer(const WhereExpressionContainer &) = delete;
    WhereExpressionContainer & operator=(const WhereExpressionContainer &) = delete;
    // Move disabled
    WhereExpressionContainer(WhereExpressionContainer &&) = delete;
    WhereExpressionContainer & operator=(WhereExpressionContainer &&) = delete;

    /*! \brief Get SQL string of the expression
     *
     * \pre db must be valid (must have a driver loaded)
     */
    QString toSql(const QSqlDatabase & db) const override
    {
      Q_ASSERT(db.isValid());
      WhereExpressionSqlTransform transform;
      return transform(mExpression, 0, db);
    }

   private:

    Expr mExpression;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Expression{

#endif // #ifndef MDT_SQL_EXPRESSION_WHERE_EXPRESSION_CONTAINER_H
