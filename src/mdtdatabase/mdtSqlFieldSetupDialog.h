/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_SQL_FIELD_SETUP_DIALOG_H
#define MDT_SQL_FIELD_SETUP_DIALOG_H

#include "ui_mdtSqlFieldSetupDialog.h"
#include "mdtSqlFieldSetupWidget.h"
#include <QDialog>


/*! \brief SQL field setup dialog
 */
class mdtSqlFieldSetupDialog : public QDialog, Ui::mdtSqlFieldSetupDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlFieldSetupDialog(QWidget* parent = nullptr);

  /*! \brief Set edition mode
   */
  void setEditionMode(mdtSqlFieldSetupEditionMode mode)
  {
    wSetupWidget->setEditionMode(mode);
  }

  /*! \brief Set table
   *
   * Will also reverse table to list all aviable fields
   */
  bool setTable(const QString & tableName, QSqlDatabase db)
  {
    return wSetupWidget->setTable(tableName, db);
  }

  /*! \brief Set table
   *
   * Will also list all aviable fields
   */
  bool setTable(const mdtSqlTableSchema & tableSchema, QSqlDatabase db)
  {
    return wSetupWidget->setTable(tableSchema, db);
  }

 private:

  Q_DISABLE_COPY(mdtSqlFieldSetupDialog)

};

#endif // #ifndef MDT_SQL_FIELD_SETUP_DIALOG_H
