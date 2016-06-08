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

   private:

    Table pvTable;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_TEMPLATE_H
