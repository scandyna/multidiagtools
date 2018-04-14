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
#ifndef MDT_SQL_WHERE_EXPRESSION_H
#define MDT_SQL_WHERE_EXPRESSION_H

#include "WhereField.h"
#include "Expression/WhereExpressionContainer.h"
#include "Expression/WhereExpressionGrammar.h"
#include <QString>
#include <QSqlDatabase>
#include <boost/proto/matches.hpp>
#include <memory>

namespace Mdt{ namespace Sql{

  namespace Expression {
    struct ExpressionContainerInterface;
  }

  /*! \brief Where expression
   *
   * Typical usage:
   * \code
   * #include <Mdt/Sql/WhereExpression.h>
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::WhereExpression;
   * using Sql::WhereField;
   * using Sql::TableName;
   * using Sql::FieldName;
   *
   * WhereField cliendId(TableName("Client_tbl"), FieldName("Id_PK") );
   * WhereField adrCliendId(TableName("Address_tbl"), FieldName("Client_Id_FK") );
   *
   * WhereExpression whereExp( adrCliendId == cliendId );
   * \endcode
   */
  class WhereExpression
  {
   public:

    /*! \brief Construct a empty expression
     */
    WhereExpression() = default;

    // Destructor must have its implementation in cpp file
    ~WhereExpression();

    /*! \brief Construct a expression
     *
     * \tparam Expr Type of the expression.
     * \param expr Expression to hold.
     * \pre Expr must be a where expression type.
     *       A where expression is based on WhereField,
     *       comparison operators, logical AND, logical OR.
     *       For example (see example code above for details):
     *       \code
     *       // Example of valid where expression
     *       adrCliendId == cliendId
     *       (adrCliendId == cliendId) && (adrCliendId >= 2)
     *
     *       // Example of invalid where expression
     *       adrCliendId + cliendId
     *       \endcode
     */
    template<typename Expr>
    WhereExpression(const Expr & expr)
     : mContainer( new Expression::WhereExpressionContainer< typename boost::proto::result_of::deep_copy<Expr>::type >(expr) )
    {
      static_assert( boost::proto::matches< Expr, Expression::WhereExpressionGrammar>::value , "Type of Expr is not a valid where expression." );
    }

    /*! \brief Construct a expression as copy of other
     */
    WhereExpression(const WhereExpression & other) = default;

    /*! \brief Construct a expression by moving other
     */
    WhereExpression(WhereExpression && other) = default;

    /*! \brief Assign other expression to this
     */
    WhereExpression & operator=(const WhereExpression & other) = default;

    /*! \brief Assign other expression to this
     */
    WhereExpression & operator=(WhereExpression &&) = default;

    /*! \brief Set expression
     *
     * See the constructors documentation for usage and preconditions.
     */
    template<typename Expr>
    void setExpression(const Expr & expr)
    {
      static_assert( boost::proto::matches< Expr, Expression::WhereExpressionGrammar>::value , "Type of Expr is not a valid where expression." );
      mContainer.reset( new Expression::WhereExpressionContainer< typename boost::proto::result_of::deep_copy<Expr>::type >(expr) );
    }

    /*! \brief Check if this expression is null
     */
    bool isNull() const
    {
      return !mContainer;
    }

    /*! \brief Get SQL representation of stored expression
     *
     * If we take clientId and adrCliendId of above example code,
     *  the expression adrCliendId == cliendId will return
     *  "Address_tbl.Client_Id_FK = Client_tbl.Id_PK" .
     *  Note that table names, field names and literal values
     *  are also escaped regarding driver provided by db .
     *
     * \pre A expression must be set
     * \pre db must be valid (a driver must be loaded)
     */
    QString toSql(const QSqlDatabase & db) const;

   private:

    std::shared_ptr<const Expression::ExpressionContainerInterface> mContainer;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_WHERE_EXPRESSION_H
