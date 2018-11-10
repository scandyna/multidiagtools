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
#ifndef MDT_ITEM_MODEL_ABSTRACT_READ_ONLY_CACHED_TABLE_MODEL_H
#define MDT_ITEM_MODEL_ABSTRACT_READ_ONLY_CACHED_TABLE_MODEL_H

#include "Mdt/Container/VariantRecord.h"
#include "Mdt/IndexRange/RowRange.h"
#include "MdtItemModelExport.h"
#include <QAbstractTableModel>

namespace Mdt{ namespace ItemModel{

  /*! \brief Cache between a storage and a view
   *
   * When presenting a set of data, typically with Qt's Model/View framework,
   *  the view will query the model about data many times.
   *  If data must be fetched from a storage (for example network, database, ...),
   *  it is not very trivial to map it to a table, and fetching the same data
   *  again and again will be slow.
   *
   * AbstractReadOnlyCachedTableModel does not know how to fetch the data,
   *  and therefore it can not be instantiated.
   *
   * A concrete class has to be implemented:
   * \code
   * class ListPersonTableModel : public Mdt::ItemModel::AbstractReadOnlyCachedTableModel
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   ListPersonTableModel(QObject *parent = nullptr);
   *
   *   int columnCount(const QModelIndex & parent = QModelIndex()) const override
   *   {
   *     Q_UNUSED(parent)
   *     return 5;
   *   }
   *
   *  private:
   *
   *   QVariant horizontalHeaderDisplayRoleData(int column) const override;
   *   bool fetchRecords(int count) override;
   * };
   * \endcode
   */
  class MDT_ITEMMODEL_EXPORT AbstractReadOnlyCachedTableModel : public QAbstractTableModel
  {
   Q_OBJECT

   using BaseClass = QAbstractTableModel;

   public:

    /*! \brief Constructor
     */
    AbstractReadOnlyCachedTableModel(QObject *parent = nullptr);

    AbstractReadOnlyCachedTableModel(const AbstractReadOnlyCachedTableModel &) = delete;
    AbstractReadOnlyCachedTableModel & operator=(const AbstractReadOnlyCachedTableModel &) = delete;
    AbstractReadOnlyCachedTableModel(AbstractReadOnlyCachedTableModel &&) = delete;
    AbstractReadOnlyCachedTableModel & operator=(AbstractReadOnlyCachedTableModel &&) = delete;

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

    /*! \brief Get count of rows
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Returns the data for the given role and section in the header with the specified orientation
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /*! \brief Get the data at \a index for \a role
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    /*! \brief Get a record from the cache of this model at \a row
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    const Mdt::Container::VariantRecord & record(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());

      return mCache[row];
    }

    /*! \brief Fetch all data from the underlaying storage, until defined row count limit
     */
    bool fetchAll();

   protected slots:

    /*! \brief Append a record comming from backend to the cache of this model
     *
     * \pre rowCount() must be < cachedRowCountLimit()
     *   \todo Full cache must no longer be a precondition.
     *         If the cache is full, the record will simply not be stored
     * \pre \a record 's columnt count must be the same as columnCount()
     */
    void fromBackendAppendRecord(const Mdt::Container::VariantRecord & record);

   protected:

    /*! \brief Insert \a count copies of \a record before \a row to the cache of this model
     *
     * \pre \a row must be >= 0 and <= rowCount()
     * \pre \a count must be >= 1
     * \pre rowCount() + \a count must be <= cachedRowCountLimit()
     *   \todo Full cache must no longer be a precondition.
     *         If the cache is full, the record will simply not be stored
     * \pre \a record 's columnt count must be the same as columnCount()
     */
    virtual void fromBackendInsertRecords(int row, int count, const Mdt::Container::VariantRecord & record);

    /*! \brief Insert \a count copies of \a record before \a row to the cache of this model
     *
     * This method will only insert the records to the cache, and does not emit any signal
     */
    void insertRecordsToCache(int row, int count, const Mdt::Container::VariantRecord & record);

    /*! \brief Get the display role data for the horizontal header
     *
     * This method is called by headerData() if all preconditions ar satisfied.
     *
     * This default implementation returns the implementation from QAbstractTableModel .
     *
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    virtual QVariant horizontalHeaderDisplayRoleData(int column) const;

    /*! \brief Get the display role data for the vertical header
     *
     * This method is called by headerData() if all preconditions ar satisfied.
     *
     * This default implementation returns the implementation from QAbstractTableModel .
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     */
    virtual QVariant verticalHeaderDisplayRoleData(int row) const;

    /*! \brief Set data comming from backend at \a row and \a column to the cache of this model
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     *
     * \todo Should be renamed fromBackendSetValue() ?
     */
    void fromBackendSetData(int row, int column, const QVariant & data);

    /*! \brief Set data at \a row and \a column to this the cache of this model
     *
     * This method will only store the data to the cache, and does not emit any signal
     */
    void setDataToCache(int row, int column, const QVariant & data);

    /*! \brief Set a record comming from backend at \a row to the cache of this model
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     */
    void fromBackendSetRecord(int row, const Mdt::Container::VariantRecord & record);

    /*! \brief Fetch \a count records from the backend
     *
     * To add the fetched records to the cache, use fromBackendAppendRecord() .
     *
     * If the backend has less records available than \a count
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
     *     fromBackendAppendRecord(record);
     *   }
     *   return true;
     * }
     * \endcode
     */
    virtual bool fetchRecords(int count) = 0;

    /*! \brief Begins a row insertion operation
     *
     * This is a convenience version of QAbstractTableModel::beginInsertRows()
     *
     * \pre \a rowRange must be valid
     */
    void beginInsertRows(const Mdt::IndexRange::RowRange & rowRange);

   private:

    int mCachedRowCountLimit = 5000;
    std::vector<Mdt::Container::VariantRecord> mCache;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ABSTRACT_READ_ONLY_CACHED_TABLE_MODEL_H
