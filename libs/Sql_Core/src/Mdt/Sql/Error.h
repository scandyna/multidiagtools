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
#ifndef MDT_SQL_ERROR_H
#define MDT_SQL_ERROR_H

#include "Mdt/Error.h"
#include <QSqlError>
#include <QString>

/*! \brief Get a Mdt::Error from last error of given query
 *
 * \sa mdtErrorFromQSqlQueryQ()
 */
#define mdtErrorFromQSqlQuery(query, className) Mdt::Sql::Error::fromQSqlQuery(query, QString::fromLocal8Bit(__FILE__), __LINE__, QString::fromLatin1(className), QString::fromLatin1(__FUNCTION__))

/*! \brief Get a Mdt::Error from last error of given query
 *
 * This version avoid giving class name,
 *  but only works for QObject subclass.
 *
 * \sa mdtErrorFromQSqlQuery()
 */
#define mdtErrorFromQSqlQueryQ(query, obj) Mdt::Sql::Error::fromQSqlQuery(query, QString::fromLocal8Bit(__FILE__), __LINE__, obj, QString::fromLatin1(__FUNCTION__))

/*! \brief Get a Mdt::Error from last error in \a queryModel
 *
 * \sa mdtErrorFromQSqlQueryModelQ()
 */
#define mdtErrorFromQSqlQueryModel(queryModel, className) Mdt::Sql::Error::fromQSqlQueryModel(queryModel, QString::fromLocal8Bit(__FILE__), __LINE__, QString::fromLatin1(className), QString::fromLatin1(__FUNCTION__))

/*! \brief Get a Mdt::Error from last error in \a queryModel
 *
 * This version avoid giving class name,
 *  but only works for QObject subclass.
 *
 * \sa mdtErrorFromQSqlQueryModel()
 */
#define mdtErrorFromQSqlQueryModelQ(queryModel, obj) Mdt::Sql::Error::fromQSqlQueryModel(queryModel, QString::fromLocal8Bit(__FILE__), __LINE__, obj, QString::fromLatin1(__FUNCTION__))

/*! \brief Get a Mdt::Error from last error of given database
 *
 * \sa mdtErrorFromQSqlDatabaseQ()
 */
#define mdtErrorFromQSqlDatabase(db, className) Mdt::Sql::Error::fromQSqlDatabase(db, QString::fromLocal8Bit(__FILE__), __LINE__, QString::fromLatin1(className), QString::fromLatin1(__FUNCTION__))

/*! \brief Get a Mdt::Error from last error of given database
 *
 * This version avoid giving class name,
 *  but only works for QObject subclass.
 *
 * \sa mdtErrorFromQSqlDatabase()
 */
#define mdtErrorFromQSqlDatabaseQ(db, obj) Mdt::Sql::Error::fromQSqlDatabase(db, QString::fromLocal8Bit(__FILE__), __LINE__, obj, QString::fromLatin1(__FUNCTION__))

class QSqlQuery;
class QSqlDatabase;
class QSqlQueryModel;
class QObject;

namespace Mdt{ namespace Sql{

  /*! \brief Translate Sql errors to Mdt::Error
   *
   * \sa mdtErrorFromQSqlQuery()
   * \sa mdtErrorFromQSqlQueryQ()
   * \sa mdtErrorFromQSqlQueryModel()
   * \sa mdtErrorFromQSqlQueryModelQ()
   * \sa mdtErrorFromQSqlDatabase()
   * \sa mdtErrorFromQSqlDatabaseQ()
   */
  class Error
  {
   public:

    /*! \brief Get a Mdt::Error from last error in \a query
     *
     * \sa mdtErrorFromQSqlQuery()
     */
    static Mdt::Error fromQSqlQuery(const QSqlQuery & query, const QString & file, int line, const QString & className, const QString & functionName);

    /*! \brief Get a Mdt::Error from last error in \a query
     *
     * \sa mdtErrorFromQSqlQueryQ()
     */
    static Mdt::Error fromQSqlQuery(const QSqlQuery & query, const QString & file, int line, const QObject * const obj, const QString & functionName);

    /*! \brief Get a Mdt::Error from last error in \a queryModel
     *
     * \sa mdtErrorFromQSqlQueryModel()
     */
    static Mdt::Error fromQSqlQueryModel(const QSqlQueryModel & queryModel, const QString & file, int line, const QString & className, const QString & functionName);

    /*! \brief Get a Mdt::Error from last error in \a queryModel
     *
     * \sa mdtErrorFromQSqlQueryModel()
     */
    static Mdt::Error fromQSqlQueryModel(const QSqlQueryModel * const queryModel, const QString & file, int line, const QString & className, const QString & functionName);

    /*! \brief Get a Mdt::Error from last error in \a queryModel
     *
     * \sa mdtErrorFromQSqlQueryModelQ()
     */
    static Mdt::Error fromQSqlQueryModel(const QSqlQueryModel & queryModel, const QString & file, int line, const QObject * const obj, const QString & functionName);

    /*! \brief Get a Mdt::Error from last error in \a queryModel
     *
     * \sa mdtErrorFromQSqlQueryModelQ()
     */
    static Mdt::Error fromQSqlQueryModel(const QSqlQueryModel * const queryModel, const QString & file, int line, const QObject * const obj, const QString & functionName);

    /*! \brief Get a Mdt::Error from last error in db
     *
     * \sa mdtErrorFromQSqlDatabase()
     */
    static Mdt::Error fromQSqlDatabase(const QSqlDatabase & db, const QString & file, int line, const QString & className, const QString & functionName);

    /*! \brief Get a Mdt::Error from last error in db
     *
     * \sa mdtErrorFromQSqlDatabaseQ()
     */
    static Mdt::Error fromQSqlDatabase(const QSqlDatabase & db, const QString & file, int line, const QObject * const obj, const QString & functionName);

    /*! \brief Get error level from QSqlError type
     */
    static Mdt::Error::Level levelFromQSqlErrorType(QSqlError::ErrorType typte);

   private:

    /*! \brief Call QObject::tr()
     */
    static QString tr(const char *sourceText);
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ERROR_H
