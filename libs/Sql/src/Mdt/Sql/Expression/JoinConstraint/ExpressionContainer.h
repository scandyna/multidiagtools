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
#ifndef MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_EXPRESSION_CONTAINER_H
#define MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_EXPRESSION_CONTAINER_H

#include "Mdt/Sql/Expression/ExpressionContainerInterface.h"
#include <boost/proto/deep_copy.hpp>
#include <QString>
#include <QSqlDatabase>

#include "Mdt/Sql/JoinConstraintField.h"

#include <boost/proto/proto.hpp>

namespace Mdt{ namespace Sql{ namespace Expression{ namespace JoinConstraint{

  /*! \brief Implementation of join constraint expression container
   *
   * \tparam Type of the expression to store
   */
  template<typename Expr>
  class ExpressionContainer : public ExpressionContainerInterface
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
    ExpressionContainer(const InExpr & expr)
     : mExpression( boost::proto::deep_copy(expr) )
    {
    }

    // Copy disabled
    ExpressionContainer(const ExpressionContainer &) = delete;
    ExpressionContainer & operator=(const ExpressionContainer &) = delete;
    // Move disabled
    ExpressionContainer(ExpressionContainer &&) = delete;
    ExpressionContainer & operator=(ExpressionContainer &&) = delete;

    /*! \brief Get SQL string of the expression
     *
     * \pre db must be valid (must have a driver loaded)
     */
    QString toSql(const QSqlDatabase & /*db*/) const override
    {
      return "NotImpl";
    }

   private:

    Expr mExpression;
  };

//   QString ExpressionContainer<Expr>::toSql(const QSqlDatabase & db) const
//   {
//   }

}}}} // namespace Mdt{ namespace Sql{ namespace Expression{ namespace JoinConstraint{

#endif // #ifndef MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_EXPRESSION_CONTAINER_H
