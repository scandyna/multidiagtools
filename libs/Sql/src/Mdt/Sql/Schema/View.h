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
#ifndef MDT_SQL_SCHEMA_VIEW_H
#define MDT_SQL_SCHEMA_VIEW_H

#include "../SelectStatement.h"
#include "../SelectTable.h"
#include "../FieldName.h"

#include "ViewTable.h"
#include "Field.h"
#include "Mdt/Sql/SelectFieldList.h"
#include "Mdt/Sql/FieldName.h"
#include "AutoIncrementPrimaryKey.h"
#include "SingleFieldPrimaryKey.h"
#include "JoinClauseList.h"
#include "JoinClause.h"

#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Helper class for SQL database schema creation
   *
   * This is a basic class that is mainly used
   *  in Mdt::Sql::Schema as container to define SQL views.
   *
   * Typical usage:
   * \code
   * #include "Mdt/Sql/Schema/View.h"
   * #include "MyEntities/Client.h"   // A entitiy defined with TableTemplate
   * #include "MyEntities/Address.h"  // A entitiy defined with TableTemplate
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::Schema::View;
   * using Sql::SelectTable;
   *
   * // Create instances of defined entities
   * Client client;
   * Address address;
   *
   * // Define tables with their aliases
   * SelectTable CLI(client, "CLI");
   * SelectTable ADR(address, "ADR");
   *
   * // Create view
   * View view;
   * view.setName("Client_address_view");
   *
   *  using Mdt::Sql::Schema::ViewTable;
   *  using Mdt::Sql::Schema::TableName;
   *  using Mdt::Sql::Schema::FieldName;
   *  using Mdt::Sql::Schema::JoinClause;
   *  using Mdt::Sql::Schema::JoinOperator;
   *  using Mdt::Sql::Schema::MainTableField;
   *  using Mdt::Sql::Schema::TableToJoinField;
   *
   *
   *  JoinClause join;
   *  JoinKey key;
   *
   *
   *  
   *  view.setTable(CLI);
   *  view.addSelectField(CLI, client.Id_PK(), "Client_Id");
   *  view.addSelectField(CLI, client.Name());
   *  view.addSelectField(ADR, FieldName("Id_PK"), "Address_Id");
   *  view.addSelectField(ADR, FieldName("Street"));
   *  join.setMainTable(CLI);
   *  join.setTableToJoin(ADR);
   *  join.addKey( MainTableField(client.Id_PK()), TableToJoinField("Client_Id_FK") );
   *  view.addJoinClause(join);
   * \endcode
   *
   * \sa SelectStatement
   */
  class View
  {
   public:

    /*! \brief Select operator
     *
     * \deprecated
     */
//     enum SelectOperator
//     {
//       Select,         /*!< SELECT operator */
//       SelectDistinct  /*!< SELECT DISTINCT oprator */
//     };

//     /*! \brief Constructor
//      */
//     View();

    /*! \brief Set the name of the view
     */
    void setName(const QString & name);

    /*! \brief Get the name of the view
     */
    QString name() const
    {
      return mName;
    }

    /*! \brief Set select operator
     *
     * Default is Select operator.
     */
    void setSelectOperator(SelectOperator s);

    /*! \brief Get select operator
     */
    SelectOperator selectOperator() const
    {
      return mSelectStatement.selectOperator();
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
     * This is the same than setFromTable()
     */
    void setTable(const SelectTable & table);

    /*! \brief Set from table
     *
     * If a raw SQL string was set,
     *  it will be cleared.
     *
     * \pre No joined table must allready been set.
     * \pre table must not be null.
     */
    void setFromTable(const SelectTable & table);


    /*! \brief Set table
     *
     * Will be used for the FROM clause
     * \deprecated
     */
    void setTable(const ViewTable & table);

    /*! \brief Add a field to select
     * \deprecated
     */
    void addSelectField(const ViewTable & table, const Field & field, const QString & fieldAlias = QString());

    /*! \brief Add a field to select
     * \deprecated
     */
    void addSelectField(const ViewTable & table, const FieldName & fieldName, const QString & fieldAlias = QString());

    /*! \brief Add a field to select
     * \deprecated
     */
    void addSelectField(const ViewTable & table, const AutoIncrementPrimaryKey & pk, const QString & fieldAlias = QString());

    /*! \brief Add a field to select
     * \deprecated
     */
    void addSelectField(const ViewTable & table, const SingleFieldPrimaryKey & pk, const QString & fieldAlias = QString());

    /*! \brief Add a select all field for table
     *
     * For example, if table has its alias set to "CLI",
     *  the equivalent SQL statement will be SELECT CLI.*
     * \deprecated
     */
    void addSelectAllFields(const ViewTable & table);

    /*! \brief Get table name
     *
     * Returns the table name of table set by setTable()
     */
    QString tableName() const
    {
      return pvTable.tableName();
    }

    /*! \brief Get table name alias
     *
     * Returns the table alias of table set by setTable()
     */
    QString tableNameAlias() const
    {
      return pvTable.alias();
    }

    /*! \brief Get table
     *
     * Returns the table set by setTable()
     */
    ViewTable table() const
    {
      return pvTable;
    }

    /*! \brief Get select field list
     * \deprecated
     */
    SelectFieldList selectFieldList() const
    {
      return pvSelectFieldList;
    }

    /*! \brief Add a JOIN clause
     * \deprecated
     */
    void addJoinClause(const JoinClause & join);

    /*! \brief Get list of JOIN clauses
     * \deprecated
     */
    JoinClauseList joinClauseList() const
    {
      return pvJoinClauseList;
    }

    /*! \brief Check if this View is null
     *
     * A view is null as long as:
     *  - View name was not set
     *  - No table was set
     *  - No field was added
     */
    bool isNull() const
    {
      return ( mName.isEmpty() || mSelectStatement.isNull() );
    }

    /*! \brief Clear
     */
    void clear();

    /*! \internal Access select statement
     */
    const SelectStatement & selectStatement() const
    {
      return mSelectStatement;
    }

   private:

    QString mName;
    Mdt::Sql::SelectStatement mSelectStatement;

//     SelectOperator pvSelectOperator;
    ViewTable pvTable;
    SelectFieldList pvSelectFieldList;
    JoinClauseList pvJoinClauseList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_VIEW_H
