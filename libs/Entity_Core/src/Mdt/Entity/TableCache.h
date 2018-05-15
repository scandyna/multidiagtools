/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ENTITY_TABLE_CACHE_H
#define MDT_ENTITY_TABLE_CACHE_H

// #include "TableCacheModifiedMap.h"
#include "Mdt/Container/TableCacheOperationMap.h"
#include "Mdt/Container/StlContainer.h"
#include "Mdt/Container/RowList.h"
#include <QtGlobal>
#include <vector>
#include <initializer_list>

namespace Mdt{ namespace Entity{

  /*! \brief Operation marking
   */
  enum class TableCacheMarking
  {
    MarkOperation,    /*!< Mark the operation */
    NotMarkOperation  /*!< Do not mark the operation */
  };

  /*! \brief A cache used in AbstractCachedRepository
   */
  template<typename Record>
  class TableCache
  {
    using Table = std::vector<Record>;

   public:

//     /*! \brief STL style value type
//      */
//     using value_type = typename Table::value_type;
// 
//     /*! \brief STL style reference
//      */
//     using reference = typename Table::reference;
// 
//     /*! \brief STL style pointer
//      */
//     using pointer = typename Table::pointer;
// 
//     /*! \brief STL style size type
//      */
//     using size_type = int;

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

    /*! \brief Get record a \a row
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     * \note Calling this method will mark the entiere record at \a row as edited in the cache
     */
    Record & recordAt(int row)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return mCache[row];
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
    Mdt::Container::TableCacheOperation operationAtRow(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return mOperationMap.operationAtRow(row);
    }

//     /*! \brief Set \a value at \a row and \a column
//      *
//      * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
//      * \pre \a column must be in valid range (  )
//      */

    /*! \brief Clear this cache
     */
    void clear()
    {
      mCache.clear();
    }

    /*! \brief Insert \a count copies of \a record
     *
     * Records will be inserted to the cache.
     *
     * \pre \a pos must be >= 0
     * \pre \a count must be >= 1
     */
    void insertRecords(int pos, int count, const Record & record, TableCacheMarking operationMarking)
    {
      Q_ASSERT(pos >= 0);
      Q_ASSERT(count >= 1);
      Mdt::Container::insertToContainer(mCache, pos, count, record);
      if(operationMarking == TableCacheMarking::MarkOperation){
        mOperationMap.insertRecords(pos, count);
      }
    }

    /*! \brief Add \a record to the end of this cache
     */
    void appendRecord(const Record & record, TableCacheMarking operationMarking)
    {
      Mdt::Container::appendToContainer(mCache, record);
      if(operationMarking == TableCacheMarking::MarkOperation){
        mOperationMap.insertRecords(rowCount()-1, 1);
      }
    }

    /*! \brief Get a list of rows that have to be inserted to the storage
     */
    Mdt::Container::RowList getRowsToInsertIntoStorage() const
    {
      return mOperationMap.getRowsToInsertIntoStorage();
    }

    /*! \brief Commit changes
     *
     * The operation for each indexes in this cache
     *  will be set to TableCacheOperation::None .
     *  This way, it is now possible to query which indexes
     *  have been sent to the storage.
     *
     * Do not forget to call removedAllIndexes()
     *  once all needed information have been retrieved.
     */
    void commitChanges()
    {
      mOperationMap.commitChanges();
    }

    /*! \brief Get the first row that have been committed to the storage
     *
     * \pre This cache must have at least 1 operation
     * \sa hasOperation()
     */
    int getFirstCommittedRow() const
    {
      Q_ASSERT(hasOperation());
//       return mOperationMap.getFirstCommittedRow();
    }

    /*! \brief Get the last row that have been committed to the storage
     *
     * \pre This cache must have at least 1 operation
     * \sa hasOperation()
     */
    int getLastCommittedRow() const
    {
      Q_ASSERT(hasOperation());
//       return mOperationMap.getLastCommittedRow();
    }

    /*! \brief Check if this cache has at least one operation
     */
    bool hasOperation() const
    {
      return !mOperationMap.isEmpty();
    }

    /*! \brief Remove all operations from this cache
     */
    void removeAllOperations()
    {
      mOperationMap.clear();
    }

//     /*! \brief Remove the indexes for which the row is in \a rowList
//      */
//     void removeIndexesForRows(const Mdt::Container::RowList & rowList)
//     {
//       mModifiedMap.removeIndexesForRows(rowList);
//     }

    /*! \brief Remove \a count records starting from \a pos
     *
     * Records are removed from the cache.
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
      Mdt::Container::removeFromContainer(mCache, pos, count);
    }

   private:

    std::vector<Record> mCache;
//     TableCacheModifiedMap mModifiedMap;
    Mdt::Container::TableCacheOperationMap mOperationMap;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_TABLE_CACHE_H
