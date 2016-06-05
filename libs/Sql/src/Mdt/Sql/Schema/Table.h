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
#ifndef MDT_SQL_SCHEMA_TABLE_H
#define MDT_SQL_SCHEMA_TABLE_H

#include "Field.h"
#include "FieldList.h"
#include "PrimaryKeyContainer.h"
#include <QString>
#include <vector>

// #include <QDebug>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Rule when updating collation on a Table
   */
  enum class TableCollationUpdateRule
  {
    UpdateAllStringFields,    /*!< All fileds in table that are of string type */
    UpdateUnsetStringFields   /*!< Fileds in table that are of string type and that have no collation set */
  };

  /*! \brief SQL schema table
   *
   * Typical usage for a simple table:
   * \code
   * using Mdt::Sql::Schema::Table;
   * using Mdt::Sql::Schema::Field;
   * using Mdt::Sql::Schema::FieldType;
   * using Mdt::Sql::Schema::AutoIncrementPrimaryKey;
   *
   * // Setup table basics
   * Table table;
   * table.setName("Client_tbl");
   * // Setup Id_PK field
   * AutoIncrementPrimaryKey Id_PK;
   * Id_PK.setFieldName("Id_PK");
   * // Setup name field
   * Field Name:
   * Name.setName("Name");
   * Name.setType(FieldType::Varchar);
   * Name.setLength(100);
   * // Add fileds to the table
   * table.setPrimaryKey(Id_PK);
   * table.addField(Name);
   * \endcode
   */
  class Table
  {
   public:

    /*! \brief Create a default constructed table
     */
    Table()
     : pvPrimaryKeyFieldIndex(-1) ,
       pvIsTemporary(false)
    {
    }

    /*! \brief Set temporary flag
     *
     * By default, Table is not temporary
     */
    void setTemporary(bool temporary)
    {
      pvIsTemporary = temporary;
    }

    /*! \brief Check if table is temporary
     */
    bool isTemporary() const
    {
      return pvIsTemporary;
    }

    /*! \brief Set table name
     */
    void setTableName(const QString & name)
    {
      pvTableName = name;
    }

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return pvTableName;
    }

    /*! \brief Set primary key
     *
     * Note that pk's field will allways appear as first field in table
     *
     * \pre A field with pk's field name must not allready been set
     */
    void setPrimaryKey(const AutoIncrementPrimaryKey & pk)
    {
      Q_ASSERT(!contains(pk.fieldName()));
      pvPrimaryKeyFieldIndex = 0;
      pvPrimaryKey.setPrimaryKey(pk);
    }

    /*! \brief Set primary key
     *
     * Note that pk's field will allways appear as first field in table
     *
     * \pre A field with pk's field name must not allready been set
     */
    void setPrimaryKey(const SingleFieldPrimaryKey & pk)
    {
      Q_ASSERT(!contains(pk.fieldName()));
      pvPrimaryKeyFieldIndex = 0;
      pvPrimaryKey.setPrimaryKey(pk);
    }

    /*! \brief Set primary key
     *
     * \pre Each field name in pk must exist in this table
     */
    void setPrimaryKey(const PrimaryKey & pk);

    /*! \brief Add a field
     *
     * \pre A field with field's name must not allready been set
     */
    void addField(const Field & field)
    {
      Q_ASSERT(!contains(field.name()));
      pvFieldList.append(field);
    }

    /*! \brief Get field count
     */
    int fieldCount() const
    {
      return (pvFieldList.size() + pvPrimaryKeyFieldIndex + 1);
    }

    /*! \brief Get index of field with fieldName in this table
     *
     * If fieldName not exists in this table,
     *  -1 is returned.
     *
     * Note that field names are compared in a case insensitive way.
     *  For exapmple, Id_PK is the same field as ID_PK
     */
    int fieldIndex(const QString & fieldName) const;

    /*! \brief Check if a field with fieldName exists in this table
     *
     * Note that field names are compared in a case insensitive way.
     *  For exapmple, Id_PK is the same field as ID_PK
     */
    bool contains(const QString & fieldName) const
    {
      return (fieldIndex(fieldName) > -1);
    }

    /*! \brief Get field name at index
     *
     * \pre index must be in valid range
     */
    QString fieldName(int index) const;

    /*! \brief Get field type at index
     *
     * \pre index must be in valid range
     */
    FieldType fieldType(int index) const;

    /*! \brief Get field length at index
     *
     * A value < 0 means that length was not set.
     *
     * \pre index must be in valid range
     */
    int fieldLength(int index) const;

    /*! \brief Check if field at index is part of primary key
     *
     * \pre index must be in valid range
     */
    bool isFieldPartOfPrimaryKey(int index) const;

    /*! \brief Check if field at index is auto increment
     *
     * \pre index must be in valid range
     */
    bool isFieldAutoIncrement(int index) const;

    /*! \brief Check if field at index is required
     *
     * \pre index must be in valid range
     */
    bool isFieldRequired(int index) const;

    /*! \brief Check if field at index is unique
     *
     * \pre index must be in valid range
     */
    bool isFieldUnique(int index) const;

    /*! \brief Check if table is null
     *
     * Table is null if one of the mandatory attribute is not set.
     *  Mandatory attributes are:
     *  - tableName
     *  - table must contain at least 1 field
     */
    bool isNull() const;

    /*! \brief Clear
     */
    void clear();

   private:

    /*! \brief Access field at index
     */
    const Field & refFieldConst(int index) const;

    int pvPrimaryKeyFieldIndex; // Used if pvPrimaryKey is AutoIncrementPrimaryKey or SingleFieldPrimaryKey
    bool pvIsTemporary;
    QString pvTableName;
    PrimaryKeyContainer pvPrimaryKey;
    FieldList pvFieldList;
    std::vector<int> pvPrimaryKeyFieldIndexList;  // Used by isFieldPartOfPrimaryKey() for PrimaryKey (multi-column)
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_H
