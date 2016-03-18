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
#ifndef MDT_SQL_COPIER_COPY_THREAD
#define MDT_SQL_COPIER_COPY_THREAD

#include "mdtError.h"
#include "TableMapping.h"
#include "mdtProgressValue.h"
#include <QThread>
#include <QString>
#include <QSqlDatabase>
#include <atomic>
#include <vector>
#include <cstdint>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief
   *
   * Note that table mapping data (Mapping subclasses) and models (TableMappingModel subclasses)
   *  are not thread safe. Enshure that no modifications will be made once thread runs.
   */
  class CopyThread : public QThread
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    CopyThread(QObject* parent = nullptr);

    // Copy disabled
    CopyThread(const CopyThread &) = delete;
    CopyThread & operator=(const CopyThread &) = delete;

    // move disabled
    CopyThread(const CopyThread &&) = delete;
    CopyThread & operator=(CopyThread &&) = delete;

    /*! \brief Get table mapping count
     */
    virtual int tableMappingCount() const = 0;

   public slots:

    /*! \brief Abort database copy
     */
    void abort();

   signals:

    /*! \brief Emitted when table copy progress was updated
     */
    void tableCopyProgressChanged(int mappingModelRow, int progress);

    /*! \brief Emitted when table copy status was updated
     */
    void tableCopyStatusChanged(int mappingModelRow, int status);

    /*! \brief Emitted when a error occured during a table copy
     */
    void tableCopyErrorOccured(int mappingModelRow, mdtError error);

    /*! \brief Emitted when global copy progress range changed
     */
    void globalProgressRangeChanged(int min, int max);

    /*! \brief Emitted when global copy progress value changed
     */
    void globalProgressValueChanged(int progress);

    /*! \brief Emitted when a global error occured
     */
    void globalErrorOccured(mdtError error);

   protected:

    /*! \brief Reset table size list
     *
     * This function must be called before starting copy
     */
//     void resetTableSizeList();

    /*! \brief Reset table copy progress
     *
     * This function must be called before starting copy
     */
    void resetCopyProgress();

    /*! \brief Set table size at given index
     */
    void setTableSize(int index, int64_t size);

    /*! \brief Get table size at given index
     */
    int64_t tableSize(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < (int)pvTableSizeList.size());
      return pvTableSizeList[index];
    }

    /*! \brief Calculate total copy size
     */
    int64_t calculateTotalCopySize();

    /*! \brief Increment copy progress by n records
     */
    void incrementTableCopyProgress(int index, int64_t n = 1);

    /*! \brief Notify that copy is done successfully for table at given index
     */
    void notifyTableCopyDone(int index);

    /*! \brief Create database connection based on given dbInfo
     */
    QSqlDatabase createConnection(const QSqlDatabase & dbInfo);

    std::atomic<bool> pvAbort;

   private:

    std::vector<int64_t> pvTableSizeList;
    std::vector<mdtProgressValue<int64_t> > pvTableProgressList;
    mdtProgressValue<int64_t, mdtProgressValueTimer> pvGlobalProgress;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_COPY_THREAD
