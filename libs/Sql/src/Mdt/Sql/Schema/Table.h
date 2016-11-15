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
#include "PrimaryKey.h"
#include "PrimaryKeyContainer.h"
#include "ForeignKey.h"
#include "ForeignKeyList.h"
#include "IndexList.h"
#include <QString>
#include <vector>
#include <type_traits>

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
   * Typical usage to define a simple table:
   * \code
   * #include "Mdt/Sql/Schema/Table.h"
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::Schema::Table;
   * using Sql::Schema::Field;
   * using Sql::Schema::FieldType;
   * using Mdt::Sql::Schema::ForeignKey;
   * using Mdt::Sql::Schema::ParentTableFieldName;
   * using Mdt::Sql::Schema::ChildTableFieldName;
   *
   * // Setup street field
   * Field Street:
   * Street.setName("Street");
   * Street.setType(FieldType::Varchar);
   * Street.setLength(100);
   *
   * // Setup table
   * Table table;
   * table.setName("Address_tbl");
   * table.setAutoIncrementPrimaryKey("Id_PK");
   * table.addField(Street);
   *
   * // Somewhere defined table
   * Table Client_tbl = getFromSomeWhere();
   * // Setup table basics
   * Table table;
   * table.setName("Address_tbl");
   * // Setup Id_PK field
   * AutoIncrementPrimaryKey Id_PK("Id_PK");
   * // Setup Client_Id_FK field
   * Field Client_Id_FK;
   * Client_Id_FK.setName("Client_Id_FK");
   * Client_Id_FK.setType(FieldType::Integer);
   * Client_Id_FK.setRequired(true);
   * // Add fileds to the table
   * table.setPrimaryKey(Id_PK);
   * table.addField(Client_Id_FK);
   * table.addField(Street);
   * // Setup fk_Client_Id_FK foreign key
   * ForeignKey fk_Client_Id_FK;
   * fk_Client_Id_FK.setParentTable(Client_tbl);
   * fk_Client_Id_FK.setOnDeleteAction(ForeignKey::Restrict);
   * fk_Client_Id_FK.setOnUpdateAction(ForeignKey::Cascade);
   * fk_Client_Id_FK.setCreateChildIndex(true);
   * fk_Client_Id_FK.addKeyFields(ParentTableFieldName(client_tbl.autoIncrementPrimaryKey()), ChildTableFieldName(Client_Id_FK));
   * table.addForeignKey(fk_Client_Id_FK);
   * \endcode
   *
   * Example for a simple table with composed primary key:
   * \code
   * #include "Mdt/Sql/Schema/Table.h"
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::Schema::Table;
   * using Sql::Schema::Field;
   * using Sql::Schema::FieldType;
   *
   * // Setup Id_A
   * Field Id_A:
   * Id_A.setName("Id_A");
   * Id_A.setType(FieldType::Integer);
   * // Setup Id_B
   * Field Id_B:
   * Id_A.setName("Id_B");
   * Id_B.setType(FieldType::Integer);
   * // Setup Name
   * Field Name;
   * Name.setName("Name");
   * Name.setType(FieldType::Varchar);
   * Name.setLength(100);
   *
   * // Setup table
   * Table table;
   * table.setName("MyTable_tbl");
   * table.setPrimaryKey(Id_A, Id_B);
   * table.addField(Name);
   * \endcode
   *
   * Example with foreign key refering to a allready existing table:
   * \code
   * #include "Mdt/Sql/Schema/Table.h"
   * #include "Client.h"  // Defaindes a Client based on TableTemplate
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::Schema::Table;
   * using Sql::Schema::Field;
   * using Sql::Schema::FieldType;
   * using Sql::Schema::ForeignKeyParameters;
   *
   * using Mdt::Sql::Schema::ForeignKey;
   * using Mdt::Sql::Schema::ParentTableFieldName;
   * using Mdt::Sql::Schema::ChildTableFieldName;
   *
   * // Somewhere defined table
   * Table client = getFromSomeWhere();
   *
   * // Setup street field
   * Field Street:
   * Street.setName("Street");
   * Street.setType(FieldType::Varchar);
   * Street.setLength(100);
   * // Setup Client_Id_FK field
   * Field Client_Id_FK;
   * Client_Id_FK.setName("Client_Id_FK");
   * Client_Id_FK.setType(FieldType::Integer);
   * Client_Id_FK.setRequired(true);
   *
   * // Define a common foreign key parameters
   * ForeignKeyParameters fkParameters;
   *
   * // Setup table
   * Table table;
   * table.setName("Address_tbl");
   * table.setAutoIncrementPrimaryKey("Id_PK");
   * table.addField(Street);
   * table.addForeignKey( Client_Id_FK, client, client.Id_PK() );
   *
   * // Add fileds to the table
   * table.setPrimaryKey(Id_PK);
   * table.addField(Client_Id_FK);
   * table.addField(Street);
   * // Setup fk_Client_Id_FK foreign key
   * ForeignKey fk_Client_Id_FK;
   * fk_Client_Id_FK.setParentTable(Client_tbl);
   * fk_Client_Id_FK.setOnDeleteAction(ForeignKey::Restrict);
   * fk_Client_Id_FK.setOnUpdateAction(ForeignKey::Cascade);
   * fk_Client_Id_FK.setCreateChildIndex(true);
   * fk_Client_Id_FK.addKeyFields(ParentTableFieldName(client_tbl.autoIncrementPrimaryKey()), ChildTableFieldName(Client_Id_FK));
   * table.addForeignKey(fk_Client_Id_FK);
   * \endcode
   *
   * \sa TableTemplate
   */
  class Table
  {
   public:

    /*! \brief Create a default constructed table
     */
    Table()
     : mPrimaryKeyFieldIndex(-1) ,
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
    void setTableName(const QString & name);

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return pvTableName;
    }

    /*! \brief Set a auto increment primary key
     *
     * \pre fieldName must not be empty
     * \pre A field having fieldName as name must not allready exist in this table
     */
    void setAutoIncrementPrimaryKey(const QString & fieldName);

    /*! \brief Set primary key
     *
     * Will set each field in fieldList as member of the primary key of this table.
     *  If a field does not allready exist, it will also be added to this table.
     *  If a field does not have its required flag set, it will also be set.
     *
     * \pre Each field in fieldList must have a different field name.
     */
    template<typename...Ts>
    void setPrimaryKey(const Ts & ...fieldList)
    {
      PrimaryKey pk(fieldList...);
      addFieldListIfNotExists(fieldList...);
      mPrimaryKey.setPrimaryKey(pk);
      updatePrimaryKeyFlags(pk);
    }

    /*! \brief Get type of stored primary key
     */
    PrimaryKeyContainer::Type primaryKeyType() const
    {
      return mPrimaryKey.primaryKeyType();
    }

    /*! \brief Get primary key
     *
     * \pre Stored primary key must be of type AutoIncrementPrimaryKey
     */
    AutoIncrementPrimaryKey autoIncrementPrimaryKey() const
    {
      return mPrimaryKey.autoIncrementPrimaryKey();
    }

    /*! \brief Get primary key
     *
     * \pre Stored primary key must be of type PrimaryKey
     */
    PrimaryKey primaryKey() const
    {
      return mPrimaryKey.primaryKey();
    }

    /*! \brief Add a field
     *
     * \pre A field with field's name must not allready been set
     */
    void addField(const Field & field)
    {
      Q_ASSERT(!contains(field));
      mFieldList.append(field);
    }

    /*! \brief Add a foreign key
     *
     * \note Child table name defined in fk is ignored. This table name is also considered as child table.
     * \note If fk requests to create a index (i.e. Foreign::createChildIndex() is true),
     *        no index is added at all in this table.
     *        When creating table in database using a Driver instance,
     *        Driver will take care to create these indexes.
     * \pre Each field of child table in fk must exist in this table
     */
    void addForeignKey(ForeignKey fk);

    /*! \brief Get foreign key that references table designed by tableName
     *
     * If no foreign key that references tableName exists in this table,
     *  a null ForeignKey is returned.
     */
    ForeignKey foreignKeyReferencing(const QString & tableName) const;

    /*! \brief Get list of foreign keys
     */
    ForeignKeyList foreignKeyList() const
    {
      return pvForeignKeyList;
    }

    /*! \brief Add a index
     *
     * \note index's name is keeped untouched.
     * \note index's table name is ignored, and replaced with this table name.
     * \pre Each field of index must exist in this table.
     */
    void addIndex(Index index);

    /*! \brief Get list of indexes
     *
     * \note Will only return indexes added with addIndex().
     *        \sa addForeignKey()
     */
    IndexList indexList() const
    {
      return pvIndexList;
    }

    /*! \brief Get field count
     */
    int fieldCount() const
    {
      return (mFieldList.size() + mPrimaryKeyFieldIndex + 1);
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

    /*! \brief Check if a field exists in this table
     *
     * Note that field names are compared in a case insensitive way.
     *  For exapmple, Id_PK is the same field as ID_PK
     */
    bool contains(const Field & field) const
    {
      return contains(field.name());
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
     *
     * \todo What is we add a unique Index ?
     */
    bool isFieldUnique(int index) const;

    /*! \brief Get default value for field at index
     *
     * \pre index must be in valid range
     */
    QVariant fieldDefaultValue(int index) const;

    /*! \brief Get field at index
     *
     * \pre index must be in valid range
     * \pre if primary key is of type AutoIncrementPrimaryKey or SingleFieldPrimaryKey,
     *       index must also be > 0
     */
    Field field(int index) const;

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

    /*! \brief Helper to add fields with variadic function
     */
    void addFieldListIfNotExists()
    {
    }

    /*! \brief Helper to add fields with variadic function
     */
    template<typename...Ts>
    void addFieldListIfNotExists(const Field & field, const Ts & ...fieldList)
    {
      if(!contains(field)){
        addField(field);
      }
      addFieldListIfNotExists(fieldList...);
    }

    /*! \brief Set list of indexes for primary key
     *
     * Will set list of indexes for primary key.
     * Each field that is part of primary key will be updated to be required.
     */
    void updatePrimaryKeyFlags(const PrimaryKey & pk);

    /*! \brief Access field at index
     */
    const Field & refFieldConst(int index) const;

    int mPrimaryKeyFieldIndex; // Used if pvPrimaryKey is AutoIncrementPrimaryKey or SingleFieldPrimaryKey
    bool pvIsTemporary;
    QString pvTableName;
    PrimaryKeyContainer mPrimaryKey;
    FieldList mFieldList;
    std::vector<int> mPrimaryKeyFieldIndexList;  // Used by isFieldPartOfPrimaryKey() for PrimaryKey (multi-column)
    ForeignKeyList pvForeignKeyList;
    IndexList pvIndexList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_H
