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
   * \include libs/Sql/tests/src/Schema/Client_tbl.h
   *
   * Client_tbl.cpp would look like this:
   * \include libs/Sql/tests/src/Schema/Client_tbl.cpp
   *
   * Example of Address_tbl.h:
   * \include libs/Sql/tests/src/Schema/Address_tbl.h
   *
   * Address_tbl.cpp would look like this:
   * \include libs/Sql/tests/src/Schema/Address_tbl.cpp
   */
  template<typename Derived>
  class TableTemplate
  {
   public:

    /*! \brief Get Table instance
     */
    Table toTable() const
    {
      return mTable;
    }

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return mTable.tableName();
    }

    /*! \brief Get list of foreign keys
     */
    ForeignKeyList foreignKeyList() const
    {
      return mTable.foreignKeyList();
    }

   protected:

    /*! \brief Set temporary flag
     */
    void setTemporary(bool temporary)
    {
      mTable.setTemporary(temporary);
    }

    /*! \brief Set table name
     */
    void setTableName(const QString & name)
    {
      mTable.setTableName(name);
    }

    /*! \brief Set a auto increment primary key
     *
     * \pre A field having fieldName as name must not allready exist in this table
     */
    void setAutoIncrementPrimaryKey(const QString & fieldName)
    {
      mTable.setAutoIncrementPrimaryKey(fieldName);
    }

    /*! \brief Set primary key
     *
     * Will set each field in fieldList as member of the primary key of this table.
     *  If a field does not allready exist, it will also be added to this table.
     *
     * \pre Each field in fieldList must have a different field name.
     */
    template<typename...Ts>
    void setPrimaryKey(const Ts & ...fieldList)
    {
      mTable.setPrimaryKey(fieldList...);
    }

    /*! \brief Add a field
     */
    void addField(const Field & field)
    {
      mTable.addField(field);
    }

    /*! \brief Add a foreign key
     *
     * \param field The field of this table that will be part of the foreign key
     * \param foreignTable The table to which the foreign key refers
     * \param foreignField The field, in foreignTable, to which the foreign key refers
     * \param settings The settings for the foreign key
     *
     * If field does not allready exist in this table, it will also be added.
     *
     * \pre field must not be null
     * \pre foreignTable's name must not be empty
     * \pre foreignField's name must not be empty
     */
    void addForeignKey(const Field & field, const ForeignTable & foreignTable, const ForeignField & foreignField, const ForeignKeySettings & settings)
    {
      mTable.addForeignKey(field, foreignTable, foreignField, settings);
    }

    /*! \brief Add a foreign key
     *
     * \param fieldList A list of fields of this table that will be part of the foreign key
     * \param foreignTable The table to which the foreign key refers
     * \param foreignFieldList A list of fields, in foreignTable, to which the foreign key refers
     * \param settings The settings for the foreign key
     *
     * Each field in fieldList that does not allready exist in this table will also be added.
     *
     * \pre fieldList must not contain any null field
     * \pre foreignTable's name must not be empty
     * \pre foreignFieldList must not contains any field with a empty field name
     * \pre fieldList and foreignFieldList must be of same size and at least contain 1 field
     */
    void addForeignKey(const FieldList & fieldList, const ForeignTable & foreignTable, const ForeignFieldList & foreignFieldList, const ForeignKeySettings & settings)
    {
      mTable.addForeignKey(fieldList, foreignTable, foreignFieldList, settings);
    }

    /*! \brief Add a foreign key
     *
     * \note Child table name defined in fk is ignored. This table name is also considered as child table.
     * \pre Each field of child table in fk must exist in this table
     */
    [[deprecated]]
    void addForeignKey(const ForeignKey & fk)
    {
      mTable.addForeignKey(fk);
    }

    /*! \brief Get primary key
     *
     * \pre Stored primary key must be of type AutoIncrementPrimaryKey
     */
    AutoIncrementPrimaryKey autoIncrementPrimaryKey() const
    {
      return mTable.autoIncrementPrimaryKey();
    }

    /*! \brief Get primary key
     *
     * \pre Stored primary key must be of type PrimaryKey
     */
    PrimaryKey primaryKey() const
    {
      return mTable.primaryKey();
    }

    /*! \brief Get field at index
     *
     * \pre index must be in valid range
     * \pre if primary key is of type AutoIncrementPrimaryKey or SingleFieldPrimaryKey,
     *       index must also be > 0
     */
    Field field(int index) const
    {
      return mTable.field(index);
    }

   private:

    Table mTable;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_TEMPLATE_H
