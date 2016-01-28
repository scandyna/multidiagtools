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
#ifndef MDT_SQL_COPIER_EXPRESSION_MATCH_ITEM_H
#define MDT_SQL_COPIER_EXPRESSION_MATCH_ITEM_H

#include "mdtSqlWhereOperator.h"

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Match item for a SQL copier expression
   */
  struct ExpressionMatchItem
  {
    /*! \brief Field index for matching value in source
     */
    int sourceValueFieldIndex;

    /*! \brief Field index to match in destination
     */
    int destinationFieldIndex;

    /*! \brief WHERE operator with previous match item
     */
    mdtSqlWhereOperator operatorWithPrevious;

    /*! \brief Construct a match item
     */
    constexpr ExpressionMatchItem(int sourceValueFieldIndex, int destinationFieldIndex) noexcept
     : sourceValueFieldIndex(sourceValueFieldIndex),
       destinationFieldIndex(destinationFieldIndex)
    {
    }

    /*! \brief Construct a match item
     */
    constexpr ExpressionMatchItem(mdtSqlWhereOperator::Type op, int sourceValueFieldIndex, int destinationFieldIndex) noexcept
     : sourceValueFieldIndex(sourceValueFieldIndex),
       destinationFieldIndex(destinationFieldIndex),
       operatorWithPrevious(op)
    {
    }

    /*! \brief Check if match item is null
     */
    constexpr bool isNull() const noexcept
    {
      return ( (sourceValueFieldIndex < 0) || (destinationFieldIndex < 0) );
    }
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_EXPRESSION_MATCH_ITEM_H
