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
#ifndef MDT_SQL_SCHEMA_TABLE_TEMPLATE_H
#define MDT_SQL_SCHEMA_TABLE_TEMPLATE_H

#include "Table.h"

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Template class to define SQL schema table
   *
   * TableTemplate is a wraper of Table that permit to define
   *  a schema table.
   *
   * Example of Client_tbl.h:
   * \code
   * #ifndef SCHEMA_CLIENT_TBL_H
   * #define SCHEMA_CLIENT_TBL_H
   *
   * #include "Mdt/Sql/Schema/TableTemplate.h"
   *
   * namespace Schema{
   *
   *  class Client_tbl : public Mdt::Sql::Schema::TableTemplate<Client_tbl>
   *  {
   *   public:
   *
   *    Client_tbl();
   *
   *    // We expose Id_PK which will be used when defining Address_tbl
   *    AutoIncrementPrimaryKey Id_PK() const
   *    {
   *      return autoIncrementPrimaryKey();
   *    }
   *  };
   *
   * } // namespace Schema{
   *
   * #endif // #ifndef SCHEMA_CLIENT_TBL_H
   * \endcode
   *
   * Client_tbl.cpp would look like this:
   * \code
   * #include "Client_tbl.h"
   *
   * namespace Schema{
   *
   *  Client_tbl::Client_tbl()
   *  {
   *    using Mdt::Sql::Schema::FieldType;
   *    using Mdt::Sql::Schema::Field;
   *    using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
   *
   *    // Id_PK
   *    AutoIncrementPrimaryKey Id_PK;
   *    Id_PK.setFieldName("Id_PK");
   *    // Name
   *    Field Name;
   *    Name.setName("Name");
   *    Name.setType(FieldType::Varchar);
   *    Name.setLength(150);
   *    // Setup table
   *    setTableName("Client_tbl");
   *    setPrimaryKey(Id_PK);
   *    addField(Name);
   *  }
   *
   * } // namespace Schema{
   * \endcode
   *
   * We could also define Address_tbl.
   *  Address_tbl.h would look like this:
   * \code
   * #ifndef SCHEMA_ADDRESS_TBL_H
   * #define SCHEMA_ADDRESS_TBL_H
   *
   * #include "Mdt/Sql/Schema/TableTemplate.h"
   *
   * namespace Schema{
   *
   *  class Address_tbl : public Mdt::Sql::Schema::TableTemplate<Address_tbl>
   *  {
   *   public:
   *
   *    Address_tbl();
   *  };
   *
   * } // namespace Schema{
   *
   * #endif // #ifndef SCHEMA_ADDRESS_TBL_H
   * \endcode
   *  Address_tbl.cpp would look like this:
   * \code
   * #include "Address_tbl.h"
   *
   * namespace Schema{
   *
   *  Address_tbl::Address_tbl()
   *  {
   *    using Mdt::Sql::Schema::FieldType;
   *    using Mdt::Sql::Schema::Field;
   *    using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
   *    using Mdt::Sql::Schema::ForeignKey;
   *
   *    Client_tbl client_tbl;
   *
   *    // Id_PK
   *    AutoIncrementPrimaryKey Id_PK;
   *    Id_PK.setFieldName("Id_PK");
   *    // Client_Id_FK
   *    Field Client_Id_FK;
   *    Client_Id_FK.setName("Client_Id_FK");
   *    Client_Id_FK.setType(FieldType::Integer);
   *    Client_Id_FK.setRequired(true);
   *    // Street
   *    Field Street;
   *    Street.setName("Street");
   *    Street.setType(FieldType::Varchar);
   *    Street.setLength(150);
   *    // Setup Fk_Client_Id_FK
   *    ForeignKey Fk_Client_Id_FK;
   *    Fk_Client_Id_FK.setParentTable(client_tbl);
   *    Fk_Client_Id_FK.setChildTable(*this);
   *    Fk_Client_Id_FK.setOnDeleteAction(ForeignKey::Restrict);
   *    Fk_Client_Id_FK.setOnUpdateAction(ForeignKey::Cascade);
   *    Fk_Client_Id_FK.setCreateChildIndex(true);
   *    Fk_Client_Id_FK.addKeyFields(ParentTableField(client_tbl.Id_PK()), ChildTableField(Client_Id_FK));
   *    // Setup table
   *    setTableName("Address_tbl");
   *    setPrimaryKey(Id_PK);
   *    addField(Street);
   *  }
   *
   * } // namespace Schema{
   * \endcode
   */
  template<typename Derived>
  class TableTemplate
  {
   public:

    /*! \brief Get Table instance
     */
    Table toTable() const
    {
      return pvTable;
    }

   protected:

    /*! \brief Set temporary flag
     */
    void setTemporary(bool temporary)
    {
      pvTable.setTemporary(temporary);
    }

    /*! \brief Set table name
     */
    void setTableName(const QString & name)
    {
      pvTable.setTableName(name);
    }

    /*! \brief Set primary key
     */
    void setPrimaryKey(const AutoIncrementPrimaryKey & pk)
    {
      pvTable.setPrimaryKey(pk);
    }

    /*! \brief Set primary key
     */
    void setPrimaryKey(const SingleFieldPrimaryKey & pk)
    {
      pvTable.setPrimaryKey(pk);
    }

    /*! \brief Set primary key
     */
    void setPrimaryKey(const PrimaryKey & pk)
    {
      pvTable.setPrimaryKey(pk);
    }

    /*! \brief Add a field
     */
    void addField(const Field & field)
    {
      pvTable.addField(field);
    }

    /*! \brief Get primary key
     *
     * \pre Stored primary key must be of type AutoIncrementPrimaryKey
     */
    AutoIncrementPrimaryKey autoIncrementPrimaryKey() const
    {
      return pvTable.autoIncrementPrimaryKey();
    }

    /*! \brief Get primary key
     *
     * \pre Stored primary key must be of type SingleFieldPrimaryKey
     */
    SingleFieldPrimaryKey singleFieldPrimaryKey() const
    {
      return pvTable.singleFieldPrimaryKey();
    }

    /*! \brief Get primary key
     *
     * \pre Stored primary key must be of type PrimaryKey
     */
    PrimaryKey primaryKey() const
    {
      return pvTable.primaryKey();
    }

   private:

    Table pvTable;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_TEMPLATE_H
