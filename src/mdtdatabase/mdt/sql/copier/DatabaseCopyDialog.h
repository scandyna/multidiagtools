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
#ifndef MDT_SQL_DATABASE_COPIER_DIALOG_H
#define MDT_SQL_DATABASE_COPIER_DIALOG_H

#include "ui_DatabaseCopyDialog.h"
#include "mdtSqlDriverType.h"
#include "mdtError.h"
#include <QDialog>
#include <QModelIndex>

namespace mdt{ namespace sql{ namespace copier{

  class DatabaseMappingModel;
  class DatabaseCopyThread;

  /*! \brief Dialog that provides database copier tool
   */
  class DatabaseCopyDialog : public QDialog, Ui::mdtSqlCopierDatabaseCopyDialog
  {
  Q_OBJECT

  public:

    /*! \brief Constructor
     */
    DatabaseCopyDialog(QWidget *parent = nullptr);

    /*! \internal Copy diabled
     */
    DatabaseCopyDialog(const DatabaseCopyDialog & other) = delete;

    /*! \brief Destructor
     */
    ~DatabaseCopyDialog();

    /*! \brief Init a database for source
     *
     * \note: currently, only SQLite is supported.
     * \pre This function can be called only once
     */
    void initSourceDatabase(mdtSqlDriverType::Type driverType);

    /*! \brief Set source database
     *
     * \pre This function must not be called if initSourceDatabase() has been caled before
     */
    void setSourceDatabase(const QSqlDatabase & db);

    /*! \brief Set source database selectable or not by the user
     */
    void setSourceDatabaseSelectable(bool selectable);

    /*! \brief Set destination database
     */
    void setDestinationDatabase(const QSqlDatabase & db);

    /*! \brief Set destination database selectable or not by the user
     */
    void setDestinationDatabaseSelectable(bool selectable);

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

    /*! \brief Edit a table mapping
     */
    void editTableMapping(const QModelIndex & index);

    /*! \brief Resize table view
     */
    void resizeTableViewToContents();

    /*! \brief Copy data
     */
    void copyData();

    /*! \brief Reciver for a global error generated from worker thread
     */
    void onThreadGlobalErrorOccured(mdtError error);

    /*! \brief
     */
    void onThreadFinished();

  private:

    /*! \brief Set source database implementation
     */
    void setSourceDatabasePv(const QSqlDatabase & db);

    /*! \brief State
     */
    enum State
    {
      DatabasesNotSet,  /*!< Source and/or destination databases contains not the needed informations for the copy.
                            It's also not possible to process. */
      DatabasesSet,     /*!< Source and destination databases have needed informations and it's possible to process.  */
      ProcessingCopy    /*!< The only possible action is abort. */
    };

    /*! \brief Set database state set or not set
     *
     * Will check if if source and destination databases contains needed informations
     *  and set state to DatabasesSet if ok,
     *  DatabasesNotSet if not.
     */
    void setStateDatabasesSetOrNotSet();

    /*! \brief Set database not set state
     */
    void setStateDatabasesNotSet();

    /*! \brief Set database set state
     */
    void setStateDatabasesSet();

    /*! \brief Check if db contains all needed informations
     */
    bool hasDatabaseNeededInformations(const QSqlDatabase & db) const;

    /*! \brief Set ProcessingCopy state
     */
    void setStateProcessingCopy();

    /*! \brief Set closable
     *
     * If closable is false, user cannot close this dialog.
     */
    void setClosable(bool closable);

    /*! \brief Close event
     */
    void closeEvent(QCloseEvent *event);

    /*! \brief Reject (event)
     */
    void reject();

    /*! \brief Display error
     */
    void displayError(const mdtError & error);

    bool pvClosable;
    State pvState;
    DatabaseMappingModel *pvMappingModel;
    DatabaseCopyThread *pvThread;
    QString pvOwningSourceConnectionName;
  //   QString pvOwningDestinationConnectionName;
    bool pvSourceDatabaseSelectable;
    bool pvDestinationDatabaseSelectable;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_DATABASE_COPIER_DIALOG_H
