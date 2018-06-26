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
#ifndef MDT_QUERY_EXPRESSION_LIKE_EXPRESSION_H
#define MDT_QUERY_EXPRESSION_LIKE_EXPRESSION_H

#include "MdtQueryExpression_CoreExport.h"
#include <QString>
#include <boost/proto/expr.hpp>
#include <boost/proto/extends.hpp>
#include <boost/proto/operators.hpp>
#include <boost/proto/traits.hpp>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Expression using wildcards
   *
   * Supported wildcards are:
   * <table class="srcdoc_td_left">
   *  <tr><th>Wildcard</th><th>Description</th></tr>
   *  <tr><td>*</td><td>A substitue for 0 or more characters</td></tr>
   *  <tr><td>?</td><td>A substitue for 1 character</td></tr>
   * </table>
   *
   * To match a wildcard, escape it with a '\'.
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT LikeExpression : boost::proto::extends<
                                                            boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<QString> >,
                                                            LikeExpression,
                                                            boost::proto::default_domain
                                                          >
  {
   private:

    using Domain = boost::proto::default_domain;
    using Expression = boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<QString> >;
    using ParentClass = boost::proto::extends< Expression, LikeExpression, Domain >;

   public:

    using value_type = QString;
    using reference = value_type &;
    using const_reference = const value_type &;

    /*
     * This will define operator()= , which permit to give better error messages with grammars.
     * (Without this, error could look like 'error: no match for operator= ......')
     */
    BOOST_PROTO_EXTENDS_USING_ASSIGN(LikeExpression)

    /*! \brief Construct a like expression
     */
    explicit LikeExpression(const QString & expr)
    : ParentClass(Expression::make( QString(expr) ))
    {
    }

    /*! \brief Get expression
     */
    QString expression() const
    {
      return boost::proto::value(*this);
    }
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_LIKE_EXPRESSION_H
