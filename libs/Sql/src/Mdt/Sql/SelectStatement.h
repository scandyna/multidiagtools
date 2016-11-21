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
#ifndef MDT_SQL_SELECT_STATEMENT_H
#define MDT_SQL_SELECT_STATEMENT_H

#include "SelectOperator.h"
#include "SelectFieldList.h"
#include "JoinOperator.h"
#include "FromClause.h"
#include "SelectTable.h"
#include "JoinConstraintField.h"
#include "JoinConstraintExpression.h"
#include "WhereExpression.h"

namespace Mdt{ namespace Sql{

  namespace Schema{

    template<typename T>
    class TableTemplate;

    class Table;
    class Field;
    class AutoIncrementPrimaryKey;
    class SingleFieldPrimaryKey;
  } // namespace Schema{

  class FieldName;

  /*! \brief Helper class to create a SQL SELECT statement
   *
   * Example of usage:
   * \code
   * #include "Mdt/Sql/SelectStatement.h"
   * #include "Mdt/Sql/FieldName.h"
   * #include "Mdt/Sql/TableName.h"
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::SelectStatement;
   * using Sql::SelectTable;
   * using Sql::FieldName;
   * using Sql::TableName;
   * using Sql::JoinConstraintField;
   * using Sql::WhereField;
   *
   * // Define tables with their alias
   * SelectTable CLI(TableName("Client_tbl") , "CLI");
   * SelectTable ADR(TableName("Address_tbl") , "ADR");
   *
   * // Define fields to join CLI and ADR
   * JoinConstraintField clientId(CLI, FieldName("Id_PK"));
   * JoinConstraintField adrClientId(ADR, FieldName("Client_Id_FK"));
   *
   * // Define fields for where expression
   * WhereField clientName(CLI, FieldName("Name"));
   *
   * // Create SELECT statement
   * SelectStatement stm;
   * stm.addField(CLI, FieldName("Id_PK"), "Client_Id");
   * stm.addField(CLI, FieldName("Name"));
   * stm.addField(ADR, FieldName("Id_PK"), "Address_Id");
   * stm.addField(ADR, FieldName("Street"));
   * stm.setFromTable(CLI);
   * stm.joinTable(ADR, adrClientId == clientId);
   * stm.setWhereExpression( (clientName == "Name 1") || (clientName == "Name 2") );
   * \endcode
   *
   * Above example does not use any predefined entities.
   *  If a schema allready exists (see Mdt::Sql::Schema namespace for details),
   *  it is better to reuse informations it allready contains:
   * \code
   * #include "Mdt/Sql/SelectStatement.h"
   * #include "MyEntities/Client.h"   // A entitiy defined with TableTemplate
   * #include "MyEntities/Address.h"  // A entitiy defined with TableTemplate
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::SelectStatement;
   * using Sql::SelectTable;
   * using Sql::WhereField;
   *
   * // Create instances of defined entities
   * Client client;
   * Address address;
   *
   * // Define tables with their alias
   * SelectTable CLI(client , "CLI");
   * SelectTable ADR(address , "ADR");
   *
   * // Define fields for where expression
   * WhereField clientName(CLI, client.Name());
   *
   * // Create SELECT statement
   * SelectStatement stm;
   * stm.addField(CLI, client.Id_PK(), "Client_Id");
   * stm.addField(CLI, client.Name());
   * stm.addField(ADR, address.Id_PK(), "Address_Id");
   * stm.addField(ADR, address.Stree());
   * stm.setFromTable(CLI);
   * stm.joinTable(ADR);
   * stm.setWhereExpression( (clientName == "Name 1") || (clientName == "Name 2") );
   * \endcode
   *
   * For select statement that only get data from 1 table, we could also avoid using SelectTable:
   * \code
   * #include "Mdt/Sql/SelectStatement.h"
   * #include "MyEntities/Client.h"   // A entitiy defined with TableTemplate
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::SelectStatement;
   *
   * // Create instance of defined entity
   * Client client;
   *
   * // Create SELECT statement
   * SelectStatement stm;
   * stm.addField(client.Id_PK(), "Client_Id");
   * stm.addField(client.Name());
   * stm.setFromTable(client);
   * \endcode
   */
  class SelectStatement
  {
   public:

    /*! \brief Set select operator
     *
     * Default is Select
     */
    void setSelectOperator(SelectOperator op)
    {
      mSelectOperator = op;
    }

    /*! \brief Get select operator
     */
    SelectOperator selectOperator() const
    {
      return mSelectOperator;
    }

    /*! \brief Add a field by specifying table and field
     *
     * \pre table must not be null
     * \pre field must not be null
     */
    void addField(const SelectTable & table, const FieldName & field , const QString & fieldAlias = QString());

    /*! \brief Add a field by specifying field
     *
     * \pre field must not be null
     */
    void addField(const FieldName & field , const QString & fieldAlias = QString());

