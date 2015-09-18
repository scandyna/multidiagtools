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
#ifndef MDT_SQL_DATABASE_COPIER_DIALOG_H
#define MDT_SQL_DATABASE_COPIER_DIALOG_H

#include "ui_mdtSqlDatabaseCopierDialog.h"
#include "mdtError.h"
#include <QDialog>

class mdtSqlDatabaseCopierMappingModel;

/*! \brief Dialog that provides database copier tool
 */
class mdtSqlDatabaseCopierDialog : public QDialog, Ui::mdtSqlDatabaseCopierDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDatabaseCopierDialog(QWidget *parent = nullptr);

  /*! \internal Copy diabled
   */
  mdtSqlDatabaseCopierDialog(const mdtSqlDatabaseCopierDialog & other) = delete;

 private slots:

  /*! \brief Select source database
   */
  void selectSourceDatabase();

  /*! \brief Select destination database
   */
  void selectDestinationDatabase();

  /*! \brief Reset mapping
   */
  void resetMapping();

  /*! \brief Map by name
   */
  void mapByName();

 private:

  /*! \brief Display error
   */
  void displayError(const mdtError & error);

  mdtSqlDatabaseCopierMappingModel *pvMappingModel;
};

#endif // #ifndef MDT_SQL_DATABASE_COPIER_DIALOG_H
