/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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

#include "mdtError.h"
#include "mdtSqlDriverType.h"
#include "mdtSqlCollation.h"
#include "mdtSqlField.h"
#include "mdtSqlIndex.h"
#include "mdtSqlPrimaryKey.h"
#include "mdtSqlForeignKey.h"
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlIndex>
#include <QList>
#include <QHash>

#include <QSqlField>  /// \todo remove once done

/*
 * Container for foreign keys informations
 */
struct mdtSqlSchemaTableForeignKeyInfo
{
  QString referingTableName;
  QSqlRecord fields;
  QSqlRecord referingFields;
  int actionOnDelete;
  int actionOnUpdate;
};

/*! \brief Helper class that generate SQL table creation string
 */
class mdtSqlSchemaTable
{
 public:

  /*! \brief Action on update or on delete
   */
  enum foreignKeyAction_t {
                            Cascade,  /*!< Cascade */
                            SetNull,  /*!< Set null */
                            NoAction, /*!< No action */
                            Restrict  /*!< Restrict */
  };

  /*! \brief Constructor
   */
  mdtSqlSchemaTable();

  /*! \brief Destructor
   */
  ~mdtSqlSchemaTable();

  /*! \brief Clear
   */
  void clear();

  /*! \brief Set the driver name
   *
   * Driver name must be given in format returned by QSqlDatabase::driverName() .
   */
  [[deprecated]]
  bool setDriverName(const QString & name);

  /*! \brief Set database name
   */
  void setDatabaseName(const QString & name);

  /*! \brief Set table name
   *
   * \todo Currently, charset and cs parameters are ignored (see mdtSqlField to define column specific collations)
   *       It must be defined if they must be used as table wide default collation (regarding standard SQL and target DBMS)
   */
  void setTableName(const QString & name, const QString &charset = QString(), Qt::CaseSensitivity cs = Qt::CaseInsensitive);

  /*! \brief Get table name
   */
  QString tableName() const;

  /*! \brief Set/unset temporary flag
   *
   * If temporary is true, the table will be created
   *  as temporary table (CREATE TEMPORARY TABLE ... in Sqlite and MySql).
   */
  void setTemporary(bool temporary);

  /*! \brief Check temporary table flag
   */
  bool isTemporary() const
  {
    return pvIsTemporary;
  }

  /*! \brief Set the Maria DB/MySQL storage engine (XtraDB, Inno DB, ...)
   */
  void setStorageEngineName(const QString & name);

  /*! \brief Add a field
   * \todo IMPLEMENT !
   */
  void addField(const mdtSqlField & field, bool isPartOfPrimaryKey);

  /*! \brief Add a field
   *
   * \deprecated
   */
  void addField(const QSqlField & field, bool isPartOfPrimaryKey);

  /*! \brief Get number of fields
   */
  int fieldCount() const
  {
    return pvFields.size();
  }

  /*! \brief Access fields list
   */
//   const QList<QSqlField> & fieldList() const
//   {
//     return pvFields;
//   }

  /*! \brief Get list of field names
   *
   * Note: the list is built at every call of this function.
   */
  QStringList getFieldNameList() const;

  /*! \brief Get field for given field name
   */
  mdtSqlField field(const QString & fieldName) const;

