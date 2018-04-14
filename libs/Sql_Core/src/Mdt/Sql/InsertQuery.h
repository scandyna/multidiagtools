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
#ifndef MDT_SQL_INSERT_QUERY_H
#define MDT_SQL_INSERT_QUERY_H

#include "Mdt/Error.h"
#include "FieldName.h"
#include <QString>
#include <QStringList>
#include <QVector>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>

class QSqlRecord;

namespace Mdt{ namespace Sql{

  namespace Schema{

    template<typename T>
    class TableTemplate;

    class Table;
    class Field;
    class AutoIncrementPrimaryKey;

  }

  /*! \brief Insert data into a database table
   *
   * Usage with basic type checking:
   * \code
   * using Mdt::Sql::InsertQuery;
   * using Mdt::Sql::FieldName;
   *
   * InsertQuery query(db);
   * query.setTableName("Client_tbl");
   * query.addValue(FieldName("Name"), "Name 1");
   * query.addValue(FieldName("Remark"), "Remarks 1");
   * if(!query.exec()){
   *   // Error handling. query.lastError() constains a error description.
   * }
   * \endcode
   *
   * If entities are defined using Mdt::Sql::Schema,
   *  it is recommended to use typed functions:
   * \code
   * using Mdt::Sql::InsertQuery;
   *
   * MySchema::Client client;
   * InsertQuery query(db);
   *
   * query.setTable(client);
   * query.addValue(client.Name(), "Name 1");
   * query.addValue(client.Remark(), "Remarks 1");
   * if(!query.exec()){
   *   // Error handling. query.lastError() constains a error description.
   * }
   * \endcode
   */
  class InsertQuery
  {
   public:

    /*! \brief Construct a insert query that acts on db
     *
     * \pre db must be valid (must have a driver loaded)
     */
    InsertQuery(const QSqlDatabase & db);

    /*! \brief Set table name
     */
    void setTableName(const QString & name);

    /*! \brief Set table
     */
    void setTable(const Schema::Table & table);

    /*! \brief Set table
     */
    template<typename T>
    void setTable(const Schema::TableTemplate<T> & table)
    {
      setTableName(table.tableName());
    }

    /*! \brief Add a value for a specified field
     */
    void addValue(const FieldName & fieldName, const QVariant & value);

    /*! \brief Add a value for a specified field
     */
    void addValue(const Schema::Field & field, const QVariant & value);

    /*! \brief Add a value for a specified field
     */
    void addValue(const Schema::AutoIncrementPrimaryKey & field, const QVariant & value);

    /*! \brief Execute insert query
     */
    bool exec();

    /*! \brief Clear
     */
    void clear();

    /*! \brief Get last insert id
     *
     * Returns QSqlQuery::lastInsertId()
     */
    QVariant lastInsertId() const
    {
      return pvQuery.lastInsertId();
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return pvLastError;
    }

   protected:

    /*! \internal Get prepare stamenet
     *
     * Is exposed as protected for unit tests
     */
    QString getPrepareStatement() const;

   private:

    /*! \brief Escape field name
     */
    QString escapeFieldName(const QString & fieldName) const;

    /*! \brief Escape table name
     */
    QString escapeTableName(const QString & tableName) const;

    /*! \brief Call QObject::tr()
     */
    static QString tr(const char *sourceText);

    QString pvTableName;
    QStringList pvFieldNameList;
    QVector<QVariant> pvValueList;
    QSqlQuery pvQuery;
    QSqlDatabase pvDatabase;
    Mdt::Error pvLastError;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_INSERT_QUERY_H
