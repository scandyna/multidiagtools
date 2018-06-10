/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_CONTAINER_TABLE_CACHE_H
#define MDT_CONTAINER_TABLE_CACHE_H

#include "TableCacheOperation.h"
#include "TableCacheOperationMap.h"
#include "RowList.h"
#include "StlContainer.h"
#include "Mdt/IndexRange/RowRange.h"
#include <QtGlobal>
#include <vector>
#include <initializer_list>

namespace Mdt{ namespace Container{

  /*! \brief Holds a list of record in memory and tracks operations
   *
   * A table cache can be used to fetch some data from a storage,
   *  and query about them many times in a fast way.
   *
   * The table cache can also track modifications made in the cache,
   *  providing a way to submit changes to the storage, or revert them.
   *
   * To be able to track modifications in the cache,
   *  it is required to separate methods that acts from the storage
   *  from them that acts from the user of the table cache.
   *
   * \todo Remember:
   *  A) Don't forget what transaction can do
   *  B) Do not requery after submit !
   *  C) -> Changes from storage should be handled as events
   * \code
   * bool insertNewRecordsToStorage()
   * {
   *   const auto rowList = mCache.getRowsToInsertIntoStorage();
   *   for(const auto row : rowList){
   *     if(!insertRecordToStorage( constRecordAt(row) )){
   *       return false;
   *     }
   *   }
   *   mCache.commitChanges();
   *   signal(mChache.firstCommitedRow(), mCache.lastModifiedRow());
   * }
   * \endcode
   *
   * When removing rows, 2 situations can occur:
   *  - The data referred by the row has allready been stored
   *  - The row has been inserted in the cache, but the data does not exist in the storage
   *
   * To remove rows, a good solution is to remove each row from the greatest to the lowest one.
   *  This avoids having to recalculate the row to match the new size of the cache.
   *
   * Example:
   * \code
   * auto rowList = mCache.getRowsToDeleteInCacheOnly();
   * for(const auto row : rowList){
   *   notifyRemoveRowBegins(row);
   *   mCache.removeRecordInCache(row);
   *   notifyRemoveRowDone();
   * }
   * // Remove data in the storage.
   * beginTransaction();
   * rowList = mCache.getRowsToDeleteInStorage();
   * for(const auto row : rowList){
   *   removeRecordFromStorage(row);
   * }
   * if(commitTransaction()){
   *   for(const auto row : rowList){
   *     notifyRemoveRowBegins(row);
   *     mCache.removeRecordInCache(row);
   *     notifyRemoveRowDone();
   *   }
   * }
   * \endcode
   *
   * \note TableCache works only at the record level, i.e. does not provide
   *    access to the data at row and column.
   *    This is to avoid restricting a column access API.
   *    STL could be a solution, but for some usage,
   *    some STL requirement can simply not be met with reasonably simple API
   *    (for example, dereferencing .............................)
   */
  template<typename Record>
  class TableCache
  {
    using Table = std::vector<Record>;

   public:

    /*! \brief Construct a empty cache
     */
    TableCache() = default;

    /*! \brief Construct a cache from \a list
     */
    TableCache(std::initializer_list<Record> list)
     : mCache(list)
    {
    }

    TableCache(const TableCache &) = delete;
    TableCache & operator=(const TableCache &) = delete;

    TableCache(TableCache &&) = delete;
    TableCache & operator=(TableCache &&) = delete;

    /*! \brief Check if this cache is empty
     */
    bool isEmpty() const
    {
      return mCache.empty();
    }

    /*! \brief Get the count of rows in this cache
     */
    int rowCount() const
    {
      return mCache.size();
    }

    /*! \brief Set record at \a row
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     * \note Calling this method will mark the entiere record at \a row as edited in the cache
     */
    void setRecordAt(int row, const Record & record)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      mCache[row] = record;
      mOperationMap.setOperationAtRow(row, TableCacheOperation::Update);
    }

    /*! \brief Get record a \a row
     *
     * No marking is done by calling this method,
     *  this must be done using setRecordAtUpdated()
     *
     * Example:
     * \code
     * void setRecord(int row, const Record & record)
     * {
     *   mCache.recordAt(row) = record;
     *   mCache.setRecordAtUpdated(row);
     * }
     * \endcode
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    Record & recordAt(int row)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());

      return mCache[row];
    }

    /*! \brief Mark the record at \a row as updated
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    void setRecordAtUpdated(int row)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());

      mOperationMap.setOperationAtRow(row, TableCacheOperation::Update);
    }

    /*! \brief Get record a \a row
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    const Record & constRecordAt(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return mCache[row];
    }

    /*! \brief Get the operation at \a row in the cache
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    TableCacheOperation operationAtRow(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return mOperationMap.operationAtRow(row);
    }

    /*! \brief Get a list of rows that have to be inserted to the storage
     */
    RowList getRowsToInsertIntoStorage() const
    {
      return mOperationMap.getRowsToInsertIntoStorage();
    }

