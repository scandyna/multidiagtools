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
   * view.addField(CLI, client.Id_PK(), "Client_Id");
   * view.addField(CLI, client.Name());
   * view.addField(ADR, address.Id_PK(), "Address_Id");
   * view.addField(ADR, address.Street());
   * view.setFromTable(CLI);
   * view.joinTable(ADR);
   * \endcode
   *
   * \sa SelectStatement
   */
  class View
  {
   public:

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

    /*! \internal Access select statement
     */
    const SelectStatement & selectStatement() const
    {
      return mSelectStatement;
    }

   private:

    QString mName;
    Mdt::Sql::SelectStatement mSelectStatement;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_VIEW_H
