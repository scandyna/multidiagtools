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
#ifndef MDT_ENTITY_ABSTRACT_READ_ONLY_CACHE_H
#define MDT_ENTITY_ABSTRACT_READ_ONLY_CACHE_H

#include "Mdt/Container/VariantRecord.h"
#include "Mdt/Error.h"
#include "Mdt/IndexRange/RowRange.h"
#include "MdtEntity_CoreExport.h"
#include <QObject>
#include <QVariant>
#include <vector>

namespace Mdt{ namespace Entity{

  /*! \brief Cache between a storage and a view
   *
   * When presenting a set of data, typically with Qt's Model/View framework,
   *  the view will query the model about data many times.
   *  If data must be fetched from a storage (for example network, database, ...),
   *  it is not very trivial to map it to a table, and fetching the same data
   *  again and again will be slow.
   *
   * AbstractReadOnlyCache does not know how to fetch the data,
   *  and therefor it can not be instantiated.
   *
   * A concrete class has to be implemented:
   * \code
   * class PersonCache : public Mdt::Entity::AbstractReadOnlyCache
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   PersonCache(QObject *parent = nullptr);
   *
   *   int columnCount() const noexcept override
   *   {
   *     return 5;
   *   }
   *
   *  private:
   *
   *   bool fetchRecords(int count) override;
   * };
   * \endcode
   *
   * \sa AbstractEditableCache
   * \sa AbstractReadOnlyCacheTableModel
   */
  class MDT_ENTITY_CORE_EXPORT AbstractReadOnlyCache : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractReadOnlyCache(QObject *parent = nullptr);

    AbstractReadOnlyCache(const AbstractReadOnlyCache &) = delete;
    AbstractReadOnlyCache & operator=(const AbstractReadOnlyCache &) = delete;
    AbstractReadOnlyCache(AbstractReadOnlyCache &&) = delete;
    AbstractReadOnlyCache & operator=(AbstractReadOnlyCache &&) = delete;

    /*! \brief Set maximum rows in cache
     *
     * \pre \a limit must be > 0
     * \sa cachedRowCountLimit()
     *
     * \todo What to do:
     *   - Clear the cache and emit resets ?
     */
    void setCachedRowCountLimit(int limit);

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
    int rowCount() const noexcept
    {
      return mCache.size();
    }

    /*! \brief Get count of columns
     */
    virtual int columnCount() const noexcept = 0;

    /*! \brief Get the horizontal header name at \a column
     *
     * This default implementation returns a emty string
     *
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    virtual QString horizontalHeaderName(int column) const;

    /*! \brief Get data at \a row and \a column
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    QVariant data(int row, int column) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());

      return mCache[row].value(column);
    }

    /*! \brief Fetch all data from the underlaying storage, until defined row count limit
     */
    bool fetchAll();

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

    /*! \brief Set record comming from backend at \a row to this cache
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a record 's columnt count must be the same as columnCount()
     */
    virtual void fromBackendSetRecord(int row, const Mdt::Container::VariantRecord & record);

    /*! \brief Insert \a count copies of \a record before \a row to this cache
     *
     * \pre \a row must be >= 0 and <= rowCount()
     * \pre \a count must be >= 1
     * \pre rowCount() + \a count must be <= cachedRowCountLimit()
     *   \todo Full cache must no longer be a precondition.
     *         If the cache is full, the record will simply not be stored
     * \pre \a record 's columnt count must be the same as columnCount()
     */
    void fromBackendInsertRecords(int row, int count, const Mdt::Container::VariantRecord & record);

    /*! \brief Remove \a count rows starting from \a row
     *
     * \pre \a row must be >= 0
     * \pre \a count must be >= 1
     * \pre \a row + \a count must be in valid range ( 1 <= \a row + \a count <= rowCount() )
     */
    void fromBackendRemoveRows(int row, int count);

   public slots:

    /*! \brief Set data comming from backend at \a row and \a column to this cache
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    void fromBackendSetData(int row, int column, const QVariant & data);

    /*! \brief Append a record comming from backend to this cache
     *
     * \pre rowCount() must be < cachedRowCountLimit()
     *   \todo Full cache must no longer be a precondition.
     *         If the cache is full, the record will simply not be stored
     * \pre \a record 's columnt count must be the same as columnCount()
     */
    void fromBackendAppendRecord(const Mdt::Container::VariantRecord & record);

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error);

   signals:

    /*! \brief This signal is emitted before the cache is cleared
     */
    void cacheAboutToBeReset();

    /*! \brief This signal is emitted after the cache has been reset
     */
    void cacheReset();

    /*! \brief This signal is emitted after some data has been set to some rows
     */
    void dataAtRowsChanged(int firstRow, int lastRow);

    /*! \brief This signal is emitted just before rows are inserted to this cache
     */
    void rowsAboutToBeInserted(int firstRow, int lastRow);

    /*! \brief This signal is emitted after rows have been inserted to this cache
     */
    void rowsInserted();

    /*! \brief This signal is emitted just before rows are removed from this cache
     */
    void rowsAboutToBeRemoved(int firstRow, int lastRow);

    /*! \brief This signal is emitted after rows have been removed from this cache
     */
    void rowsRemoved();

   protected:

    /*! \brief Fetch \a count records from the underlaying storage
     *
     * To add the fetched records to the cache, use appendRecordFromBackend() .
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
     *     VariantRecord record(3);
     *     // Fill the record
     *     appendRecordFromBackend(record);
     *   }
     *   return true;
     * }
     * \endcode
     */
    virtual bool fetchRecords(int count) = 0;

   private:

    /*! \brief Begin a cache reset
     */
    void beginResetCache();

    /*! \brief Complete a cache reset
     */
    void endResetCache();

    /*! \brief Begins a row insertion operation
     *
     * \pre \a rows must be a valid range
     */
    void beginInsertRows(Mdt::IndexRange::RowRange rows);

    /*! \brief End a row insertion operation
     */
    void endInsertRows();

    /*! \brief Begins a row removal operation
     *
     * \pre \a rows must be a valid range
     */
    void beginRemoveRows(Mdt::IndexRange::RowRange rows);

    /*! \brief End a row removal operation
     */
    void endRemoveRows();

    std::vector<Mdt::Container::VariantRecord> mCache;
    int mCachedRowCountLimit = 5000;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_READ_ONLY_CACHE_H
