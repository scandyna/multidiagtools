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

#include "SelectField.h"
#include "SelectFieldList.h"
#include "SelectTable.h"
#include "JoinConstraintField.h"
#include "JoinConstraintExpression.h"

namespace Mdt{ namespace Sql{

  namespace Schema{
    class Field;
    class AutoIncrementPrimaryKey;
    class SingleFieldPrimaryKey;
  }

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
   * using Sql::SelectField;
   * using Sql::SelectTable;
   * using Sql::FieldName;
   * using Sql::TableName;
   * using Sql::JoinConstraintField;
   *
   * // Define tables with their alias
   * SelectTable CLI(TableName("Client_tbl") , "CLI");
   * SelectTable ADR(TableName("Address_tbl") , "ADR");
   *
   * // Define fields to join CLI and ADR
   * JoinConstraintField clientId(CLI, FieldName("Id_PK"));
   * JoinConstraintField adrClientId(ADR, FieldName("Client_Id_FK"));
   *
   * // Create SELECT statement
   * SelectStatement stm;
   * stm.addField(CLI, FieldName("Id_PK"), "Client_Id");
   * stm.addField(CLI, FieldName("Name"));
   * stm.addField(ADR, FieldName("Id_PK"), "Address_Id");
   * stm.addField(ADR, FieldName("Street"));
   * stm.setTable(CLI);
   * stm.joinTable(ADR, adrClientId == clientId);
   * \endcode
   *
   * Above example does not use any predefined entities.
   *  If a schema allready exists (see Mdt::Sql::Shema namespace for details),
   *  it is better to reuse informations it allready contains:
   * \code
   * #include "Mdt/Sql/SelectStatement.h"
   * #include "MyEntities/Client.h"   // A entitiy defined with TableTemplate
   * #include "MyEntities/Address.h"  // A entitiy defined with TableTemplate
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::SelectStatement;
   * using Sql::SelectField;
   * using Sql::SelectTable;
   *
   * // Create instances of defined entities
   * Client client;
   * Address address;
   *
   * // Define tables with their alias
   * SelectTable CLI(client , "CLI");
   * SelectTable ADR(address , "ADR");
   *
   * // Create SELECT statement
   * SelectStatement stm;
   * stm.addField(CLI, client.Id_PK(), "Client_Id");
   * stm.addField(CLI, client.Name());
   * stm.addField(ADR, address.Id_PK(), "Address_Id");
   * stm.addField(ADR, address.Stree());
   * stm.setTable(CLI);
   * stm.joinTable(ADR);
   * \endcode
   */
  class SelectStatement
  {
   public:

    /*! \brief Add a field to the statement
     */
    void addField(const SelectTable & table, const SelectField & field);

    /*! \brief Add a field to the statement
     */
    void addField(const SelectTable & table, const FieldName & field , const QString & fieldAlias = QString());

    /*! \brief Add a field to the statement
     */
    void addField(const SelectTable & table, const Schema::Field & field , const QString & fieldAlias = QString());

    /*! \brief Add a field to the statement
     */
    void addField(const SelectTable & table, const Schema::AutoIncrementPrimaryKey & field , const QString & fieldAlias = QString());

    /*! \brief Add a field to the statement
     */
    void addField(const SelectTable & table, const Schema::SingleFieldPrimaryKey & field , const QString & fieldAlias = QString());

    /*! \brief Get list of fields
     */
    SelectFieldList fieldList() const
    {
      return mFieldList;
    }

   private:

    SelectFieldList mFieldList;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SELECT_STATEMENT_H
