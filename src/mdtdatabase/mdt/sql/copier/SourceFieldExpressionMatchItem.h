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
#ifndef MDT_SQL_COPIER_SOURCE_FIELD_EXPRESSION_MATCH_ITEM_H
#define MDT_SQL_COPIER_SOURCE_FIELD_EXPRESSION_MATCH_ITEM_H

#include <QString>
#include <QLatin1String>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief A item of the WHERE clause in SourceFieldExpression
   *
   * The expression is of form [operatorWithPrevious] referringTableField = sourceTableField
   */
  struct SourceFieldExpressionMatchItem
  {
    /*! \brief Operator
     */
    enum Operator
    {
      None, /*!< No operator */
      And,  /*!< AND operator*/
      Or    /*!< OR operator */
    };

    /*! \brief Operator with previous item
     *
     * F.ex. AND, OR
     */
    Operator operatorWithPrevious;

    /*! \brief Referring table field
     *
     * This is the left part of =
     */
    QString referringTableField;

    /*! \brief Source table field
     *
     * This is the right part of =
     */
    QString sourceTableField;

    /*! \brief Get operatorWithPrevious string
     */
    QString operatorWithPreviousStr() const
    {
      switch(operatorWithPrevious){
        case None:
          return QString();
        case And:
          return QLatin1String("AND");
        case Or:
          return QLatin1String("OR");
      }
      return QString();
    }
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_SOURCE_FIELD_EXPRESSION_MATCH_ITEM_H