    /*! \brief Get a list of rows that have to be updated in the storage
     */
    RowList getRowsToUpdateInStorage() const
    {
      return mOperationMap.getRowsToUpdateInStorage();
    }

    /*! \brief Get a list of rows that have to be deleted in the storage
     *
     * \note The returned list is sorted in descending way,
     *    which helps avoiding problems when removing a list of rows.
     *
     * \todo Must be removed because need a single pass remove rows
     */
    RowList getRowsToDeleteInStorage() const
    {
      return mOperationMap.getRowsToDeleteInStorage();
    }

    /*! \brief Get a list of rows that have to be deleted from the cache only
     *
     * \note The returned list is sorted in descending way,
     *    which helps avoiding problems when removing a list of rows.
     *
     * \todo Must be removed because need a single pass remove rows
     */
    RowList getRowsToDeleteInCacheOnly() const
    {
      return mOperationMap.getRowsToDeleteInCacheOnly();
    }

    /*! \brief Clear this cache
     */
    void clear()
    {
      mCache.clear();
      mOperationMap.clear();
    }

    /*! \brief Commit changes
     *
     * Will clear all operation marking in this cache
     *  and update committed states, like committedRows .
     *  The rows that have been marked as to be removed,
     *  will also be removed from this cache.
     *
     * \code
     * bool submitAll()
     * {
     *   if(!submitDataToStorage()){
     *     return false;
     *   }
     *   mCache.commitChanges();
     *   const auto committedRows = mCache.committedRows();
     *   if(!committedRows.isNull()){
     *     emit operationAtRowsChanged(committedRows.firstRow(), committedRows.lastRow());
     *   }
     * }
     * \endcode
     */
    void commitChanges()
    {
      removeDeletedRecords();
      mOperationMap.commitChanges();
    }

    /*! \brief Get the range of committed rows
     *
     * Returns a range of rows that have been committed.
     *  If commitChanges() was not called, or no operation marking exists in this cache,
     *  a null range is returned.
     *
     * \todo Should be renamed updatedRows() .
     *    It helps to notify which range of rows are to update, for example in a UI (to reflect the new operation)
     */
    Mdt::IndexRange::RowRange committedRows() const
    {
      return mOperationMap.committedRows();
    }

    /*! \brief Insert \a count copies of \a record
     *
     * Records will be inserted to the cache and marked as insert operation.
     *
     * \pre \a pos must be >= 0
     * \pre \a count must be >= 1
     */
    void insertRecords(int pos, int count, const Record & record)
    {
      Q_ASSERT(pos >= 0);
      Q_ASSERT(count >= 1);
      insertToContainer(mCache, pos, count, record);
      mOperationMap.insertRecords(pos, count);
    }

    /*! \brief Add \a record to the end of this cache
     *
     * The record will be added to the end of the cache and marked as insert operation.
     */
    void appendRecord(const Record & record)
    {
      insertRecords(rowCount(), 1, record);
    }

    /*! \brief Add \a record to the end of this cache
     *
     * The record will be added to the end of the cache.
     *  No operation marking is done.
     */
    void appendRecordFromStorage(const Record & record)
    {
      mCache.push_back(record);
    }

    /*! \brief Remove \a count records starting from \a pos
     *
     * Records are marked as delete operation, but not removed from the cache.
     *  Removing records from the cache is done by commitChanges().
     *
     * \pre \a pos must be >= 0
     * \pre \a count must be >= 1
     * \pre \a pos + \a count must be in valid range ( 1 <= \a pos + \a count <= rowCount() )
     */
    void removeRecords(int pos, int count)
    {
      Q_ASSERT(pos >= 0);
      Q_ASSERT(count >= 0);
      Q_ASSERT( (pos + count) <= rowCount() );

      mOperationMap.removeRecords(pos, count);
    }

    /*! \brief Remove a record in the cache
     *
     * The record at \a row will be removed from the cache.
     *  The corresponding marking is also removed as well.
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    void removeRecordInCache(int row)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());

      mOperationMap.removeOperationAtRow(row);
      removeFromContainer(mCache, row, 1);
    }

   private:

    void removeDeletedRecords()
    {
      const auto rows = mOperationMap.getRowsToDeleteInCache();
      for(const auto row : rows){
        removeRecordInCache(row);
      }
    }

    Table mCache;
    TableCacheOperationMap mOperationMap;
  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_TABLE_CACHE_H