    /*! \brief Add all fields for table (the * in SQL)
     *
     * \pre table must not be null
     */
    void addAllFields(const SelectTable & table);

    /*! \brief Add all fields (the * in SQL, not table defined)
     */
    void addAllFields();

    /*! \brief Add a raw SQL string to select a field
     *
     * \pre sql must not be a empty string
     */
    void addRawSqlFieldExpression(const QString & sql);

    /*! \brief Set table
     *
     * This is the same than setFromTable(const SelectTable & table)
     */
    void setTable(const SelectTable & table)
    {
      setFromTable(table);
    }

    /*! \brief Set table
     *
     * This is the same than setFromTable(const Schema::Table & table)
     */
    void setTable(const Schema::Table & table)
    {
      setFromTable(table);
    }

    /*! \brief Set table
     *
     * This is the same than setFromTable(const Schema::TableTemplate<T> & table)
     */
    template<typename T>
    void setTable(const Schema::TableTemplate<T> & table)
    {
      setFromTable(table);
    }

    /*! \brief Set from table
     *
     * If a raw SQL string was set,
     *  it will be cleared.
     *
     * \pre No joined table must allready been set.
     * \pre table must not be null.
     */
    void setFromTable(const SelectTable & table);

    /*! \brief Set from table
     *
     * If a raw SQL string was set,
     *  it will be cleared.
     *
     * \pre No joined table must allready been set.
     * \pre table must have a non empty name.
     */
    void setFromTable(const Schema::Table & table);

    /*! \brief Set from table
     *
     * If a raw SQL string was set,
     *  it will be cleared.
     *
     * \pre No joined table must allready been set.
     * \pre table must have a non empty name.
     */
    template<typename T>
    void setFromTable(const Schema::TableTemplate<T> & table)
    {
      Q_ASSERT(!table.tableName().isEmpty());
      setFromTable(SelectTable(table));
    }

    /*! \brief Join a table
     *
     * Join table with expr as constraint
     *
     * \pre table must not be null
     * \pre expr must not be null
     * \pre From table must allready been set (see setFromTable()).
     */
    void joinTable(const SelectTable & table, const JoinConstraintExpression & expr);

    /*! \brief Join a table automatically
     *
     * Will fetch table's foreign key list
     *  and fromTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre table or fromTable must have a relation defined by their foreign key
     * \pre From table must allready been set (see setFromTable()).
     */
    void joinTable(const SelectTable & table);

    /*! \brief Join a table automatically
     *
     * Will fetch table's foreign key list
     *  and constraintOnTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre constraintOnTable must not be null
     * \pre table or constraintOnTable must have a relation defined by their foreign key
     * \pre From table must allready been set (see setFromTable()).
     */
    void joinTable(const SelectTable & table, const SelectTable & constraintOnTable);

    /*! \brief Left join a table
     *
     * Join table with expr as constraint
     *
     * \pre table must not be null
     * \pre expr must not be null
     * \pre From table must allready been set (see setFromTable()).
     */
    void leftJoinTable(const SelectTable & table, const JoinConstraintExpression & expr);

    /*! \brief Left join a table automatically
     *
     * Will fetch table's foreign key list
     *  and fromTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre table or fromTable must have a relation defined by their foreign key
     * \pre From table must allready been set (see setFromTable()).
     */
    void leftJoinTable(const SelectTable & table);

    /*! \brief Left join a table automatically
     *
     * Will fetch table's foreign key list
     *  and constraintOnTable's foreign key list about relation
     *  and also generate the apropriate join constraint.
     *
     * \pre table must not be null
     * \pre constraintOnTable must not be null
     * \pre table or constraintOnTable must have a relation defined by their foreign key
     * \pre From table must allready been set (see setFromTable()).
     */
    void leftJoinTable(const SelectTable & table, const SelectTable & constraintOnTable);

    /*! \brief Set where expression
     *
     * \pre expr must not be null
     */
    void setWhereExpression(const WhereExpression & expr);

    /*! \brief Check if this statement is null
     *
     * A select statement is null as long as:
     *  - No table was set
     *  - No field was added
     */
    bool isNull() const
    {
      return (mFromClause.isNull() || mFieldList.isEmpty());
    }

    /*! \internal Get list of fields
     *
     * Used by transforms and unit tests
     */
    const SelectFieldList & fieldList() const
    {
      return mFieldList;
    }

    /*! \internal Access mFromClause
     *
     * Used by transforms and unit tests
     */
    const FromClause & fromClause() const
    {
      return mFromClause;
    }

    /*! \internal Access mWhereExpression
     *
     * Used for transforms and unit tests
     */
    const WhereExpression & whereExpression() const
    {
      return mWhereExpression;
    }

   private:

    SelectOperator mSelectOperator = SelectOperator::Select;
    SelectFieldList mFieldList;
    FromClause mFromClause;
    WhereExpression mWhereExpression;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SELECT_STATEMENT_H
