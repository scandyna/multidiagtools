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
#ifndef MDT_SQL_DATABASE_COPIER_THREAD_H
#define MDT_SQL_DATABASE_COPIER_THREAD_H

#include "mdtSqlDatabaseCopierMapping.h"
#include "mdtSqlDatabaseCopierTableMapping.h"
#include "mdtProgressValue.h"
#include "mdtError.h"
#include <QThread>
#include <QString>
#include <QSqlDatabase>
#include <atomic>
#include <vector>
#include <cstdint>

/*! \brief Worker thread for SQL database copy
 *
 * This class is used by mdtSqlDatabaseCopierDialog,
 *  witch should be used by applications.
 */
class mdtSqlDatabaseCopierThread : public QThread
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDatabaseCopierThread(QObject* parent = nullptr);

  /*! \internal Copy disabled
   */
  mdtSqlDatabaseCopierThread(const mdtSqlDatabaseCopierThread &) = delete;

  /*! \internal Move disabled
   */
  mdtSqlDatabaseCopierThread(const mdtSqlDatabaseCopierThread &&) = delete;

  /*! \brief Copy tables of given database mapping
   *
   * Will copy data of source tables to destination tables regarding given mapping.
   *
   * Databases objects contained in mapping will be used to get needed informations
   *  (such as driver name, database name, etc...).
   *  Internally, new connections are created (this is required because of multi-threading).
   *
   * \warning It is recommanded that no connection refers to one of the
   *           databases on witch the schema will be copied.
   *           This check is not done here, and paying no attention to
   *           this case can conduct to undefined behaviour.
   * \note If mapping's source and destination database objects refers to the same database,
   *        only one connection will be created for the copy.
   *        In other words, this is not a problem.
   *
   * \pre Thread must be stopped before calling this function
   */
  void copyData(const mdtSqlDatabaseCopierMapping & mapping);

 public slots:

  /*! \brief Abort database copy
   */
  void abort();

 signals:

  /*! \brief Emitted when table copy progress was updated
   */
  void tableCopyProgressChanged(int dbMappingModelRow, int progress);

  /*! \brief Emitted when table copy status was updated
   */
  void tableCopyStatusChanged(int dbMappingModelRow, int status);

  /*! \brief Emitted when a error occured during a table copy
   */
  void tableCopyErrorOccured(int dbMappingModelRow, mdtError error);

  /*! \brief Emitted when global copy progress range changed
   */
  void globalProgressRangeChanged(int min, int max);

  /*! \brief Emitted when global copy progress value changed
   */
  void globalProgressValueChanged(int progress);

  /*! \brief Emitted when a global error occured
   */
  void globalErrorOccured(mdtError error);

 private:

  /*! \brief Thread function implementation
   */
  void run();

  /*! \brief Create database connection based on given dbInfo
   */
  QSqlDatabase createConnection(const QSqlDatabase & dbInfo);

  /*! \brief Check if dbA and dbB are refering to the same database
   */
  bool isSameDatabase(const QSqlDatabase & dbA, const QSqlDatabase & dbB) const;

  /*! \brief Calculate size of each table in source database
   */
  void calculateTableSizes(const QSqlDatabase & sourceDatabase);

  /*! \brief Get size (row count) of source table for given row
   */
  int64_t tableSize(int dbMappingModelRow) const;

  /*! \brief Get total copy size
   */
  int64_t getTotalCopySize() const;

  /*! \brief Copy source table to destination table regarding table mapping
   */
  bool copyTable(const mdtSqlDatabaseCopierTableMapping & tm, int dbMappingModelRow,
                 const QSqlDatabase & sourceDatabase, const QSqlDatabase & destinationDatabase,
                 mdtProgressValue<int64_t> & globalProgress);

  std::atomic<bool> pvAbort;
  mdtSqlDatabaseCopierMapping pvMapping;
  std::vector<int64_t> pvTableSizeList;   // Contains items (or row) count for each table in source database
};

#endif // #ifndef MDT_SQL_DATABASE_COPIER_THREAD_H
