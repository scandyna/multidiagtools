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
#ifndef MDT_SQL_COPIER_CSV_FILE_IMPORT_THREAD_H
#define MDT_SQL_COPIER_CSV_FILE_IMPORT_THREAD_H

#include "CopyThread.h"

namespace mdt{ namespace sql{ namespace copier{

  class CsvFileImportMappingModel;
  class CsvFileImportTableMapping;

  /*! \brief Worker thread for CSV file import
   *
   * This class is used by CsvFileImportDialog,
   *  witch should be used by applications.
   */
  class CsvFileImportThread : public CopyThread
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    CsvFileImportThread(QObject* parent = nullptr);

    // Copy disabled
    CsvFileImportThread(const CsvFileImportThread &) = delete;
    CsvFileImportThread & operator=(const CsvFileImportThread &) = delete;
    // move disabled
    CsvFileImportThread(const CsvFileImportThread &&) = delete;
    CsvFileImportThread & operator=(CsvFileImportThread &&) = delete;

    /*! \brief Get table mapping count
     */
    int tableMappingCount() const override;

    /*! \brief Copy files of given CSV file import mapping
     *
     * Will copy data of source files to destination tables regarding given mapping.
     *
     * Database object contained in mapping will be used to get needed informations
     *  (such as driver name, database name, etc...).
     *  Internally, new connections are created (this is required because of multi-threading).
     *
     * \warning It is recommanded that no connection refers to the
     *           databases on witch the schema will be copied.
     *           This check is not done here, and paying no attention to
     *           this case can conduct to undefined behaviour.
     *
     * \pre Thread must be stopped before calling this function
     */
    void startCopy(const CsvFileImportMappingModel * const model);

   private:

    /*! \brief Import CSV file regarding table mapping
     */
    bool importFile(const CsvFileImportTableMapping * const tm, int index, const QSqlDatabase & destinationDatabase);

    /*! \brief Thread function implementation
     */
    void run();

    const CsvFileImportMappingModel *pvModel;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_CSV_FILE_IMPORT_THREAD_H
