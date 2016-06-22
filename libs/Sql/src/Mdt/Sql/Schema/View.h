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

#include "ViewTable.h"
#include "SelectFieldList.h"
#include "Field.h"
#include "FieldName.h"
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
   *  #include "Mdt/Sql/Schema/View.h"
   *  #include "MyEntities/Client.h"  // A entitiy defined with TableTemplate
   *
   *  using Mdt::Sql::Schema::View;
   *  using Mdt::Sql::Schema::ViewTable;
   *  using Mdt::Sql::Schema::TableName;
   *  using Mdt::Sql::Schema::FieldName;
   *  using Mdt::Sql::Schema::JoinClause;
   *  using Mdt::Sql::Schema::JoinOperator;
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
   *  JoinKey key;
   *  View view;
   *
   *  view.setName("Client_address_view");
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
   */
  class View
  {
   public:

    /*! \brief Select operator
     */
    enum SelectOperator
    {
      Select,         /*!< SELECT operator */
      SelectDistinct  /*!< SELECT DISTINCT oprator */
    };

    /*! \brief Constructor
     */
    View();

    /*! \brief Set the name of the view
     */
    void setName(const QString & name);

    /*! \brief Get the name of the view
     */
    QString name() const
    {
      return pvName;
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
      return pvSelectOperator;
    }

    /*! \brief Set table
     *
     * Will be used for the FROM clause
     */
    void setTable(const ViewTable & table);

    /*! \brief Add a field to select
     */
    void addSelectField(const ViewTable & table, const Field & field, const QString & fieldAlias = QString());

    /*! \brief Add a field to select
     */
    void addSelectField(const ViewTable & table, const FieldName & fieldName, const QString & fieldAlias = QString());

    /*! \brief Add a field to select
     */
    void addSelectField(const ViewTable & table, const AutoIncrementPrimaryKey & pk, const QString & fieldAlias = QString());

    /*! \brief Add a field to select
     */
    void addSelectField(const ViewTable & table, const SingleFieldPrimaryKey & pk, const QString & fieldAlias = QString());

    /*! \brief Add a select all field for table
     *
     * For example, if table has its alias set to "CLI",
     *  the equivalent SQL statement will be SELECT CLI.*
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

    /*! \brief Get select field list
     */
    SelectFieldList selectFieldList() const
    {
      return pvSelectFieldList;
    }

    /*! \brief Add a JOIN clause
     */
    void addJoinClause(const JoinClause & join);

    /*! \brief Get list of JOIN clauses
     */
    JoinClauseList joinClauseList() const
    {
      return pvJoinClauseList;
    }

    /*! \brief Check if this View is null
     *
     * A View is null if:
     *  - View name was not set
     *  - (Main) table was not set
     *  - No select field was added
     */
    bool isNull() const
    {
      return ( pvName.isEmpty() || pvTable.isNull() || pvSelectFieldList.isEmpty() );
    }

    /*! \brief Clear
     */
    void clear();

   private:

    SelectOperator pvSelectOperator;
    QString pvName;
    ViewTable pvTable;
    SelectFieldList pvSelectFieldList;
    JoinClauseList pvJoinClauseList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_VIEW_H
