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
#ifndef MDT_SQL_SCHEMA_JOIN_KEY_H
#define MDT_SQL_SCHEMA_JOIN_KEY_H

#include "JoinOperator.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief SQL join key
   */
  class JoinKey
  {
   public:

    /*! \brief Constructor
     */
    JoinKey()
     : pvConstraintOperator(JoinOperator::On),
       pvFieldComparisonOperator(JoinOperator::MtfEqualTdjf)
    {
    }

    /*! \brief Set main table field
     */
    void setMainTableFieldName(const QString & fieldName)
    {
      pvMainTableFieldName = fieldName;
    }

    /*! \brief Get main table field
     */
    QString mainTableFieldName() const
    {
      return pvMainTableFieldName;
    }

    /*! \brief Set table to join field
     */
    void setTableToJoinFieldName(const QString & fieldName)
    {
      pvTableToJoinFieldName = fieldName;
    }

    /*! \brief Get table to join field
     */
    QString tableToJoinFieldName() const
    {
      return pvTableToJoinFieldName;
    }

    /*! \brief Set JOIN constraint operator
     *
     * Default is ON
     */
    void setConstraintOperator(JoinOperator::ConstraintOperator op)
    {
      pvConstraintOperator = op;
    }

    /*! \brief Get JOIN constraint operator
     */
    JoinOperator::ConstraintOperator constraintOperator() const
    {
      return pvConstraintOperator;
    }

    /*! \brief Set JOIN field comparison operator
     *
     * Default operator is =
     */
    void setFieldComparisonOperator(JoinOperator::AbsoluteFieldComparisonOperator op)
    {
      pvFieldComparisonOperator = op;
    }

    /*! \brief Get JOIN field comparison operator
     */
    JoinOperator::AbsoluteFieldComparisonOperator fieldComparisonOperator() const
    {
      return pvFieldComparisonOperator;
    }

    /*! \brief Check if key is null
     *
     * Key is null until main table field and table to join field are set.
     */
    bool isNull() const
    {
      return ( pvMainTableFieldName.isEmpty() || pvTableToJoinFieldName.isEmpty() );
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvConstraintOperator = JoinOperator::On;
      pvFieldComparisonOperator = JoinOperator::MtfEqualTdjf;
      pvMainTableFieldName.clear();
      pvTableToJoinFieldName.clear();
    }

   private:

    JoinOperator::ConstraintOperator pvConstraintOperator;
    JoinOperator::AbsoluteFieldComparisonOperator pvFieldComparisonOperator;
    QString pvMainTableFieldName;
    QString pvTableToJoinFieldName;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_JOIN_KEY_H
