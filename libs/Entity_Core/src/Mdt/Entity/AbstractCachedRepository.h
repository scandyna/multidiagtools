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
#ifndef MDT_ENTITY_ABSTRACT_CACHED_REPOSITORY_H
#define MDT_ENTITY_ABSTRACT_CACHED_REPOSITORY_H

#include "AbstractRepository.h"
#include "Mdt/Container/RowList.h"
#include "Mdt/Container/TableCache.h"
#include "Mdt/IndexRange/RowRange.h"
#include <QtGlobal>
#include <QVariant>

// #include <QDebug>

namespace Mdt{ namespace Entity{

  /*! \brief Base class to create a cached repository
   *
   * \code
   * class AbstractPersonRepository : public Mdt::Entity::AbstractCachedRepository<PersonData>
   * {
   *  public:
   *
   *   int columnCount() const override;
   *   QVariant data(int row, int column) const override;
   *   void setData(int row, int column, const QVariant & data) override;
   *   bool fetchRecords(int count) override;
   * };
   * \endcode
   *
   * To create a cached repository for a single entity based on Mdt::Entity::DataTemplate ,
   *  consider using Mdt::Entity::AbstractCachedEntityRepository .
   */
  template<typename Record>
  class AbstractCachedRepository : public AbstractRepository
  {
   public:

    /*! \brief STL style value type
     *
     * \todo Remove once possible
     */
    using value_type = Record;

    using record_type = Record;

    /*! \brief Construct a repository
     */
    AbstractCachedRepository(QObject *parent = nullptr)
     : AbstractRepository(parent)
    {
    }

    AbstractCachedRepository(const AbstractCachedRepository &) = delete;
    AbstractCachedRepository & operator=(const AbstractCachedRepository &) = delete;

    AbstractCachedRepository(AbstractCachedRepository &&) = delete;
    AbstractCachedRepository & operator=(AbstractCachedRepository &&) = delete;

    /*! \brief Set maximum rows in cache
     *
     * \sa cachedRowCountLimit()
     */
    void setCachedRowCountLimit(int limit)
    {
      mCachedRowCountLimit = limit;
    }

    /*! \brief Get maximum rows in cache
     */
    int cachedRowCountLimit() const
    {
      return mCachedRowCountLimit;
    }

    /*! \brief Get cout of rows
     *
     * \note Returns the count of rows that are actually in cache
     */
    int rowCount() const
    {
      return mCache.rowCount();
    }

    /*! \brief Get count of columns
     */
    virtual int columnCount() const = 0;

    /*! \brief Get data at \a row and \a column
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    virtual QVariant data(int row, int column) const = 0;

    /*! \brief Set \a data at \a row and \a column
     *
     * This will also mark the data as edited in the cache.
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    void setData(int row, int column, const QVariant & data)
    {
      setDataToCache(row, column, data);
      mCache.setRecordAtUpdated(row);
      emitDataAtRowChanged(row);
      emitOperationAtRowChanged(row);
    }

    /*! \brief Get record a \a row
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    const Record & constRecordAt(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return mCache.constRecordAt(row);
    }

    /*! \brief Get the operation at \a row in the cache
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    Mdt::Container::TableCacheOperation operationAtRow(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return mCache.operationAtRow(row);
    }

    /*! \brief Fetch all data from the underlaying storage, until defined row count limit
     */
    bool fetchAll()
    {
      beginResetCache();
      mCache.clear();
      const bool ok = fetchRecords( cachedRowCountLimit() );
      endResetCache();

      return ok;
    }

    /*! \brief Insert \a count copies of \a record
     *
     * Records will be inserted to the cache.
     *
     * \pre \a pos must be >= 0
     * \pre \a count must be >= 1
     */
    void insertRecords(int pos, int count, const Record & record)
    {
      Q_ASSERT(pos >= 0);
      Q_ASSERT(count >= 1);

      Mdt::IndexRange::RowRange rowRange;
      rowRange.setFirstRow(pos);
      rowRange.setRowCount(count);

      beginInsertRows(rowRange.firstRow(), rowRange.lastRow());
      mCache.insertRecords(pos, count, record);
      endInsertRows();
    }

    /*! \brief Remove \a count records starting from \a pos
     *
     * Records are marked as deleted in the cache.
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

      mCache.removeRecords(pos, count);
      Mdt::IndexRange::RowRange rowRange;
      rowRange.setFirstRow(pos);
      rowRange.setRowCount(count);
      emitOperationAtRowsChanged(rowRange.firstRow(), rowRange.lastRow());
    }

    /*! \brief Submit changes
     *
     * Will submit changes done in the cache to the storage.
     */
    bool submitChanges()
    {
      if(!updateModifiedRecordsInStorage()){
        return false;
      }
      if(!insertNewRecordsToStorage()){
        return false;
      }
      removeRecordsToDeleteFromCacheOnly();
      if(!removeRecordsToDeleteFromStorage()){
        return false;
      }
      mCache.commitChanges();
      const auto committedRows = mCache.committedRows();
      if(!committedRows.isNull()){
        emitOperationAtRowsChanged(committedRows.firstRow(), committedRows.lastRow());
      }
      return true;
    }

