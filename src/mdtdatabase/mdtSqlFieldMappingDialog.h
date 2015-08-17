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
#ifndef MDT_SQL_FIELD_MAPPING_DIALOG_H
#define MDT_SQL_FIELD_MAPPING_DIALOG_H

#include "ui_mdtSqlFieldMappingDialog.h"
#include "mdtSqlFieldMappingData.h"
#include "mdtError.h"
#include <QDialog>

/*! \brief Field mapping dialog
 */
class mdtSqlFieldMappingDialog : public QDialog, Ui::mdtSqlFieldMappingDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlFieldMappingDialog(QWidget *parent = nullptr);

  /*! \brief Set source edition mode
   */
  void setSourceEditionMode(mdtSqlFieldSetupEditionMode_t mode);

  /*! \brief Set source table
   *
   * Will also reverse table to list all aviable fields
   */
  bool setSourceTable(const QString & tableName, QSqlDatabase db);

  /*! \brief Set source table
   *
   * Will also list all aviable fields
   */
  bool setSourceTable(const mdtSqlSchemaTable & tableSchema, QSqlDatabase db);

  /*! \brief Set destination edition mode
   */
  void setDestinationEditionMode(mdtSqlFieldSetupEditionMode_t mode);

  /*! \brief Set destination table
   *
   * Will also reverse table to list all aviable fields
   */
  bool setDestinationTable(const QString & tableName, QSqlDatabase db);

  /*! \brief Set destination table
   *
   * Will also list all aviable fields
   */
  bool setDestinationTable(const mdtSqlSchemaTable & tableSchema, QSqlDatabase db);

  /*! \brief Set field mapping
   */
  void setMapping(const mdtSqlFieldMappingData & mapping);

  /*! \brief Get field mapping
   */
  mdtSqlFieldMappingData mapping() const
  {
    return pvMapping;
  }

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private slots:

  /*! \brief Enable/disable source splitting options widgets regarding rbNoSourceSplitting's state
   */
  void setSourceSplittingEditionEnabled(bool rbNoSourceSplittingEnabled);

  /*! \brief Enable/disable destination cat options widgets regarding rbNoDestinationCat's state
   */
  void setDestinationCatEditionEnabled(bool rbNoDestinationCatEnabled);

 private:

  Q_DISABLE_COPY(mdtSqlFieldMappingDialog)

  mdtError pvLastError;
  mdtSqlFieldMappingData pvMapping;
};

#endif // #ifndef MDT_SQL_FIELD_MAPPING_DIALOG_H
