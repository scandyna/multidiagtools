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
#ifndef MDT_ENTITY_ABSTRACT_EDITABLE_CACHE_H
#define MDT_ENTITY_ABSTRACT_EDITABLE_CACHE_H

#include "AbstractReadOnlyCache.h"
#include "Mdt/Container/TableCacheOperation.h"
#include "Mdt/Container/TableCacheOperationMap.h"
#include "Mdt/Container/RowList.h"
#include "MdtEntity_CoreExport.h"

namespace Mdt{ namespace Entity{

  /*! \brief Cache between a storage and a view
   *
   * The main goal is the same as AbstractReadOnlyCache.
   *
   * A additionnal possible feature is to edit a set of records
   *  and submit them to the system at once.
   *
   * AbstractEditableCache does not know how to fetch and store the data,
   *  and therefor it can not be instantiated.
   *
   * A concrete class has to be implemented:
   * \code
   * class EditPersonCache : public Mdt::Entity::AbstractEditableCache
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
   *   bool addRecordToBackend(const VariantRecord & record) override;
   * };
   * \endcode
   */
  class MDT_ENTITY_CORE_EXPORT AbstractEditableCache : public AbstractReadOnlyCache
  {
   Q_OBJECT

    using ParentClass = AbstractReadOnlyCache;

   public:

    /*! \brief Get the operation at \a row in the cache
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     * \sa operationAtRowsChanged()
     */
    Mdt::Container::TableCacheOperation operationAtRow(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());

      return mOperationMap.operationAtRow(row);
    }

    /*! \brief Set \a data at \a row and \a column
     *
     * This will also mark the data as edited in the cache.
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    void setData(int row, int column, const QVariant & data);

    /*! \brief Set record comming from backend at \a row to this cache
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a record 's columnt count must be the same as columnCount()
     */
    void fromBackendSetRecord(int row, const Mdt::Container::VariantRecord & record) override;

    /*! \brief Insert \a count copies of \a record before \a row to this cache
     *
     * The inserted records will also be marked as inserted.
     *
     * \pre \a row must be >= 0 and <= rowCount()
     * \pre \a count must be >= 1
     * \pre rowCount() + \a count must be <= cachedRowCountLimit()
     * \pre \a record 's columnt count must be the same as columnCount()
     */
    void insertRecords(int row, int count, const Mdt::Container::VariantRecord & record);

    /*! \brief Add a row to the end of this cache
     *
     * Will add a empty record to the end of this cache
     */
    void appendRow();

    /*! \brief Remove \a count rows starting from \a row
     *
     * Rows will be marked as deleted in the cache.
     *
     * \pre \a row must be >= 0
     * \pre \a count must be >= 1
     * \pre \a row + \a count must be in valid range ( 1 <= \a row + \a count <= rowCount() )
     */
    void removeRows(int row, int count);

    /*! \brief Submit changes
     *
     * Will submit changes done in the cache to the backend.
     */
    bool submitChanges();

   signals:

    /*! \brief This signal is emitted after some operation changed in this cache for some rows
     *
     * \sa operationAtRow
     */
    void operationAtRowsChanged(int firstRow, int lastRow);

   protected:

//     /*! \brief Add \a record to the backend
//      *
//      * \pre \a record 's columnt count must be the same as columnCount()
//      */
//     virtual bool addRecordToBackend(const VariantRecord & record) = 0;

    /*! \brief Add the record at \a row to the backend
     *
     * This method must be implemented in the concrete class.
     *
     * Once the record has been successfully processed from the backend,
     *  fromBackendSetRecord() should be called to update the corresponding
     *  record to the new values.
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     */
    virtual bool addRecordToBackend(int row) = 0;

    /*! \brief Add records for \a rows to the backend
     *
     * This method can be implemented to add all records
     *  for \a rows .
     *
     * The default implementation will call addRecordToBackend()
     *  for each row.
     *
     * \pre each row in \a rows must be in valid range ( 0 <= \a row < rowCount() ).
     */
    virtual bool addRecordsToBackend(const Mdt::Container::RowList & rows);

    /*! \brief Update the record at \a row in the backend
     *
     * Once the record has been successfully processed from the backend,
     *  fromBackendSetRecord() should be called to update the corresponding
     *  record to the new values.
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     */
    virtual bool updateRecordInBackend(int row) = 0;

    /*! \brief Update records for \a rows in the backend
     *
     * The default implementation will call updateRecordInBackend()
     *  for each row.
     */
    virtual bool updateRecordsInBackend(const Mdt::Container::RowList & rows);

    /*! \brief Remove the record at \a row from the backend
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     */
    virtual bool removeRecordFromBackend(int row) = 0;

    /*! \brief Remove the records for \a rows from the backend
     *
     * The default implementation will call removeRecordFromBackend()
     *  for each row.
     *
     * \note \a rows is sorted from highest to lowest row,
     *    making it easy to deal with common Containers.
     */
    virtual bool removeRecordsFromBackend(const Mdt::Container::RowList & rows);

   private:

    bool addNewRecordsToBackend();
    bool updateModifiedRowsInBackend();
    void removeRowsToDeleteFromCacheOnly();
    bool removeRowsToDeleteFromBackend();

    Mdt::Container::TableCacheOperationMap mOperationMap;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_EDITABLE_CACHE_H
