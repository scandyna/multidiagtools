/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_SQL_VIEW_SCHEMA_JOIN_CLAUSE_H
#define MDT_SQL_VIEW_SCHEMA_JOIN_CLAUSE_H

#include <QString>
#include <QSqlDriver>

/*! \brief Helper class for mdtSqlViewSchema to handle JOIN clause
 */
class mdtSqlViewSchemaJoinClause
{
 public:

  /*! \brief Default constructor
   */
  mdtSqlViewSchemaJoinClause()
   : pvJoinOperator("JOIN")
  {
  }

  /*! \brief Set join operator
   *
   * Default is JOIN
   */
  void setJoinOperator(const QString & op)
  {
    pvJoinOperator = op;
  }

  /*! \brief Set tables
   *
   * \param table Name or alias of table to witch join is to be done
   * \param tableToJoinName Name of the table to join
   * \param TableToJoinAlias Alias of table to join
   *
   * For example, if we have a table named Client_tbl with CLI as alias,
   *  and we join Address_tbl with ADR as alias, we call this function as follow:
   * \code
   *  setTables("CLI", "Address_tbl", "ADR");
   * \endcode
   */
  void setTables(const QString & table, const QString & tableToJoinName, const QString & tableToJoinAlias = QString())
  {
    pvTable = table;
    pvTableToJoinName = tableToJoinName;
    pvTableToJoinAlias = tableToJoinAlias;
  }

  /*! \brief Set the ON constraint
   */
  void setConstraintOn(const QString & tableField, const QString & tableToJoinField, const QString & op = "=")
  {
    pvTableField = tableField;
    pvTableToJoinField = tableToJoinField;
    pvConstraintOperator = op;
  }

  /*! \brief Clear
   */
  void clear()
  {
    pvTable.clear();
    pvTableToJoinName.clear();
    pvTableToJoinAlias.clear();
    pvJoinOperator = "JOIN";
    pvTableField.clear();
    pvTableToJoinField.clear();
    pvConstraintOperator.clear();
  }

  /*! \brief Get the SQL statement for join clause
   *
   * Note that each call of this function will rebuild the SQL statement.
   *
   * \pre driver must not be a null pointer
   */
  QString getSql(const QSqlDriver *driver) const
  {
    Q_ASSERT(driver != nullptr);

    QString sql;

    // Check if we have all tables set
    if( (pvTable.isEmpty()) || (tableToJointNameOrAlias().isEmpty()) ){
      return sql;
    }
    // Check that both fields are set
    if( (pvTableField.isEmpty()) || (pvTableToJoinField.isEmpty()) ){
      return sql;
    }
    // Check that operators are set
    if( (pvJoinOperator.isEmpty()) || (pvConstraintOperator.isEmpty()) ){
      return sql;
    }
    // Build SQL statement
    sql = " " + pvJoinOperator + " " + driver->escapeIdentifier(pvTableToJoinName, QSqlDriver::TableName);
    if(pvTableToJoinAlias.isEmpty()){
      sql += "\n";
    }else{
      sql += " " + driver->escapeIdentifier(pvTableToJoinAlias, QSqlDriver::TableName) + "\n";
    }
    sql += "  ON " + driver->escapeIdentifier(tableToJointNameOrAlias(), QSqlDriver::TableName) \
         + "." + driver->escapeIdentifier(pvTableToJoinField, QSqlDriver::FieldName) \
         + " " + pvConstraintOperator + " " + driver->escapeIdentifier(pvTable, QSqlDriver::TableName) \
         + "." + driver->escapeIdentifier(pvTableField, QSqlDriver::FieldName) + "\n";

    return sql;
  }

 private:

  /*! \brief Get table to join name or alias
   *
   * If pvTableToJoinAlias was set it will be returned,
   *  else pvTableToJoinName
   */
  QString tableToJointNameOrAlias() const
  {
    if(pvTableToJoinAlias.isEmpty()){
      return pvTableToJoinName;
    }
    return pvTableToJoinAlias;
  }

  QString pvTable;
  QString pvTableToJoinName;
  QString pvTableToJoinAlias;
  QString pvJoinOperator;
  QString pvTableField;
  QString pvTableToJoinField;
  QString pvConstraintOperator;
};

#endif // #ifndef MDT_SQL_VIEW_SCHEMA_JOIN_CLAUSE_H
