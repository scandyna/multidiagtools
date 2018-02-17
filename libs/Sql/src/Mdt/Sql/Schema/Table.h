/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "PrimaryKeyType.h"
#include "AutoIncrementPrimaryKey.h"
#include "PrimaryKey.h"
#include "PrimaryKeyFieldIndexList.h"
#include "PrimaryKeyContainer.h"
#include "ForeignField.h"
#include "ForeignFieldList.h"
#include "ForeignKeySettings.h"

#include "ForeignKey.h"
#include "ForeignKeyList.h"
#include "IndexList.h"
#include "MdtSqlExport.h"
#include <QString>
#include <vector>
#include <type_traits>

// #include <QDebug>

namespace Mdt{ namespace Sql{ namespace Schema{

  class ForeignTable;

  template<typename T>
  class TableTemplate;

  /*! \brief Rule when updating collation on a Table
   */
  enum class TableCollationUpdateRule
  {
    UpdateAllStringFields,    /*!< All fileds in table that are of string type */
    UpdateUnsetStringFields   /*!< Fileds in table that are of string type and that have no collation set */
  };

  /*! \brief SQL schema table
   *
   * \todo Modify obselete example
   * Typical usage to define a simple table:
   * \code
   * #include <Mdt/Sql/Schema/Table.h>
   *
   * using namespace Mdt::Sql::Schema;
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
   * #include <Mdt/Sql/Schema/Table.h>
   *
   * using namespace Mdt::Sql::Schema;
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
   * Example with foreign key referring to a allready existing table:
   * \code
   * #include "Mdt/Sql/Schema/Table.h"
   * #include "Mdt/Sql/Schema/ForeignTable.h"
   * #include "Client.h"  // Defindes a Client based on TableTemplate
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::Schema::Table;
   * using Sql::Schema::Field;
   * using Sql::Schema::FieldType;
   * using Sql::Schema::ForeignKeyAction;
   * using Sql::Schema::ForeignKeySettings;
   * using Sql::Schema::ForeignTable;
   * using Sql::Schema::ForeignField;
   *
   * // Somewhere defined table
   * Client client;
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
   * // Define common foreign key settings
   * ForeignKeySettings defaultFkSettings;
   * defaultFkSettings.setOnDeleteAction(ForeignKeyAction::Restrict);
   * defaultFkSettings.setOnUpdateAction(ForeignKeyAction::Cascade);
   * defaultFkSettings.setCreateIndex(true);
   *
   * // Setup table
   * Table table;
   * table.setTableName("Address_tbl");
   * table.setAutoIncrementPrimaryKey("Id_PK");
   * table.addField(Street);
   * table.addForeignKey( Client_Id_FK, ForeignTable(client), ForeignField(client.Id_PK()), defaultFkSettings );
   * \endcode
   *
   * \sa TableTemplate
   */
  class MDT_SQL_EXPORT Table
  {
   public:

    /*! \brief Create a default constructed table
     */
    Table()
     : pvIsTemporary(false)
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
      return mTableName;
    }

    /*! \brief Set a auto increment primary key
     *
     * If no field having \a fieldName as name already exists in this table,
     *  it will be added to this table as primary key,
     *  and its attributes (like isUnique or isRequired)
     *  will be set according to the requirements of a primary key.
     *
     * If a field having \a fieldName as name already exists in this table,
     *  its attributes (like isUnique or isRequired)
     *  will be set according to the requirements of a primary key,
     *  and the field will be set as primary key.
     *
     * \pre \a fieldName must not be empty
     * \pre if a field having \a fieldName as name already exists in this table,
     *   its type must be valid for a auto increment primary key.
     * \sa AutoIncrementPrimaryKey::isValidFieldType()
     */
    void setAutoIncrementPrimaryKey(const QString & fieldName);

    /*! \brief Set a auto increment primary key
     *
     * This method works like setAutoIncrementPrimaryKey(const QString&) ,
     *  but the field type can be specified.
     *
     * \pre \a fieldName must not be empty
     * \pre \a fieldType must be valid for a auto increment primary key
     * \pre if a field having \a fieldName as name already exists in this table,
     *   its type must be the same as \a fieldType .
     * \sa AutoIncrementPrimaryKey::isValidFieldType()
     */
    void setAutoIncrementPrimaryKey(const QString & fieldName, FieldType fieldType);

