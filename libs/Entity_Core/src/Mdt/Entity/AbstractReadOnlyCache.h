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

#include "Mdt/Error.h"
#include "MdtEntity_CoreExport.h"
#include <QObject>
#include <QVariant>
// #include <QAbstractItemModel>
#include <vector>

namespace Mdt{ namespace Entity{

  /*! \brief Record using QVariant as column data
   */
  class MDT_ENTITY_CORE_EXPORT VariantRecord
  {
   public:

    /*! \brief Construct a record with \a count columns
     */
    explicit VariantRecord(int count)
     : mRecord(count)
    {
    }

    /*! \brief Copy construct a record from \a other
     */
    VariantRecord(const VariantRecord & other) = default;

    /*! \brief Copy assign \a other to this record
     */
    VariantRecord & operator=(const VariantRecord & other) = default;

    /*! \brief Move construct a record from \a other
     */
    VariantRecord(VariantRecord && other) = default;

    /*! \brief Move assign \a other to this record
     */
    VariantRecord & operator=(VariantRecord && other) = default;

    /*! \brief Get count of columns
     */
    int columnCount() const noexcept
    {
      return mRecord.size();
    }

    /*! \brief Get \a value at \a column
     *
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    QVariant value(int column) const
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());

      return mRecord[column];
    }

    /*! \brief Set \a value at \a column
     *
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    void setValue(int column, const QVariant & value)
    {
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());

      mRecord[column] = value;
    }

   private:

    std::vector<QVariant> mRecord;
  };

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
   * Here is a example of using a cache in a table model:
   * \code
   * class PersonTableModel : public Mdt::ItemModel::AbstractCachedTableModel
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   PersonTableModel(QObject *parent = nullptr);
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

   public slots:

    /*! \brief Set data comming from backend at \a row and \a column to this cache
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    void setDataFromBackend(int row, int column, const QVariant & data);

    /*! \brief Append a record comming from backend to this cache
     *
     * \pre rowCount() must be < cachedRowCountLimit()
     * \pre \a record 's columnt count must be the same as columnCount()
     */
    void appendRecordFromBackend(const VariantRecord & record);

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

   protected:

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error);

//     /*! \brief Append a record that come from storage to the cache
//      *
//      * \pre rowCount() must be < cachedRowCountLimit()
//      * \pre \a record 's columnt count must be the same as columnCount()
//      */
//     void appendRecordToCache(const VariantRecord & record);

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

    /*! \brief Begin a cache reset
     */
    void beginResetCache();

    /*! \brief Complete a cache reset
     */
    void endResetCache();

   private:

    std::vector<VariantRecord> mCache;
    int mCachedRowCountLimit = 5000;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_READ_ONLY_CACHE_H
