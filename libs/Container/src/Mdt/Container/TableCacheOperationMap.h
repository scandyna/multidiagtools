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

    /*! \brief Get the operation at \a row in the cache
     *
     * \pre \a row must be >= 0
     */
    TableCacheOperation operationAtRow(int row) const;

    /*! \brief Get a list of rows that have to be inserted to the storage
     */
    RowList getRowsToInsertIntoStorage() const;

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
    void commitChanges();

    /*! \brief Get the first row that have been committed to the storage
     *
     * \pre This map must not be empty
     */
    int getFirstCommittedRow() const;

    /*! \brief Get the last row that have been committed to the storage
     *
     * \pre This map must not be empty
     */
    int getLastCommittedRow() const;

   private:

    TableCacheOperationIndex findIndex(int row, int column) const;
    TableCacheOperationIndex findRow(int row) const;

    std::vector<TableCacheOperationIndex> mMap;
  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_TABLE_CACHE_OPERATION_MAP_H
