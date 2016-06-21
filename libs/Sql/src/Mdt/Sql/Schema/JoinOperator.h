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
#ifndef MDT_SQL_SCHEMA_JOIN_OPERATOR_H
#define MDT_SQL_SCHEMA_JOIN_OPERATOR_H

namespace Mdt{ namespace Sql{ namespace Schema{

  class JoinOperator
  {
   public:

    /*! \brief JOIN operator
     */
    enum Operator
    {
      Join,     /*!< JOIN operator */
      LeftJoin  /*!< LEFT JOIN operator */
    };

    /*! \brief Absolute JOIN constraint field comparison operator
     */
    enum AbsoluteFieldComparisonOperator
    {
      MtfEqualTdjf,       /*!< Maint table field = table to join field */
      MtfLessThanTdjf,    /*!< Maint table field < table to join field */
      MtfGreaterThanTdjf  /*!< Maint table field > table to join field */
    };

    /*! \brief Relative JOIN constraint field comparison operator
     */
    enum RelativeFieldComparisonOperator
    {
      Equal,        /*!< table field at left = table field at right */
      LessThan,     /*!< table field at left < table field at right  */
      GreaterThan   /*!< table field at left > table field at right  */
    };

    /*! \brief JOIN constraint operator
     */
    enum ConstraintOperator
    {
      On,   /*!< ON operator */
      And,  /*!< AND operator */
      Or    /*!< OR operator */
    };
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
#endif // #ifndef MDT_SQL_SCHEMA_JOIN_OPERATOR_H
