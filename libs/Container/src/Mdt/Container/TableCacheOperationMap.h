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
#ifndef MDT_CONTAINER_TABLE_CACHE_OPERATION_MAP_H
#define MDT_CONTAINER_TABLE_CACHE_OPERATION_MAP_H

#include "TableCacheOperation.h"
#include "TableCacheOperationIndex.h"
#include "RowList.h"
#include "Mdt/IndexRange/RowRange.h"
#include "MdtContainerExport.h"
#include <QtGlobal>
#include <vector>

namespace Mdt{ namespace Container{

  /*! \brief Map of operations indexes in a table cache
   */
  class MDT_CONTAINER_EXPORT TableCacheOperationMap
  {
   public:

    /*! \brief STL style const iterator
     */
    using const_iterator = std::vector<TableCacheOperationIndex>::const_iterator;

    /*! \brief STL style iterator
     */
    using iterator = std::vector<TableCacheOperationIndex>::iterator;

    /*! \brief Get a iterator to the first element in this map
     */
    const_iterator cbegin() const
    {
      return mMap.cbegin();
    }

    /*! \brief Get a iterator past the the last element in this map
     */
    const_iterator cend() const
    {
      return mMap.cend();
    }

    /*! \brief Get a iterator to the first element in this map
     */
    const_iterator begin() const
    {
      return mMap.cbegin();
    }

    /*! \brief Get a iterator past the the last element in this map
     */
    const_iterator end() const
    {
      return mMap.cend();
    }

    /*! \brief Get the count of indexes in this map
     */
    int indexCount() const
    {
      return mMap.size();
    }

    /*! \brief Get the index at \a i
     *
     * \pre \a i must be >= 0
     * \pre \a i must be < indexCount()
     */
    const TableCacheOperationIndex & indexAt(int i)
    {
      Q_ASSERT(i >= 0);
      Q_ASSERT(i < indexCount());

      return mMap[i];
    }

    /*! \brief Check if this map is empty
     */
    bool isEmpty() const
    {
      return mMap.empty();
    }

    /*! \brief Clear this map
     */
    void clear();

    /*! \brief Add indexes to mark \a count records, starting from \a pos , as inserted records
     *
     * \pre \a pos must be >= 0
     * \pre \a count must be >= 0
     */
    void insertRecords(int pos, int count);

    /*! \brief Add indexes to mark \a count records, starting from \a pos , as deleted records
     *
     * \pre \a pos must be >= 0
     * \pre \a count must be >= 1
     */
    void removeRecords(int pos, int count);

    /*! \brief Cancel the removal of \a count records starting from \a pos
     *
     * \pre \a pos must be >= 0
     * \pre \a count muste be >= 1
     */
    void cancelRemoveRecords(int pos, int count);

    /*! \brief Remove the operation at \a row from thi map
     *
     * \pre \a row must be >= 0
     */
    void removeOperationAtRow(int row);

    /*! \brief Shift the rows starting from \a row with a offset of \a count
     *
     * \pre \a row must be >= 0
     * \note \a count can also be < 0
     */
    void shiftRowsInMap(int row, int count);

    /*! \brief Set the operation at \a row in the cache
     *
     * If no operation exists for \a row , it will be added and set to \a operation .
     *
     * If a operation allready exists at \a row , it will be updated regarding opertaionFromExisting() .
     *
     * \pre \a row must be >= 0
     */
    void setOperationAtRow(int row, TableCacheOperation operation);

    /*! \brief Get the operation at \a row in the cache
     *
     * \pre \a row must be >= 0
     */
    TableCacheOperation operationAtRow(int row) const;

    /*! \brief Get a list of rows that have to be inserted to the storage
     */
    RowList getRowsToInsertIntoStorage() const;

    /*! \brief Get a list of rows that have to be updated in the storage
     */
    RowList getRowsToUpdateInStorage() const;

    /*! \brief Get a list of rows that have to be deleted in the storage
     *
     * \note The returned list is sorted in descending way,
     *    which helps avoiding problems when removing a list of rows.
     */
    RowList getRowsToDeleteInStorage() const;

    /*! \brief Get a list of rows that have to be deleted from the cache only
     *
     * \note The returned list is sorted in descending way,
     *    which helps avoiding problems when removing a list of rows.
     */
    RowList getRowsToDeleteInCacheOnly() const;

