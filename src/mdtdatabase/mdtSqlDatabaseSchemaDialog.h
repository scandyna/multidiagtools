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
#ifndef MDT_SQL_DATABASE_SCHEMA_DIALOG_H
#define MDT_SQL_DATABASE_SCHEMA_DIALOG_H

#include "ui_mdtSqlDatabaseSchemaDialog.h"
#include "mdtSqlDatabaseSchema.h"
#include "mdtError.h"
#include <QDialog>
#include <QSqlDatabase>

class mdtSqlDatabaseSchemaModel;
class mdtSqlDatabaseSchemaThread;

/*! \brief Permit to create SQL database schema
 */
class mdtSqlDatabaseSchemaDialog : public QDialog, Ui::mdtSqlDatabaseSchemaDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDatabaseSchemaDialog(QWidget *parent = nullptr);

  /*! \internal Copy disabled
   */
  mdtSqlDatabaseSchemaDialog(const mdtSqlDatabaseSchemaDialog & other) = delete;

  /*! \brief Set the database schema
   */
  void setSchema(const mdtSqlDatabaseSchema & s);

 private slots:

  /*! \brief Select (or create) database
   */
  void selectDatabase();

  /*! \brief Create schema
   */
  void createSchema();

  /*! \brief Update progress of given object
   */
  void updateObjectProgress(int objectCategory, QString objectName, int progress);

  /*! \brief Will update status for given object
   */
  void setObjectStatus(int objectCategory, QString objectName, int status);

  /*! \brief Will update status to Error and set error text as tooltip
   */
  void setObjectError(int objectCategory, QString objectName, mdtError error);

  /*! \brief Update global progress
   */
  void updateGlobalProgress(int progress);
  
  void onThreadFinished();

  /*! \brief Reciver for a global error generated from worker thread
   */
  void onThreadGlobalErrorOccured(mdtError error);

 private:

  /*! \brief Check if selected database is open
   *
   * If selected database is open, or a connection
   *  that refers to the same database is open,
   *  the user will be warned.
   *
   * \return true if no connection refering to selected database is open,
   *          or user accepted to continue, false otherwise.
   */
  bool assureNoOpenConnectionToDatabase();

  mdtSqlDatabaseSchemaModel *pvModel;
  mdtSqlDatabaseSchemaThread *pvThread;
  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_SQL_DATABASE_SCHEMA_DIALOG_H
