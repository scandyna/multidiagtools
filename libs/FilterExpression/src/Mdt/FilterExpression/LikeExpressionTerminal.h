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
#ifndef MDT_FILTER_EXPRESSION_LIKE_EXPRESSION_TERMINAL_H
#define MDT_FILTER_EXPRESSION_LIKE_EXPRESSION_TERMINAL_H

#include <QString>
#include <boost/proto/expr.hpp>
#include <boost/proto/extends.hpp>
#include <boost/proto/operators.hpp>
#include <boost/proto/traits.hpp>

namespace Mdt{ namespace FilterExpression{

  /*! \brief Expression using wildcards in a FilterExpression
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
  template<typename Domain = boost::proto::default_domain>
  struct LikeExpressionTerminal : boost::proto::extends<
                                      boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<QString> >,
                                      LikeExpressionTerminal< Domain >,
                                      Domain
                                    >
  {
   private:

    typedef boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<QString> >  terminal_type;
    typedef LikeExpressionTerminal< Domain >                                                      LikeExpression_t;
    typedef boost::proto::extends< terminal_type, LikeExpression_t, Domain >                      base_type;

   public:

    typedef QString          value_type;
    typedef QString &        reference;
    typedef const QString &  const_reference;
    /*
     * This will define operator()= , which permit to give better error messages with grammars.
     * (Without this, error could look like 'error: no match for operator= ......')
     */
    BOOST_PROTO_EXTENDS_USING_ASSIGN(LikeExpressionTerminal)

    /*! \brief Construct a like expression
     */
    explicit LikeExpressionTerminal(const QString & expr)
    : base_type(terminal_type::make( QString(expr) ))
    {
    }

    /*! \brief Get expression
     */
    QString expression() const
    {
      return boost::proto::value(*this);
    }
  };

}} // namespace Mdt{ namespace FilterExpression{

#endif // #ifndef MDT_FILTER_EXPRESSION_LIKE_EXPRESSION_TERMINAL_H