    /*! \brief Get a list of all rows that have to be deleted from the cache
     *
     * This is equivalent of the result of getRowsToDeleteInStorage() + getRowsToDeleteInCacheOnly()
     *
     * \note The returned list is sorted in descending way,
     *    which helps avoiding problems when removing a list of rows.
     */
    RowList getRowsToDeleteInCache() const;

    /*! \brief Commit changes
     *
     * Will clear all operations in this map
     *  and update committed states, like committedRows .
     */
    void commitChanges();

    /*! \brief Get the range of committed rows
     *
     * Returns a range of rows that have been committed.
     *  If commitChanges() was not called, or no operation exists in this map,
     *  a null range is returned.
     */
    Mdt::IndexRange::RowRange committedRows() const
    {
      return mCommittedRows;
    }

    /*! \brief Get the operation regarding \a existingOperation
     *
     * |       Existing operation        |            Operation            |          Result           |
     * |:-------------------------------:|:-------------------------------:|:-------------------------:|
     * |TableCacheOperation::None        |TableCacheOperation::None        |TableCacheOperation::None  |
     * |TableCacheOperation::None        |TableCacheOperation::Insert      |TableCacheOperation::Insert|
     * |TableCacheOperation::None        |TableCacheOperation::Update      |TableCacheOperation::Update|
     * |TableCacheOperation::None        |TableCacheOperation::Delete      |TableCacheOperation::Delete|
     * |TableCacheOperation::None        |TableCacheOperation::InsertDelete| ?? |
     * |TableCacheOperation::Insert      |TableCacheOperation::None        | ?? |
     * |TableCacheOperation::Insert      |TableCacheOperation::Insert      |TableCacheOperation::Insert|
     * |TableCacheOperation::Insert      |TableCacheOperation::Update      |TableCacheOperation::Insert|
     * |TableCacheOperation::Insert      |TableCacheOperation::Delete      |TableCacheOperation::InsertDelete|
     * |TableCacheOperation::Insert      |TableCacheOperation::InsertDelete| ?? |
     * |TableCacheOperation::Update      |TableCacheOperation::None        | ?? |
     * |TableCacheOperation::Update      |TableCacheOperation::Insert      | ?? |
     * |TableCacheOperation::Update      |TableCacheOperation::Update      |TableCacheOperation::Update|
     * |TableCacheOperation::Update      |TableCacheOperation::Delete      |TableCacheOperation::Delete|
     * |TableCacheOperation::Update      |TableCacheOperation::InsertDelete| ?? |
     * |TableCacheOperation::Delete      |TableCacheOperation::None        | ?? |
     * |TableCacheOperation::Delete      |TableCacheOperation::Insert      |TableCacheOperation::Delete|
     * |TableCacheOperation::Delete      |TableCacheOperation::Update      |TableCacheOperation::Update|
     * |TableCacheOperation::Delete      |TableCacheOperation::Delete      |TableCacheOperation::Delete|
     * |TableCacheOperation::Delete      |TableCacheOperation::InsertDelete|TableCacheOperation::Delete|
     * |TableCacheOperation::InsertDelete|TableCacheOperation::None        | ?? |
     * |TableCacheOperation::InsertDelete|TableCacheOperation::Insert      | ?? |
     * |TableCacheOperation::InsertDelete|TableCacheOperation::Update      | ?? |
     * |TableCacheOperation::InsertDelete|TableCacheOperation::Delete      | ?? |
     * |TableCacheOperation::InsertDelete|TableCacheOperation::InsertDelete| ?? |
     */
    static TableCacheOperation operationFromExisting(TableCacheOperation existingOperation, TableCacheOperation operation);

   private:

    TableCacheOperationIndex findIndex(int row, int column) const;

    iterator findRowIterator(int row);
    const_iterator findRowConstIterator(int row) const;
    TableCacheOperationIndex findRow(int row) const;

    static bool isCommittedRow(const TableCacheOperationIndex & index);
    void updateOrAddOperationsForRows(int row, int count, TableCacheOperation operation);
    TableCacheOperationIndex operationIndexAtIfExists(int index) const;
    void setCommittedRows();
    RowList getRowsForOperation(TableCacheOperation operation) const;
    RowList getRowsForOperation(TableCacheOperation operation1, TableCacheOperation operation2) const;

    std::vector<TableCacheOperationIndex> mMap;
    Mdt::IndexRange::RowRange mCommittedRows;
  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_TABLE_CACHE_OPERATION_MAP_H
