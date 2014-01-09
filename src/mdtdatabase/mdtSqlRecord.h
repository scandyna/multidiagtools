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

/*! \brief Database record container class
 *
 * This class can be useful to create data storage classes in application.
 *
 * Based on QSqlRecord , all field attributes and values are stored as QSqlField objects.
 *
 * A possible way to create a sub class could be:
 *  - Create fields that are relevant, using addField()
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
