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
#ifndef MDT_SQL_JOIN_CONSTRAINT_FIELD_PAIR_H
#define MDT_SQL_JOIN_CONSTRAINT_FIELD_PAIR_H

#include <QString>

namespace Mdt{ namespace Sql{

  /*! \brief Container for a pair of field names
   */
  class JoinConstraintFieldPair
  {
   public:

    /*! \brief Construct a field pair
     *
     * \pre left and right must not be empty
     */
    JoinConstraintFieldPair(const QString & left, const QString & right)
     : mLeftField(left) ,
       mRightField(right)
    {
      Q_ASSERT(!mLeftField.isEmpty());
      Q_ASSERT(!mRightField.isEmpty());
    }

    /*! \brief Get left field
     */
    QString leftField() const
    {
      return mLeftField;
    }

    /*! \brief Get right field
     */
    QString rightField() const
    {
      return mRightField;
    }

   private:

    QString mLeftField;
    QString mRightField;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_JOIN_CONSTRAINT_FIELD_PAIR_H
