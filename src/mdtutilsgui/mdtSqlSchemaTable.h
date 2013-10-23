/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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

#include <QString>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlIndex>
#include <QList>
#include <QHash>

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
  void setDriverName(const QString & name);

  /*! \brief Set database name
   */
  void setDatabaseName(const QString & name);

  /*! \brief Set table name
   *
   * If dropIfExists is set, sqlForCreateTable() will include
   *  a drop table before the create table statement.
   */
  void setTableName(const QString & name, bool dropIfExists, const QString &charset = QString(), Qt::CaseSensitivity cs = Qt::CaseInsensitive);

  /*! \brief Set the Maria DB/MySQL storage engine (XtraDB, Inno DB, ...)
   */
  void setStorageEngineName(const QString & name);

  /*! \brief Add a field
   */
  void addField(const QSqlField & field, bool isPartOfPrimaryKey);

  /*! \brief Add a index
   *
   * If name allready exists, it will not be appended
   */
  void addIndex(const QString & name, bool unique);

  /*! \brief Add a field to a index
   *
   * Field must allready be created with addField() and index also with addIndex() .
   *  If index with name indexName does not exists, or field with fieldName not exists,
   *  this method returns false.
   */
  bool addFieldToIndex(const QString & indexName, const QString & fieldName);

  /*! \brief Add a foreign key contraint
   */
  void addForeignKey(const QString & name, const QString & referingTableName, const foreignKeyAction_t & actionOnDelete, const foreignKeyAction_t & actionOnUpdate);

  /*! \brief Add a field to a foreign key
   *
   * Field must allready be created with addField() and foreign key also with addForeignKey() .
   *  If foreign key with name foreignKeyName does not exists, or field with fieldName not exists,
   *  this method returns false.
   */
  bool addFieldToForeignKey(const QString & foreignKeyName, const QString & fieldName, const QString & referingFieldName);

  /*! \brief Get SQL statement for table creation
   */
  QString sqlForCreateTable();

  /*! \brief Get SQL statement for table deleteion
   */
  QString sqlForDropTable();

 private:

  Q_DISABLE_COPY(mdtSqlSchemaTable);

  /*! \brief Build create table statement for Maria DB/MySQL
   */
  QString sqlForCreateTableMySql();

  /*! \brief Build create table statement for Sqlite
   */
  QString sqlForCreateTableSqlite();

  /*! \brief Get SQL statement for the fields part for Maria DB/MySQL
   */
  QString sqlForFieldsMySql();

  /*! \brief Get SQL statement for the fields part for Sqlite
   */
  QString sqlForFieldsSqlite();

  /*! \brief Get SQL statement for the primary key
   */
  QString sqlForPrimaryKey(const QString &delimiter);

  /*! \brief Get SQL statement for the indexes
   */
  QString sqlForIndexes(const QString &delimiter);

  /*! \brief Get SQL statement for the foreign keys
   */
  QString sqlForForeignKeys(const QString &delimiter);

  /*! \brief Get field type name for Maria DB/MySQL
   */
  QString fieldTypeNameMySql(QVariant::Type type, int length);

  /*! \brief Get field type name for Maria Sqlite
   */
  QString fieldTypeNameSqlite(QVariant::Type type, int length);

  /*! \brief Set collation regarding pvCharset and cs
   */
  ///void setCollation(Qt::CaseSensitivity cs);

  /*! \brief Get the COLLATE string for Maria DB/MySQL
   */
  QString sqlForCollateMySql();

  /*! \brief Get the COLLATE string for Sqlite
   */
  QString sqlForCollateSqlite();

  /*! \brief Get the string version of foreignKeyAction_t
   */
  QString foreignKeyActionName(foreignKeyAction_t action);

  QString pvDriverName;
  QString pvDatabaseName;
  QString pvTableName;
  QString pvCharset;
  ///QString pvCollation;
  Qt::CaseSensitivity pvCaseSensitivity;
  bool pvDropTableIfExists;
  QString pvStorageEngineName;
  QList<QSqlField> pvFields;
  QSqlIndex pvPrimaryKey;
  QHash<QString, QSqlIndex> pvIndexes;
  QHash<QString, bool> pvIndexeAtIsUnique;
  QHash<QString, mdtSqlSchemaTableForeignKeyInfo> pvForeignKeys;
};

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_H
