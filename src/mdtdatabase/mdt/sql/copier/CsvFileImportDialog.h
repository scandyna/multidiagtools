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
#include <vector>

class QItemSelection;
class QModelIndex;

namespace mdt{ namespace sql{ namespace copier{

  class CsvFileImportMappingModel;
  class CsvFileImportThread;

  /*! \brief CSV file import dialog
   */
  class CsvFileImportDialog : public QDialog, Ui::mdtSqlCopierCsvFileImportDialog
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    CsvFileImportDialog (QWidget *parent = nullptr);

    // Copy disabled
    CsvFileImportDialog(const CsvFileImportDialog &) = delete;
    CsvFileImportDialog &operator=(const CsvFileImportDialog &) = delete;
    // Move disabled
    CsvFileImportDialog(CsvFileImportDialog &&) = delete;
    CsvFileImportDialog & operator=(CsvFileImportDialog &&) = delete;

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

    /*! \brief Edit selected copy item
     */
    void editCopyItem();

    /*! \brief Remove selected copy item
     */
    void removeCopyItem();

    /*! \brief Called when selection changes in mapping
     */
    void onMappingSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

    /*! \brief Import files
     */
    void importFiles();

   private:

    /*! \brief State
     */
    enum State
    {
      DatabaseNotSet,   /*!< Destination database contains not the needed informations for the copy.
                            It's also not possible to process. */
      DatabaseSet,      /*!< Destination database have needed informations and it's possible to process.  */
      ProcessingCopy    /*!< The only possible action is abort. */
    };

    /*! \brief Update selected rows list
     */
    void updateSelectedRowsList(const QItemSelection & s);

    /*! \brief Update add copy item button state
     */
    void updateAddCopyItemState();

    /*! \brief Update remove copy item button state
     */
    void updateEditCopyItemState();

    /*! \brief Update remove copy item button state
     */
    void updateRemoveCopyItemState();

    /*! \brief Set database state set or not set
     *
     * Will check if destination database contains needed informations
     *  and set state to DatabaseSet if ok,
     *  DatabaseNotSet if not.
     */
    void setStateDatabaseSetOrNotSet();

    /*! \brief Set database not set state
     */
    void setStateDatabaseNotSet();

    /*! \brief Set database set state
     */
    void setStateDatabaseSet();

    /*! \brief Set ProcessingCopy state
     */
    void setStateProcessingCopy();

    /*! \brief Check if db contains all needed informations
     */
    bool hasDatabaseNeededInformations(const QSqlDatabase & db) const;

    /*! \brief Resize table view
     */
    void resizeTableViewToContents();

    /*! \brief Display error
     */
    void displayError(const mdtError & error);

    State pvState;
    CsvFileImportMappingModel *pvMappingModel;
    CsvFileImportThread *pvThread;
    std::vector<int> pvSelectedRows;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_CSV_FILE_IMPORT_DIALOG_H
