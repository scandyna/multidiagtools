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
#ifndef MDT_SQL_TABLE_POPULATION_SCHEMA_H
#define MDT_SQL_TABLE_POPULATION_SCHEMA_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QList>
#include <QSqlDriver>

/*! \brief Simple container that holds data to populate on a specific table
 */
class mdtSqlTablePopulationSchema
{
 public:

  /*! \brief Set identification name
   *
   * This name will not interact with the database,
   *  but is used when database schema is presented to the user.
   */
  void setName(const QString & name)
  {
    pvName = name;
  }

  /*! \brief Get identification name
   */
  QString name() const
  {
    return pvName;
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

  /*! \brief Set list of fields
   *
   * Note: if some row data where stored, they will be celared.
   */
  void setFieldNameList(const QStringList & list)
  {
    pvRowDataList.clear();
    pvFieldNameList = list;
  }

  /*! \brief Add a field name
   *
   * Note: if some row data where stored, they will be celared.
   */
  void addFieldName(const QString & name)
  {
    pvRowDataList.clear();
    pvFieldNameList.append(name);
  }

  /*! \brief Get field names count
   */
  int fieldcount() const
  {
    return pvFieldNameList.size();
  }

  /*! \brief Get list of field names
   */
  QStringList fieldNameList() const
  {
    return pvFieldNameList;
  }

  /*! \brief Access current row data
   *
   * This can be typically used to stream row data:
   * \code
   *   mdtSqlTablePopulationSchema tps;
   *   // common setup...
   *   // Add data to current row
   *   tps.currentRowData() << "A" << "B" << "C";
   *   // Commit the row
   *   tps.commitCurrentRowData();
   *   // Now we are ready to populate next row and commit it
   *   tps.currentRowData() << "D" << "E" << "F";
   *   tps.commitCurrentRowData();
   * \endcode
   */
  QVariantList & currentRowData()
  {
    return pvCurrentRowData;
  }

  /*! \brief Commit current row data
   *
   * Will also clear currentRowData
   *
   * \pre currentRowData must be the same size than fieldNameList
   */
  void commitCurrentRowData()
  {
    Q_ASSERT(pvCurrentRowData.size() == pvFieldNameList.size());
    pvRowDataList << pvCurrentRowData;
    pvCurrentRowData.clear();
  }

  /*! \brief Get count of row data
   */
  int rowDataCount() const
  {
    return pvRowDataList.size();
  }

  /*! \brief Get row data for given row
   *
   * \pre row must be in valid range
   */
  QVariantList rowData(int row) const
  {
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < pvRowDataList.size());
    return pvRowDataList.at(row);
  }

  /*! \brief Get SQL for insertion
   *
   * Return SQL statement that can be used with QSqlQuery bind values.
   *  For example, if tableName is Client_tbl and we have fields Id_PK and Name,
   *  the SQL statement will be: \" INSERT INTO Client_tbl (Id_PK, Name) VALUES (?, ?) \"
   *
   * Note: each call of this function will rebuild the SQL statement
   *
   * \pre driver must be non null pointer
   */
  QString sqlForInsert(const QSqlDriver *driver) const
  {
    Q_ASSERT( driver != nullptr);

    QString sql;
    int lastIndex = pvFieldNameList.size() - 1;

    if(lastIndex < 0){
      return sql;
    }
    sql = "INSERT INTO " + driver->escapeIdentifier(pvTableName, QSqlDriver::TableName) + " (";
    for(int i = 0; i < lastIndex; ++i){
      sql += driver->escapeIdentifier(pvFieldNameList.at(i), QSqlDriver::FieldName) + ",";
    }
    sql += driver->escapeIdentifier(pvFieldNameList.at(lastIndex), QSqlDriver::FieldName);
    sql += ") VALUES (";
    for(int i = 0; i < lastIndex; ++i){
      sql += "?,";
    }
    sql += "?)";

    return sql;
  }

  /*! \brief Clear
   */
  void clear()
  {
    pvName.clear();
    pvTableName.clear();
    pvFieldNameList.clear();
    pvRowDataList.clear();
  }

 private:

  QString pvName;
  QString pvTableName;
  QStringList pvFieldNameList;
  QVariantList pvCurrentRowData;
  QList<QVariantList> pvRowDataList;
};

#endif // #ifndef MDT_SQL_TABLE_POPULATION_SCHEMA_H