   protected:

    /*! \brief Fetch \a count records from the underlaying storage
     *
     * To add the fetched records to the cache, use appendRecordToCache() .
     *
     * If the storage has less records available than \a count
     *  (or no records at all), this method should return true.
     *  false should be returned on errors, like DB connection failure,
     *  file read errors, etc..
     *
     * Example of a possible, simplified, implementation using QSqlQuery:
     * \code
     * bool fetchRecords(int count) override
     * {
     *   if(!execSelectAll()){
     *     return false;
     *   }
     *   for(int i = 0; i < count; ++i){
     *     if(!mQuery.next()){
     *       return true;
     *     }
     *     addCurrentRecordToCache();
     *   }
     *   return true;
     * }
     * \endcode
     */
    virtual bool fetchRecords(int count) = 0;

    /*! \brief Append a record that come from storage to the cache
     *
     * \pre rowCount() must be < cachedRowCountLimit()
     */
    void appendRecordToCache(const Record & record)
    {
      Q_ASSERT(rowCount() < cachedRowCountLimit());
      mCache.appendRecordFromStorage(record);
    }

    /*! \brief Set \a data at \a row and \a column into the cache
     *
     * Implement this method to edit data in the cache.
     *
     * Example:
     * \code
     * void setDataToCache(int row, int column, const QVariant & data)
     * {
     *   switch(column){
     *     case 0:
     *       recordAt(row).setId(data.toULongLong());
     *       break;
     *     case 1:
     *       recordAt(row).setFirstName(data.toString());
     *       break;
     *   }
     * }
     * \endcode
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     * \note this method should not set any operation marking in the cache
     * \note this method should not emit any signal
     */
    virtual void setDataToCache(int row, int column, const QVariant & data) = 0;

    /*! \brief Set a auto ID to the cache
     *
     * If the repository uses auto ID
     *  (for example SQL auto increment primary key),
     *  this method should be implemented to set the auto ID.
     *
     * Example:
     * \code
     * void setAutoIdToCache(int row, const QVariant & id) override
     * {
     *   recordAt(row).setId(id.toULongLong());
     * }
     * \endcode
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \note this method should not set any operation marking in the cache
     * \note this method should not emit any signal
     */
    virtual void setAutoIdToCache(int row, const QVariant & id)
    {
      Q_UNUSED(row);
      Q_UNUSED(id);
    }

    /*! \brief Get record a \a row
     *
     * No marking is done by calling this method.
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    Record & recordAt(int row)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());

      return mCache.recordAt(row);
    }

    /*! \brief Insert \a record to the underlaying storage
     *
     * If the repository uses auto ID
     *  (for example SQL auto increment primary key),
     *  \a autoId should be set by the implementation.
     */
    virtual bool insertRecordToStorage(const Record & record, QVariant & autoId) = 0;

    /*! \brief Remove the record at \a row from the storage
     */
    virtual bool removeRecordFromStorage(int row) = 0;

    /*! \brief Update the record at \a row in the storage
     */
    virtual bool updateRecordInStorage(int row) = 0;

   private:

    bool insertNewRecordsToStorage()
    {
      const auto rowList = mCache.getRowsToInsertIntoStorage();
      for(const auto row : rowList){
        QVariant autoId;
        if(!insertRecordToStorage( constRecordAt(row), autoId )){
          return false;
        }
        if(!autoId.isNull()){
          setAutoIdToCache(row, autoId);
        }
      }
      return true;
    }

    void removeRecordsToDeleteFromCacheOnly()
    {
      const auto rowList = mCache.getRowsToDeleteInCacheOnly();
      for(const auto row : rowList){
        beginRemoveRows(row, row);
        mCache.removeRecordInCache(row);
        endRemoveRows();
      }
    }

    bool removeRecordsToDeleteFromStorage()
    {
      const auto rowList = mCache.getRowsToDeleteInStorage();
      for(const auto row : rowList){
        if(!removeRecordFromStorage(row)){
          return false;
        }
        beginRemoveRows(row, row);
        mCache.removeRecordInCache(row);
        endRemoveRows();
      }
      return true;
    }

    bool updateModifiedRecordsInStorage()
    {
      const auto rowList = mCache.getRowsToUpdateInStorage();
      for(const auto row : rowList){
        if(!updateRecordInStorage(row)){
          return false;
        }
      }
      return true;
    }

    Mdt::Container::TableCache<Record> mCache;
    int mCachedRowCountLimit = 5000;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_CACHED_REPOSITORY_H
