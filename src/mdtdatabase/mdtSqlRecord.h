/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_SQL_RECORD_H
#define MDT_SQL_RECORD_H

#include "mdtError.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>

/*! \brief Database record container class
 *
 * This class can be useful to create data storage classes in application.
 *
 * Based on QSqlRecord , all field attributes and values are stored as QSqlField objects.
 *
 * To set a value, use setValue(int, const QVariant &) or setValue(const QString &, const QVariant &).
 *  To check if a value was set, use hasValue(int) or hasValue(const QString &).
 *  To clear values and hasValue flags, use clearValues().
 *
 * A possible way to create a sub class could be:
 *  - Create fields that are relevant, using addField() , or addAllFields().
 *  - On error, use clear()  (from QSqlRecord).
 *  - The user can also check if data record instance is valid by using
 *     isEmpty() (from QSqlRecord) and lastError().
 */
class mdtSqlRecord
{
 public:

  /*! \brief Constructor
   */
  mdtSqlRecord();

  /*! \brief Build a mdtSqlRecord on the base of a QSqlRecord
   */
  mdtSqlRecord(const QSqlRecord & sqlRecord);

  /*! \brief Destructor
   */
  virtual ~mdtSqlRecord();

  /*! \brief Get QSqlRecord version
   */
  operator QSqlRecord();

  /*! \brief Add a field to record
   *
   * Will use given db object and tableName to get requested fieldName attributes,
   *  and add it to fields list using QSqlRecord::append() method.
   *
   * \param fieldName Name of the field to add.
   * \param tableName Name of table in database (used to get fields attributes).
   * \param db Database to use. Pass a empty object ( QSqlDatabase() ) to use default database.
   *            (This parameter is used to get fields attributes).
   * \return False if database is closed, if the table or field does not exists.
   *          Error is also available with lastError().
   */
  bool addField(const QString & fieldName, const QString & tableName, const QSqlDatabase & db);

  /*! \brief Add all fields of given table to record
   *
   * \param tableName Name of table in database.
   * \param db Database to use.
   * \sa addField()
   */
  bool addAllFields(const QString & tableName, const QSqlDatabase & db);

  /*! \brief Returns the number of fields in the record.
   */
  inline int count() const { return pvRecord.count(); }

  /*! \brief Returns true if there are no fields in the record; otherwise returns false.
   */
  inline bool isEmpty() const { return pvRecord.isEmpty(); }

  /*! \brief Returns the field at position index.
   *
   * If the index is out of range, function returns a default-constructed value.
   */
  inline QSqlField field(int index) const { return pvRecord.field(index); }

  /*! \brief Returns the field called name.
   */
  inline QSqlField field(const QString & name) const { return pvRecord.field(name); }

  /*! \brief Returns the position of the field called name within the record, or -1 if it cannot be found.
   *
   * Field names are not case-sensitive. If more than one field matches, the first one is returned.
   */
  inline int indexOf(const QString & name) const { return pvRecord.indexOf(name); }

  /*! \brief Returns true if there is a field in the record called name; otherwise returns false.
   */
  inline bool contains(const QString & name) const { return pvRecord.contains(name); }

  /*! \brief Get a list with all field names
   */
  QStringList fieldNames() const;

  /*! \brief Check if a value was set
   *
   * By insertion or edition in database, it can be important to know
   *  if a field must be updated or not.
   *  Because a Null value can be valid, it's not possible to use this flag.
   *  This method returns true once a value was set in given field.
   *
   * To clear this flag, use clearValues().
   *
   * Note: internally, QSqlRecord::isGenerated() is used.
   */
  inline bool hasValue(int fieldIndex) const { return pvRecord.isGenerated(fieldIndex); }

  /*! \brief Check if a value was set
   *
   * \sa hasValue(int)
   */
  inline bool hasValue(const QString & fieldName) const { return pvRecord.isGenerated(fieldName); }

  /*! \brief Set value
   *
   * Set the hasValue flag and set value.
   */
  void setValue(int fieldIndex, const QVariant & val);

  /*! \brief Set value
   *
   * \sa setValue(int)
   */
  void setValue(const QString & fieldName, const QVariant & val);

  /*! \brief Returns the value of the field located at position index in the record.
   *
   * If index is out of bounds, an invalid QVariant is returned.
   */
  inline QVariant value(int index) const { return pvRecord.value(index); }

  /*! \brief Returns the value of the field called name in the record.
   *
   * If field name does not exist an invalid variant is returned.
   */
  inline QVariant value(const QString & name) const { return pvRecord.value(name); }

  /*! \brief Removes all the record's fields.
   */
  virtual void clear();

  /*! \brief Clear values
   *
   * Will clear value and hasValue flag for all fields.
   */
  virtual void clearValues();

  /*! \brief Get last error
   */
  mdtError lastError() const;

  /*! \brief Get a list with field indexes that have a value
   *
   * Note: each time this method is called,
   *  the list will be generated.
   *
   * \sa hasValue()
   */
  QVector<int> fieldIndexesWithValue() const;

  /*! \brief Get SQL statement for insertion with QSqlQuery
   *
   * Will generate SQL statement for insertion
   *  in given table name.
   *  The statement works with QSqlQuery values binding
   *  with positional placeholders.
   *  Example of a returned statement: INSERT INTO table (Id_PK, Name) VALUES (?, ?).
   *
   *  Only fields that have a value are added in SQL statement.
   */
  QString sqlForInsert(const QString & tableName) const;

  /*! \brief Get SQL statement for update with QSqlQuery
   *
   * Will generate SQL statement for update
   *  in given table name.
   *  The statement works with QSqlQuery values binding
   *  with positional placeholders.
   *  Example of a returned statement: UPDATE table SET Id_PK=?,Name=?.
   *
   *  Only fields that have a value are added in SQL statement.
   *
   * \param tableName Name of table to update.
   * \param matchData For each field in matchData, a condition of type fieldName=fieldValue is added.
   *            This will determine witch row to update.
   */
  QString sqlForUpdate(const QString & tableName, const QSqlRecord & matchData) const;

  /*! \brief Get SQL data delimiter for given data type
   */
  static QString sqlDataDelimiter(QVariant::Type type);

  /*! \brief Get information about data that has a value to update (for debug)
   */
  QString dataToUpdateInfo() const;

 protected:

  mdtError pvLastError;

 private:

  QSqlRecord pvRecord;
};

#endif // #ifndef MDT_SQL_RECORD_H
