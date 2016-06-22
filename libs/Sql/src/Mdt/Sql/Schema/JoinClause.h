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
#ifndef MDT_SQL_SCHEMA_JOIN_CLAUSE_H
#define MDT_SQL_SCHEMA_JOIN_CLAUSE_H

#include "JoinOperator.h"
#include "JoinKeyList.h"
#include "MainTableField.h"
#include "TableToJoinField.h"
#include "ViewTable.h"

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief SQL JOIN clause
   *
   * Typical usage:
   * \code
   *  using Mdt::Sql::Schema::JoinClause;
   *  using Mdt::Sql::Schema::JoinOperator;
   *  using Mdt::Sql::Schema::ViewTable;
   *  using Mdt::Sql::Schema::TableName;
   *  using Mdt::Sql::Schema::MainTableField;
   *  using Mdt::Sql::Schema::TableToJoinField;
   *
   *  // Create instances of defined entities
   *  Client client;
   *
   *  // Define tables with their aliases
   *  ViewTable CLI(client, "CLI");
   *  ViewTable ADR(TableName("Address_tbl"), "ADR");
   *  JoinClause join;
   *
   *  join.setMainTable(CLI);
   *  join.setTableToJoin(ADR);
   *  join.addKey( MainTableField(client.Id_PK()), TableToJoinField("Client_Id_FK") );
   * \endcode
   *
   * \sa JoinClauseHelper
   */
  class JoinClause
  {
   public:

    /*! \brief Constructor
     */
    JoinClause()
     : pvOperator(JoinOperator::Join) {}

    /*! \brief Set JOIN operator
     */
    void setJoinOperator(JoinOperator::Operator op)
    {
      pvOperator = op;
    }

    /*! \brief Get JOIN operator
     */
    JoinOperator::Operator joinOperator() const
    {
      return pvOperator;
    }

    /*! \brief Set main table
     */
    void setMainTable(const ViewTable & table)
    {
      pvMainTable = table;
    }

    /*! \brief Get main table
     */
    ViewTable mainTable() const
    {
      return pvMainTable;
    }

    /*! \brief Set table to join
     */
    void setTableToJoin(const ViewTable & table)
    {
      pvTableToJoin = table;
    }

    /*! \brief Get table to join
     */
    ViewTable tableToJoin() const
    {
      return pvTableToJoin;
    }

    /*! \brief Add a couple of fields, between main table and table to join, that must be compared with fieldOperator
     *
     * The corresponding expression is:
     *  "<constraintOperator> <mainTableField> <fieldOperator> <tableToJoinField>".
     *  For example: "ON Id_PK = Client_Id_FK".
     */
    void addKey(JoinOperator::ConstraintOperator constraintOperator, const MainTableField & mainTableField, JoinOperator::RelativeFieldComparisonOperator fieldOperator, const TableToJoinField & tableToJoinField);

    /*! \brief Add a couple of fields, between main table and table to join, that must be compared with fieldOperator
     *
     * If this is the first key added, the corresponding expression is:
     *  "ON <mainTableField> <fieldOperator> <tableToJoinField>".
     *  For example: "ON Id_PK = Client_Id_FK".
     *
     * If a key was previously added, the corresponding expression is:
     *  "AND <mainTableField> <fieldOperator> <tableToJoinField>".
     *  For example: "AND Id_PK = Client_Id_FK".
     */
    void addKey(const MainTableField & mainTableField, JoinOperator::RelativeFieldComparisonOperator fieldOperator, const TableToJoinField & tableToJoinField);

    /*! \brief Add a couple of fields, between main table and table to join
     *
     * If this is the first key added, the corresponding expression is:
     *  "ON <mainTableField> = <tableToJoinField>".
     *  For example: "ON Id_PK = Client_Id_FK".
     *
     * If a key was previously added, the corresponding expression is:
     *  "AND <mainTableField> = <tableToJoinField>".
     *  For example: "AND Id_PK = Client_Id_FK".
     */
    void addKey(const MainTableField & mainTableField, const TableToJoinField & tableToJoinField);

    /*! \brief Get join key list
     */
    JoinKeyList keyList() const
    {
      return pvKeyList;
    }

    /*! \brief Clear
     */
    void clear();

   private:

    JoinOperator::Operator pvOperator;
    ViewTable pvMainTable;
    ViewTable pvTableToJoin;
    JoinKeyList pvKeyList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_JOIN_CLAUSE_H
