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
#ifndef MDT_FILTER_EXPRESSION_LIKE_EXPRESSION_H
#define MDT_FILTER_EXPRESSION_LIKE_EXPRESSION_H

#include <QString>

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
  class LikeExpression
  {
   public:

    /*! \brief Construct a like expression
     */
    explicit LikeExpression(const QString & expr)
     : mExpression(expr)
    {
    }

    /*! \brief Get expression
     */
    QString expression() const
    {
      return mExpression;
    }

   private:

    QString mExpression;
  };

}} // namespace Mdt{ namespace FilterExpression{

#endif // #ifndef MDT_FILTER_EXPRESSION_LIKE_EXPRESSION_H
