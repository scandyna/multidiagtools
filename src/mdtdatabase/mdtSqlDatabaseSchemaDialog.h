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
#include <QDialog>

class mdtSqlDatabaseSchemaModel;

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

 private:

  mdtSqlDatabaseSchemaModel *pvModel;
};

#endif // #ifndef MDT_SQL_DATABASE_SCHEMA_DIALOG_H
