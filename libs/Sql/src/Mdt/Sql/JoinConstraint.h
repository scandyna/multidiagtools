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
#ifndef MDT_SQL_JOIN_JOIN_CONSTRAINT_H
#define MDT_SQL_JOIN_JOIN_CONSTRAINT_H

#include "JoinConstraintOperator.h"
#include "JoinConstraintExpression.h"
#include "JoinConstraintFieldPairList.h"
#include <boost/variant.hpp>

namespace Mdt{ namespace Sql{

  class SelectTable;

  /*! \brief Join constraint part of a JoinClause
   */
  class JoinConstraint
  {
   public:

    /*! \brief Construct a null join constraint
     */
    JoinConstraint() = default;

    /*! \brief Construct a ON join constraint expression
     */
    JoinConstraint(const JoinConstraintExpression & expr);

    /*! \brief Construct a ON join constraint linking left and right tables
     */
    JoinConstraint(const SelectTable & left, const SelectTable & right);

    // Copy and move
    JoinConstraint(const JoinConstraint &) = default;
    JoinConstraint(JoinConstraint &&) = default;
    JoinConstraint & operator=(const JoinConstraint &) = default;
    JoinConstraint & operator=(JoinConstraint &&) = default;

    /*! \brief Set a ON expression
     */
    void setOnExpression(const JoinConstraintExpression & expr);

    /*! \brief Generate a ON expression to join left and right table
     */
    void setOnExpression(const SelectTable & left, const SelectTable & right);

    /*! \brief Check if this constraint is null
     */
    bool isNull() const;

    /*! \brief Get constraint operator
     *
     * Note: if this join constraint is null,
     *        returned operator has no sense.
     */
    JoinConstraintOperator constraintOperator() const
    {
      return mOperator;
    }

    /*! \internal Access internal variant
     *
     * This is used by transforms and unit tests
     */
    const auto & constraint() const
    {
      return mConstraint;
    }

   private:

    JoinConstraintOperator mOperator;
    boost::variant<JoinConstraintExpression, JoinConstraintFieldPairList> mConstraint;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_JOIN_JOIN_CONSTRAINT_H
