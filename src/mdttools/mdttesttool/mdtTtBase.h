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
#ifndef MDT_TT_BASE_H
#define MDT_TT_BASE_H

#include "mdtError.h"
#include "mdtSqlRecord.h"
#include "mdtSqlTableSelection.h"
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QSqlRecord>
#include <QObject>
#include <QSqlQuery>

/*! \brief Base class for helper class
 *
 * To edit entities (links, Article, Unit, ...) ,
 *  dedicated classes helps for interaction with database .
 *
 * This class is a base that regroups common methods .
 */
class mdtTtBase : public QObject
{
 public:

  /*! \brief Constructor
   */
  mdtTtBase(QObject *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  virtual ~mdtTtBase();

  /*! \brief Get database instance
   */
  QSqlDatabase database()
  {
    return pvDatabase;
  }

  /*! \brief Get last error
   */
  mdtError lastError();

  /*! \brief Add a record to given table
   *
   * Will only get values from fields that have a value.
   *  For this, mdtSqlRecord::hasValue() is used.
   */
  bool addRecord(const mdtSqlRecord & record, const QString & tableName);

  /*! \brief Add a record to given table
   *
   * Works the same way as addRecord(const mdtSqlRecord&, const QString&) ,
   *  but uses given QSqlQuery object.
   *
   * This can be useful, for example, if QSqlQuery::lastInsertId() must
   *  be used by insersion in multiple tables.
   */
  bool addRecord(const mdtSqlRecord & record, const QString & tableName, QSqlQuery & query);

  /*! \brief Add a list of records to given table
   *
   * If singleTransaction is true, a transaction will be used for
   *  adding all records. If you need to handle a transaction
   *  outside, set this parameter to false.
   */
  template <typename T>
  bool addRecordList(const QList<T> & recordList, const QString & tableName, bool singleTransaction = false)
  {
    int i;

    if(singleTransaction){
      if(!beginTransaction()){
        return false;
      }
    }
    for(i = 0; i < recordList.size(); ++i){
      if(!addRecord(recordList.at(i), tableName)){
        if(singleTransaction){
          rollbackTransaction();
        }
        return false;
      }
    }
    if(singleTransaction){
      if(!commitTransaction()){
        return false;
      }
    }

    return true;  
  }

  /*! \brief Get data of type T for given sql statement
   *
   * Note that this method will not check if given SQL statement
   *  makes a SELECT, INSERT, etc..
   *  It is simply executed.
   *
   * T can be QSqlRecord, QVariant or a type that QVariant can handle.
   *
   * \param sql SQL statement
   * \param ok Will contain true on success, false else (in that case, lastError() will contain error ).
   * \param expectedFields If not empty, it will be checked that query result
   *                        contains all listed fields, and fail if some fields are missing.
   * \pre If T is another type than QSqlRecord, sql must select exactly on field.
   */
  template <typename T>
  T getData(const QString & sql, bool & ok, const QStringList & expectedFields = QStringList())
  {
    T data{};
    QSqlQuery query(pvDatabase);

    if(!processGetData(sql, query, expectedFields)){
      ok = false;
      return data;
    }
    if(query.next()){
      QSqlRecord record = query.record();
      // If query has more items, this is a error (by calling this function we expect max 1 record)
      if(query.next()){
        pvLastError.setError(tr("Given query returned more than 1 record. SQL: '") + sql + tr("'"), mdtError::Error);
        MDT_ERROR_SET_SRC(pvLastError, "mdtTtBase");
        pvLastError.commit();
        ok = false;
        return data;
      }
      // Fill data
      convertRecord(data, record);
    }
    ok = true;

    return data;
  }

  /*! \brief Get a list of data of type T for given sql statement
   *
   * Note that this method will not check if given SQL statement
   *  makes a SELECT, INSERT, etc..
   *  It is simply executed.
   *
   * T can be QSqlRecord, QVariant or a type that QVariant can handle.
   *
   * \param sql SQL statement
   * \param ok Will contain true on success, false else (in that case, lastError() will contain error ).
   * \param expectedFields If not empty, it will be checked that query result
   *                        contains all listed fields, and fail if some fields are missing.
   * \pre If T is another type than QSqlRecord, sql must select exactly on field.
   */
  template <typename T>
  QList<T> getDataList(const QString & sql, bool & ok, const QStringList & expectedFields = QStringList())
  {
    QList<T> dataList;
    QSqlQuery query(pvDatabase);
    T data;

    if(!processGetData(sql, query, expectedFields)){
      ok = false;
      return dataList;
    }
    while(query.next()){
      convertRecord(data, query.record());
      dataList.append(data);
    }
    ok = true;

    return dataList;
  }

  /*! \brief Update data for given table
   *
   * \param tableName Name of table in witch data must be updated.
   * \param record Record containing data to update.
   * \param matchData Records that contains conditions.
   *               For each field, a condition of type fieldName = fieldValue is added.
   */
  bool updateRecord(const QString & tableName, const mdtSqlRecord & record, const QSqlRecord & matchData);

  /*! \brief Update data for given table
   *
   * \param tableName Name of table in witch data must be updated.
   * \param record Record containing data to update.
   * \param matchField Field that contains condition.
   * \param matchData Data that contains conditions.
   */
  bool updateRecord(const QString & tableName, const mdtSqlRecord & record, const QString & matchField, const QVariant & matchData);

  /*! \brief Update data for given table
   *
   * \param tableName Name of table in witch data must be updated.
   * \param record Record containing data to update.
   * \param matchField1 Field for witch key1 must match.
   * \param key1 Key that matches matchField1.
   * \param matchField2 Field for witch key2 must match.
   * \param key2 Key that matches matchField2.
   */
  bool updateRecord(const QString & tableName, const mdtSqlRecord & record, 
                    const QString & matchField1, const QVariant & key1, const QString & matchField2, const QVariant & key2);

  /*! \brief Remove data from given table
   *
   * This method can be used with result of mdtSqlTableWidget::indexListOfSelectedRows()
   *  (the indexes part) .
   *
   * \param tableName Name of table in witch data must be removed .
   * \param fields List of fields. Be shure that this list matches exactly with given indexes .
   * \param indexes In each index, the column will be used to find field name, and data is used for row condition .
   */
  bool removeData(const QString & tableName, const QStringList & fields, const QModelIndexList & indexes);

  /*! \brief Remove data from given table
   *
   * This method works for a single field .
   *
   * \sa removeData(const QString &, const QStringList &, const QModelIndexList &)
   */
  bool removeData(const QString & tableName, const QString & fieldName, const QModelIndexList & indexes);

  /*! \brief Remove data from given table
   *
   * \param tableName Name of table in witch data must be removed .
   * \param fieldName Name of field that contains delete condition .
   * \param matchData Data that match delete condition .
   */
  bool removeData(const QString & tableName, const QString & fieldName, const QVariant & matchData);

  /*! \brief Remove data from given table
   *
   * This variant is typically usefull for tables containing 2 primary keys.
   *  Will only remove rows that match the 2 fields (condition 1 AND condition 2).
   *
   * \param tableName Name of table in witch data must be removed .
   * \param fieldName1 Name of first field that contains delete condition .
   * \param matchData1 Data that match delete condition for first field.
   * \param fieldName2 Name of second field that contains delete condition .
   * \param matchData2 Data that match delete condition for second field.
   */
  bool removeData(const QString & tableName, const QString & fieldName1, const QVariant & matchData1, const QString & fieldName2, const QVariant & matchData2);

  /*! \brief Remove data in given selection from given table
   *
   * A SQL statement will be built with a filter (WHERE clause).
   *  For a given row, each field will be added with a AND clause.
   *  Each row will be separated with a OR clause.
   *
   * For example, if we want to remove data from AB_tbl, and have following data in the selection:
   *
   *  <table border="1" cellpadding="5">
   *    <tr><th>Row</th><th>Field name</th><th>Data</th></tr>
   *    <tr><th>0</th><td>A_Id_FK</td><td>1</td></tr>
   *    <tr><th>0</th><td>B_Id_FK</td><td>10</td></tr>
   *    <tr><th>1</th><td>A_Id_FK</td><td>2</td></tr>
   *    <tr><th>1</th><td>B_Id_FK</td><td>20</td></tr>
   *  </table>
   *
   * the query will be: DELETE FROM AB_tbl WHERE (A_Id_FK=1 AND B_Id_FK=10) OR (A_Id_FK=2 AND B_Id_FK=20)
   *
   * \param tableName Name of table in witch data must be removed.
   * \param s Selection (see detailed description).
   * \param handleTransaction If true, a transaction will be initiated internally.
   */
  bool removeData(const QString & tableName, const mdtSqlTableSelection & s, bool handleTransaction);

 protected:

  /*! \brief Beginn manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   *
   * \deprecated See mdtSqlTransaction
   */
  bool beginTransaction();

  /*! \brief Rollback manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   * 
   * \deprecated See mdtSqlTransaction
   */
  bool rollbackTransaction();

  /*! \brief Commit manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   * 
   * \deprecated See mdtSqlTransaction
   */
  bool commitTransaction();

  /*! \brief Get SQL data delimiter for given data type
   */
  QString sqlDataDelimiter(QVariant::Type type);

  mdtError pvLastError;

 private:

  /*! \brief Rund query, check errors and missing fields
   */
  bool processGetData(const QString & sql, QSqlQuery & query, const QStringList & expectedFields);

  /*! \brief Convert a record to QSqlRecord
   */
  inline void convertRecord(QSqlRecord & recOut,  const QSqlRecord & record) const {
    recOut = record;
  }

  /*! \brief Convert a record to QVariant
   */
  inline void convertRecord(QVariant & varOut,  const QSqlRecord & record) const {
    Q_ASSERT(record.count() == 1);
    varOut = record.value(0);
  }

  /*! \brief Convert a record to T
   */
  template <typename T>
  inline void convertRecord(T & out,  const QSqlRecord & record) const {
    Q_ASSERT(record.count() == 1);
    out = record.value(0).value<T>();
  }

  Q_DISABLE_COPY(mdtTtBase)

  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_TT_BASE_H
