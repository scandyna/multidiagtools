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
#ifndef MDT_SQL_SCHEMA_DRIVER_IMPLEMENTATION_INTERFACE_H
#define MDT_SQL_SCHEMA_DRIVER_IMPLEMENTATION_INTERFACE_H

#include "DriverType.h"
#include "FieldType.h"
#include "FieldTypeList.h"
#include "Field.h"
#include "FieldList.h"
#include "Charset.h"
#include "Collation.h"
#include "AutoIncrementPrimaryKey.h"
#include "SingleFieldPrimaryKey.h"
#include "PrimaryKey.h"
#include "PrimaryKeyContainer.h"
#include "ForeignKey.h"
#include "Index.h"
#include "IndexList.h"
#include "Table.h"
#include "TableTemplate.h"
#include "View.h"
#include "TablePopulation.h"
#include "Trigger.h"
#include "Schema.h"
#include "Mdt/Error.h"
#include "Mdt/Expected.h"
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QMetaType>
#include <QVariant>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief SQL Schema driver implementation interface
   */
  class DriverImplementationInterface
  {
   public:

    /*! \brief Constructor
     */
    DriverImplementationInterface(const QSqlDatabase & db)
     : mDatabase(db)
    {
      Q_ASSERT(mDatabase.isValid());
    }

    /*! \brief Get QSqlDriver instance
     *
     * \post Allways returns a non null pointer
     */
    QSqlDriver *qsqlDriver() const
    {
      Q_ASSERT(mDatabase.driver() != nullptr);
      return mDatabase.driver();
    }

    /*! \brief Get driver type
     */
    virtual DriverType type() const = 0;

    /*! \brief Get a list of avaliable field type
     *
     * Default implementation returns all type defined in FieldType
     */
    virtual FieldTypeList getAvailableFieldTypeList() const;

    /*! \brief Get SQL field type from QMetaType::Type
     *
     * Can be overloaded if default implementation does not match
     */
    virtual FieldType fieldTypeFromQMetaType(QMetaType::Type qmt) const;

    /*! \brief Get SQL field type from QMetaType::Type
     *
     * \sa fieldTypeFromQMetaType() and Qt's QVariant documentation (QVariant::Type is obselete)
     */
    FieldType fieldTypeFromQVariantType(QVariant::Type qvt) const
    {
      return fieldTypeFromQMetaType(static_cast<QMetaType::Type>(qvt));
    }

    /*! \brief Get QMetaType::Type from SQL field type
     *
     * Can be overloaded if default implementation does not match
     */
    virtual QMetaType::Type fieldTypeToQMetaType(FieldType ft) const;

    /*! \brief Get QVariant::Type from SQL field type
     *
     * \sa fieldTypeToQMetaType() and Qt's QVariant documentation (QVariant::Type is obselete)
     */
    QVariant::Type fieldTypeToQVariantType(FieldType ft) const
    {
      return static_cast<QVariant::Type>(fieldTypeToQMetaType(ft));
    }

    /*! \brief Get field type from string
     *
     * Extracts the field tape name and retruns corresponding field type.
     *  For example, if fieldTypeString is VARCHAR(50),
     *  FieldType::Varchar is retruned.
     *
     * \pre fieldTypeString must not be empty
     */
    virtual FieldType fieldTypeFromString(const QString & fieldTypeString) const;

    /*! \brief Get field length from string
     *
     * Extracts the field length from fieldTypeString.
     *  For example, if fieldTypeString is VARCHAR(50),
     *  50 is returned.
     *  If fieldTypeString contains no length information,
     *  -1 is returned.
     *  If a error occured, a value < -1 is returned.
     *
     * \pre fieldTypeString must not be empty
     */
    virtual int fieldLengthFromString(const QString & fieldTypeString) const;

    /*! \brief Get default charset for current database/schema
     *
     * \note The database/schema is the one specified at connection time.
     */
    virtual Charset getDatabaseDefaultCharset() const = 0;

    /*! \brief Get collation definition
     *
     * If collation is defined, a string with COLLATE ... is returned,
     *  else a empty string.
     */
    virtual QString getCollationDefinition(const Collation & collation) const = 0;

    /*! \brief Get field definition
     */
    virtual QString getFieldDefinition(const Field & field) const = 0;

    /*! \brief Get FieldList for table from database
     *
     * \note Try to get information about a non existing table is also a error.
     * \note It seems that some DBMS accept table without any field,
     *        this is why returning a empty list on error can be confusing.
     *
     * This default implementation uses Qt to fetch informations in database.
     */
    virtual Mdt::Expected<FieldList> getTableFieldListFromDatabase(const QString & tableName) const;

    /*! \brief Get field definition of a auto increment primary key
     */
    virtual QString getPrimaryKeyFieldDefinition(const AutoIncrementPrimaryKey & pk) const = 0;

    /*! \brief Get field definition of a single field primary key
     */
    virtual QString getPrimaryKeyFieldDefinition(const SingleFieldPrimaryKey & pk) const = 0;

    /*! \brief Get primary key definition
     */
    virtual QString getPrimaryKeyDefinition(const PrimaryKey & pk) const;

    /*! \brief Get primary key for table from database
     *
     * Returned primary key type depends on how it was defined:
     *  - For a auto increment primary key, type will be AutoIncrementPrimaryKeyType
     *  - For other primary key that only refers to 1 column, type will be SingleFieldPrimaryKeyType
     *  - For other cases, type will be PrimaryKeyType
     *
     * \note If 1 column was declared as table primary key constraint,
     *        it will here be returned as SingleFieldPrimaryKey,
     *        which reflects more a column constraint.
     *       If more than 1 column where declared as primary key column constraint,
     *        it will here be returned as PrimaryKey,
     *        which reflects more a table constraint.
     *       This can look like a limitation, but reflects how schema are defined
     *        using Mdt::Sql::Schema classes.
     *        Reflecting the exact schema from a database is not supported at all.
     */
    virtual Mdt::Expected<PrimaryKeyContainer> getTablePrimaryKeyFromDatabase(const QString & tableName) const = 0;

    /*! \brief Get foreign key definition
     */
    virtual QString getForeignKeyDefinition(const ForeignKey & fk) const;

    /*! \brief Get list of foreign keys for table from database
     */
    virtual Mdt::Expected<ForeignKeyList> getTableForeignKeyListFromDatabase(const QString & tableName) const = 0;

    /*! \brief Get SQL statement to create a index
     */
    virtual QString getSqlToCreateIndex(const Index & index) const;

    /*! \brief Get SQL statement to drop a index
     */
    virtual QString getSqlToDropIndex(const Index & index) const;

    /*! \brief Get a list of idexes for table from database
     *
     * Returns only indexes that where explicitly created.
     *  Indexes automaticaly generated from a column constraint
     *  are not included here.
     */
    virtual Mdt::Expected<IndexList> getTableIndexListFromDatabase(const QString & tableName) const = 0;

    /*! \brief Get SQL statement to create a table
     */
    virtual QString getSqlToCreateTable(const Table & table) const;

    /*! \brief Get SQL statement to drop a table
     */
    virtual QString getSqlToDropTable(const Table & table) const;

    /*! \brief Create table in database
     */
    bool createTable(const Table & table);

    /*! \brief Drop table
     */
    bool dropTable(const Table & table);

    /*! \brief Create a view in database
     */
    bool createView(const View & view);

    /*! \brief Drop view
     */
    bool dropView(const View & view);

    /*! \brief Get SQL statement to create a trigger
     *
     * Can be overloaded if default implementation does not work for a specific DBMS
     */
    virtual QString getSqlToCreateTrigger(const Trigger & trigger) const;

    /*! \brief Get SQL statement to drop a trigger
     *
     * Can be overloaded if default implementation does not work for a specific DBMS
     */
    virtual QString getSqlToDropTrigger(const Trigger & trigger) const;

    /*! \brief Create trigger
     */
    bool createTrigger(const Trigger & trigger);

    /*! \brief Drop trigger
     */
    bool dropTrigger(const Trigger & trigger);

    /*! \brief Populate a table
     */
    bool populateTable(const TablePopulation & tp);

    /*! \brief Create schema
     */
    bool createSchema(const Schema & schema);

    /*! \brief Drop schema
     */
    bool dropSchema(const Schema & schema);

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   protected:

    /*! \brief Escape field name
     *
     * Returns the fieldName escaped according to the database rules.
     */
    QString escapeFieldName(const QString & fieldName) const;

    /*! \brief Escape table name
     *
     * Returns the tableName escaped according to the database rules.
     */
    QString escapeTableName(const QString & tableName) const;

    /*! \brief Escape default value
     */
    QString escapeFieldDefaultValue(const Field & field) const;

    /*! \brief Get trigger event key word
     */
    QString triggerEventKeyWord(Trigger::Event event) const;

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error) const
    {
      mLastError = error;
    }

    /*! \brief Access database object
     */
    const QSqlDatabase & database() const
    {
      return mDatabase;
    }

    /*! \brief Call QObject::tr()
     */
    static QString tr(const char *sourceText);

   private:

    QSqlDatabase mDatabase;
    mutable Mdt::Error mLastError;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_DRIVER_IMPLEMENTATION_INTERFACE_H