    /*! \brief Set primary key
     *
     * Will set each field in \a primaryKey as member of the primary key of this table.
     *  If a field does not have its required flag set, it will also be set.
     *
     * \pre \a primaryKey must not be null
     * \pre Each field in \a primaryKey must already exit in this table.
     */
    void setPrimaryKey(const PrimaryKey & primaryKey);

    /*! \brief Set primary key
     *
     * Will set each field in \a fieldList as member of the primary key of this table.
     *  If a field does not already exist, it will also be added to this table.
     *  If a field does not have its required flag set, it will also be set.
     *
     * \pre Each field in fieldList must have a different field name.
     */
    template<typename...Ts>
    void setPrimaryKey(const Ts & ...fieldList)
    {
      PrimaryKey pk(fieldList...);
      addFieldListIfNotExists(fieldList...);
      setPrimaryKey(pk);
    }

    /*! \brief Set \a primaryKeyContainer to this table
     *
     * \sa setAutoIncrementPrimaryKey()
     * \sa setPrimaryKey(const PrimaryKey &)
     */
    void setPrimaryKeyContainer(const PrimaryKeyContainer & primaryKeyContainer);

    /*! \brief Get type of stored primary key
     */
    PrimaryKeyType primaryKeyType() const
    {
      return mPrimaryKeyType;
    }

    /*! \brief Get primary key
     *
     * \pre Stored primary key must be of type AutoIncrementPrimaryKey
     */
    AutoIncrementPrimaryKey autoIncrementPrimaryKey() const
    {
      Q_ASSERT(mPrimaryKeyType == PrimaryKeyType::AutoIncrementPrimaryKey);
      Q_ASSERT(mPrimaryKeyFieldIndexList.count() == 1);

      return mPrimaryKeyFieldIndexList.toAutoIncrementPrimaryKey(mFieldList);
    }

    /*! \brief Get primary key
     *
     * \pre Stored primary key must be of type PrimaryKey
     */
    PrimaryKey primaryKey() const
    {
      Q_ASSERT(mPrimaryKeyType == PrimaryKeyType::PrimaryKey);
      Q_ASSERT(!mPrimaryKeyFieldIndexList.isEmpty());

      return mPrimaryKeyFieldIndexList.toPrimaryKey(mFieldList);
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
    void addForeignKey(const Field & field, const ForeignTable & foreignTable, const ForeignField & foreignField, const ForeignKeySettings & settings);

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
    void addForeignKey(const FieldList & fieldList, const ForeignTable & foreignTable, const ForeignFieldList & foreignFieldList, const ForeignKeySettings & settings);

    /*! \brief Get foreign key that references table
     *
     * If no foreign key that references table exists in this table,
     *  a null ForeignKey is returned.
     */
    template<typename T>
    ForeignKey foreignKeyReferencing(const TableTemplate<T> & table) const
    {
      return foreignKeyReferencing(table.tableName());
    }

    /*! \brief Get foreign key that references table
     *
     * If no foreign key that references table exists in this table,
     *  a null ForeignKey is returned.
     */
    ForeignKey foreignKeyReferencing(const Table & table) const
    {
      return foreignKeyReferencing(table.mTableName);
    }

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
      return mForeignKeyList;
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
      return mFieldList.size();
    }

    /*! \brief Get index of field with fieldName in this table
     *
     * If fieldName not exists in this table,
     *  -1 is returned.
     *
     * Note that field names are compared in a case insensitive way.
     *  For exapmple, Id_PK is the same field as ID_PK
     */
    int fieldIndex(const QString & fieldName) const
    {
      return mFieldList.fieldIndex(fieldName);
    }

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

    /*! \brief Access field at index
     */
    const Field & refFieldConst(int index) const;

    bool pvIsTemporary;
    QString mTableName;
    PrimaryKeyType mPrimaryKeyType = PrimaryKeyType::Unknown;
    PrimaryKeyFieldIndexList mPrimaryKeyFieldIndexList;
    FieldList mFieldList;
    ForeignKeyList mForeignKeyList;
    IndexList pvIndexList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_H
