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
#ifndef MDT_SQL_DATABASE_SCHEMA_THREAD_H
#define MDT_SQL_DATABASE_SCHEMA_THREAD_H

#include "mdtSqlDatabaseSchema.h"
#include "mdtSqlTableSchema.h"
#include "mdtSqlTablePopulationSchema.h"
#include "mdtSqlViewSchema.h"
#include "mdtSqlTriggerSchema.h"
#include "mdtError.h"
#include <QThread>
#include <QString>
#include <QSqlDatabase>
#include <QList>
#include <atomic>

/*! \brief Worker thread for SQL database schema creation
 *
 * This class is used by mdtSqlDatabaseSchemaDialog, wich should be used
 *  by applications.
 */
class mdtSqlDatabaseSchemaThread : public QThread
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDatabaseSchemaThread(QObject *parent = nullptr);

  /*! \internal Copy disabled
   */
  mdtSqlDatabaseSchemaThread(const mdtSqlDatabaseSchemaThread & other) = delete;

  /*! \brief Create given database schema
   *
   * Will copy given schema and start processing in background (thread is automatically started).
   *  This function also returns directly.
   *
   * Given database object will be used to get needed informations
   *  (such as driver name, database name, etc...).
   *  Internally, a new connection is created, based on informations contained in given db object
   *  (this is required because of multi-threading).
   *
   * \warning It is recommanded that no connection refers to the
   *           database on witch the schema will be created.
   *           This check is not done here, and paying no attention to
   *           this case can conduct to undefined behaviour.
   *
   * \pre Thread must be stopped before calling this function
   */
  void createSchema(const mdtSqlDatabaseSchema & s, const QSqlDatabase & db);

  /*! \brief Create given database schema
   *
   * This version will not run a separate thread
   *  and should not be used in GUI applications.
   *  Internally, this function is mainly used in
   *  unit tests.
   *
   * Given database object is directly used for database creation.
   *
   * \pre db must be valid (its driver must be loaded) and open
   */
  bool createSchemaBlocking(const mdtSqlDatabaseSchema & s, const QSqlDatabase & db);

 public slots:

  /*! \brief Abort database schema creation
   */
  void abort();

 signals:

  /*! \brief Emitted when a object progress was updated
   */
  void objectProgressChanged(int objectCategory, QString objectName, int progress);

  /*! \brief Emitted when a object was successfully created
   */
  void objectStatusChanged(int objectCategory, QString objectName, int status);

  /*! \brief Emitted when a error occured for a specific object
   */
  void objectErrorOccured(int objectCategory, QString objectName, mdtError error);

  /*! \brief Emittes when global progress changed
   */
  void globalProgressChanged(int progress);

  /*! \brief Emitted when a error occured
   */
  void globalErrorOccured(mdtError error);

 private:

  void run();

  /*! \brief Create database connection based on pvDatabaseInfo
   */
  QSqlDatabase createConnection();

  /*! \brief Create tables
   */
  void createTables(QList<mdtSqlTableSchema> & tables, const QSqlDatabase & db,
                    double & globalProgress, double globalProgressStep);

  /*! \brief Create a table
   */
  bool createTable(const mdtSqlTableSchema & ts, const QSqlDatabase & db);

  /*! \brief Populate tables
   */
  void populateTables(const QList<mdtSqlTablePopulationSchema> & tablePopulations, const QSqlDatabase & db,
                      double & globalProgress, double globalProgressStep);

  /*! \brief Populate a table
   */
  bool populateTable(const mdtSqlTablePopulationSchema & tps, const QSqlDatabase & db);

  /*! \brief Create views
   */
  void createViews(const QList<mdtSqlViewSchema::Schema> & views, const QSqlDatabase & db,
                   double & globalProgress, double globalProgressStep);

  /*! \brief Create a view
   */
  bool createView(const mdtSqlViewSchema::Schema & vs, const QSqlDatabase & db);

  /*! \brief Create triggers
   */
  void createTriggers(const QList<mdtSqlTriggerSchema> & triggers, const QSqlDatabase & db,
                      double & globalProgress, double globalProgressStep);

  /*! \brief Create a trigger
   */
  bool createTrigger(const mdtSqlTriggerSchema & trigger, const QSqlDatabase & db);

  std::atomic<bool> pvAbort;
  mdtSqlDatabaseSchema pvSchema;
  QSqlDatabase pvDatabaseInfo;    // Only to store database informations, never use it in thraed (other than GUI).
};

#endif // #ifndef MDT_SQL_DATABASE_SCHEMA_THREAD_H

