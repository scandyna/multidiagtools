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
#ifndef MDT_SQL_COPIER_CSV_FILE_IMPORT_DIALOG_H
#define MDT_SQL_COPIER_CSV_FILE_IMPORT_DIALOG_H

#include "ui_CsvFileImportDialog.h"
#include "mdtError.h"
#include <QDialog>
#include <QSqlDatabase>

namespace mdt{ namespace sql{ namespace copier{

  class CsvFileImportMappingModel;

  /*! \brief CSV file import dialog
   */
  class CsvFileImportDialog : public QDialog, Ui::mdtSqlCopierCsvFileImportDialog
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    CsvFileImportDialog (QWidget *parent = nullptr);

    /*! \brief Set destination database
     */
    void setDestinationDatabase(const QSqlDatabase & db);

  private slots:

    /*! \brief Select destination database
     */
    void selectDestinationDatabase();

    /*! \brief Add a copy item
     */
    void addCopyItem();

   private:

    /*! \brief Resize table view
     */
    void resizeTableViewToContents();

    /*! \brief Display error
     */
    void displayError(const mdtError & error);

    CsvFileImportMappingModel *pvMappingModel;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_CSV_FILE_IMPORT_DIALOG_H
