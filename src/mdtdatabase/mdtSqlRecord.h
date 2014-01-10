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
#include <QSqlDatabase>
#include <QString>
#include <QVariant>

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
 *
 * \sa Qt's QSqlRecord documentation
 */
class mdtSqlRecord : public QSqlRecord
{
 public:

  /*! \brief Constructor
   */
  mdtSqlRecord();

  /*! \brief Destructor
   */
  ~mdtSqlRecord();

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
  bool hasValue(int fieldIndex) const;

  /*! \brief Check if a value was set
   *
   * \sa hasValue(int)
   */
  bool hasValue(const QString & fieldName) const;

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

  /*! \brief Clear values
   *
   * Will clear value and hasValue flag for all fields.
   */
  void clearValues();

  /*! \brief Get last error
   */
  mdtError lastError() const;

 protected:

  /*! \brief Get last error for write access
   *
   * Can be used by subclass to set the last error.
   */
  mdtError & lastErrorW();

 private:

    mdtError pvLastError;
};

#endif // #ifndef MDT_SQL_RECORD_H
