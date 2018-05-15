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
// #include "TableCache.h"
#include "Mdt/Container/RowList.h"
#include "Mdt/Container/TableCache.h"
#include <QtGlobal>
#include <QVariant>

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

//     /*! \brief Destructor
//      */
//     virtual ~AbstractCachedRepository() = default;

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
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    void setData(int row, int column, const QVariant & data)
    {
      setDataToCache(row, column, data);
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

    /*! \brief Get record a \a row
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     * \note Calling this method will mark the entiere record at \a row as edited in the cache
     * \todo Should be protected and should not change oprations
     */
    Record & recordAt(int row)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return mCache.recordAt(row);
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
      mCache.insertRecords(pos, count, record);
    }

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
      mCache.removeRecords(pos, count);
    }

    /*! \brief Submit changes
     *
     * Will submit changes done in the cache to the storage.
     */
    bool submitChanges()
    {
      if(!insertNewRecordsToStorage()){
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
//       mCache.appendRecord(record, TableCacheMarking::NotMarkOperation);
    }

    /*! \brief Insert \a record to the underlaying storage
     */
    virtual bool insertRecordToStorage(const Record & record) = 0;

    /*! \brief Set \a data at \a row and \a column into the cache
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    virtual void setDataToCache(int row, int column, const QVariant & data) = 0;

   private:

    bool insertNewRecordsToStorage()
    {
      const auto rowList = mCache.getRowsToInsertIntoStorage();
      for(const auto row : rowList){
        if(!insertRecordToStorage( constRecordAt(row) )){
          return false;
        }
      }
//       mCache.removeIndexesForRows(rowList);
      return true;
    }

    bool updateModifiedRecordsInStorage()
    {
    }

    Mdt::Container::TableCache<Record> mCache;
    int mCachedRowCountLimit = 5000;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_CACHED_REPOSITORY_H
