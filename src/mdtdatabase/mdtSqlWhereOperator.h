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
#ifndef MDT_SQL_WHERE_OPERATOR_H
#define MDT_SQL_WHERE_OPERATOR_H

#include <QString>

struct mdtSqlWhereOperator;


/*! \brief Operator that can be used in SQL WHERE clause
 */
struct mdtSqlWhereOperator final
{
  /*! \brief This enum describes some operators that can be used in SQL WHERE clause
   */
  enum Type
  {
    Null, /*!< No operator */
    And,  /*!< AND operator */
    Or    /*!< OR operator */
  };

  /*! \brief Operator of object
   */
  Type value;

  /*! \brief Create a operator
   */
  constexpr mdtSqlWhereOperator(Type op = Null) noexcept
   : value(op)
  {
  }

  /*! \brief Assign operator
   */
  mdtSqlWhereOperator & operator=(mdtSqlWhereOperator::Type op) noexcept
  {
    value = op;
    return *this;
  }

  /*! \brief Check if operator a and b are equal
   */
  friend
  constexpr bool operator==(const mdtSqlWhereOperator & a, mdtSqlWhereOperator::Type b) noexcept
  {
    return (a.value == b);
  }

  /*! \brief Check if operator a and b are different
   */
  friend
  constexpr bool operator!=(const mdtSqlWhereOperator & a, mdtSqlWhereOperator::Type b) noexcept
  {
    return (a.value != b);
  }

  /*! \brief Check if operator a and b are equal
   */
  friend
  constexpr bool operator==(mdtSqlWhereOperator::Type a, const mdtSqlWhereOperator & b) noexcept
  {
    return (a == b.value);
  }

  /*! \brief Check if operator a and b are different
   */
  friend
  constexpr bool operator!=(mdtSqlWhereOperator::Type a, const mdtSqlWhereOperator & b) noexcept
  {
    return (a != b.value);
  }

  /*! \brief Check if operator a and b are equal
   */
  friend
  constexpr bool operator==(const mdtSqlWhereOperator & a, const mdtSqlWhereOperator & b) noexcept
  {
    return (a.value == b.value);
  }

  /*! \brief Check if operator a and b are different
   */
  friend
  constexpr bool operator!=(const mdtSqlWhereOperator & a, const mdtSqlWhereOperator & b) noexcept
  {
    return !(a == b);
  }

  /*! \brief Get string representation of operator
   */
  QString toString() const
  {
    return mdtSqlWhereOperatorString(value);
  }

  /*! \brief Get string representation of given operator
   */
  friend
  QString mdtSqlWhereOperatorString(mdtSqlWhereOperator::Type op)
  {
    switch(op){
      case mdtSqlWhereOperator::Null:
        return QStringLiteral();
        break;
      case mdtSqlWhereOperator::And:
        return QStringLiteral("AND");
        break;
      case mdtSqlWhereOperator::Or:
        return QStringLiteral("OR");
        break;
    }
    return QStringLiteral();
  }
};

#endif // #ifndef