  /*! \brief Get field for given field index
   *
   * \pre index must be in valid range
   */
  mdtSqlField field(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvFields.size());
    return pvFields.at(index);
  }

  /*! \brief Get index of field name
   *
   * If field name does not exist, -1 is returned.
   */
  int fieldIndex(const QString & name)
  {
    for(int i = 0; i < pvFields.size(); ++i){
      if(pvFields.at(i).name() == name){
        return i;
      }
    }
    return -1;
  }

  /*! \brief Get field name for given field index
   *
   * \pre index must be valid
   */
  QString fieldName(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvFields.size());
    return pvFields.at(index).name();
  }

  /*! \brief Get field type name for given field index
   *
   * \pre index must be valid
   */
  QString fieldTypeName(int index, mdtSqlDriverType::Type driverType) const;

  /*! \brief Get field length for given index
   *
   * \pre index must be valid
   */
  QVariant fieldLength(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvFields.size());

    int length = pvFields.at(index).length();
    if(length < 0){
      return QVariant();
    }
    return length;
  }

  /*! \brief Access primary key
   */
  const mdtSqlPrimaryKey & primaryKey() const
  {
    return pvPrimaryKey;
  }

  /*! \brief Add a index
   *
   * \note The table name of given index is ignored.
   */
  void addIndex(const mdtSqlIndex & index);

  /*! \brief Get list of indexes
   */
  QList<mdtSqlIndex> indexList() const
  {
    return pvIndexes;
  }

  /*! \brief Add a foreign key
   *
   * \note fk's child table name is ignored.
   * \pre fk's parent table name must be set
   * \pre For each key field contained in FK, child table's field must exist in current table
   */
  void addForeignKey(const mdtSqlForeignKey & fk);

  /*! \brief Set foreign key at given index
   *
   * \note fk's child table name is ignored.
   * \pre fk's parent table name must be set
   * \pre For each key field contained in FK, child table's field must exist in current table
   * \pre index must be valid
   */
  void setForeignKeyAt(int index, const mdtSqlForeignKey & fk);

  /*! \brief Get list of foreign keys
   */
  QList<mdtSqlForeignKey> foreignKeyList() const
  {
    return pvForeignKeys;
  }

  /*! \brief Add a index
   *
   * If name allready exists, it will not be appended
   *
   * \deprecated
   */
  void addIndex(const QString & name, bool unique);

  /*! \brief Add a field to a index
   *
   * Field must allready be created with addField() and index also with addIndex() .
   *  If index with name indexName does not exists, or field with fieldName not exists,
   *  this method returns false.
   *
   * \deprecated
   */
  bool addFieldToIndex(const QString & indexName, const QString & fieldName);

  /*! \brief Add a foreign key contraint
   *
   * \deprecated
   */
  void addForeignKey(const QString & name, const QString & referingTableName, const foreignKeyAction_t & actionOnDelete, const foreignKeyAction_t & actionOnUpdate);

  /*! \brief Add a field to a foreign key
   *
   * Field must allready be created with addField() and foreign key also with addForeignKey() .
   *  If foreign key with name foreignKeyName does not exists, or field with fieldName not exists,
   *  this method returns false.
   *
   * \deprecated
   */
  bool addFieldToForeignKey(const QString & foreignKeyName, const QString & fieldName, const QString & referingFieldName);

  /*! \brief Update a foreign key refering table name
   * \deprecated
   */
  bool updateForeignKeyReferingTable(const QString & name, const QString & referingTableName);

  /*! \brief Setup structure from a existing table
   *
   * Can be used to create a table that has same structure than a existing one.
   *  (Could also be used for reverse engeneering in future..)
   *
   * \pre db must be open
   * \todo Add indexes + foreign keys support
   */
  bool setupFromTable(const QString & name, QSqlDatabase db);

  /*! \brief Get SQL statements for table creation
   *
   * \pre db's driver must be loaded
   */
  QStringList getSqlForCreateTable(const QSqlDatabase & db) const;

  /*! \brief Get SQL statements for table deletion
   *
   * \pre db's driver must be loaded
   */
  QStringList getSqlForDropTable(const QSqlDatabase & db) const;

  /*! \brief Get SQL statement for table creation
   */
  [[deprecated]]
  QString sqlForCreateTable();

  /*! \brief Get SQL statement for table deleteion
   */
  [[deprecated]]
  QString sqlForDropTable() const;

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Check if primary key definition must be added in field definition or not
   */
  bool includePrimaryKeyDefinitionInField(const QString & fieldName) const;

  /*! \brief For each key fields in fk, check if child table's field exists in current table
   */
  bool childFieldsExistsInTable(const mdtSqlForeignKey & fk);

  /*! \brief Get foreignKeyAction_t type from name
   *
   * If name is unknown, NoAction is returned.
   */
  foreignKeyAction_t foreignKeyActionFromName(const QString & name) const;

  /*! \brief Setup fields from database
   *
   * This method is used by setupFromTable()
   */
  bool setupFieldsFromDatabase(const QSqlDatabase & db);

  /*! \brief Setup fields from database (Sqlite version)
   */
  bool setupFieldsFromDatabaseSqlite(const QSqlDatabase & db);

  /*! \brief Setup indexes from database
   *
   * This method is used by setupFromTable()
   */
  bool setupIndexesFromDatabase(const QSqlDatabase & db);

  /*! \brief Setup indexes from database (Sqlite version)
   */
  bool setupIndexesFromDatabaseSqlite(const QSqlDatabase & db);

  /*! \brief Setup foreign keys from database
   *
   * This method is used by setupFromTable()
   */
  bool setupForeignKeysFromDatabase(const QSqlDatabase & db);

  /*! \brief Setup foreign keys from database (Sqlite version)
   */
  bool setupForeignKeysFromDatabaseSqlite(const QSqlDatabase & db);

  uint pvIsTemporary : 1;
  QString pvDatabaseName;
  QString pvTableName;
  mdtSqlCollation pvCollation;
  QString pvStorageEngineName;
  mdtSqlPrimaryKey pvPrimaryKey;
  QList<mdtSqlField> pvFields;
  QList<mdtSqlIndex> pvIndexes;
  QList<mdtSqlForeignKey> pvForeignKeys;
  
  mdtSqlDriverType pvDriverType;  /// \todo Obselete
  mutable mdtError pvLastError;
};

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_H
