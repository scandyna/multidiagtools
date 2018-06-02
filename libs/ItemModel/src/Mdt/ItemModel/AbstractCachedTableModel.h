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
#ifndef MDT_ITEM_MODEL_ABSTRACT_CACHED_TABLE_MODEL_H
#define MDT_ITEM_MODEL_ABSTRACT_CACHED_TABLE_MODEL_H

#include "AbstractTableModel.h"
#include "Mdt/Container/TableCacheOperation.h"
// #include "Mdt/Container/RowList.h"
#include "Mdt/IndexRange/RowRange.h"
#include "MdtItemModelExport.h"

namespace Mdt{ namespace ItemModel{

  /*! \brief Base class to create a cached table model
   *
   * AbstractCachedTableModel does not handle any cache itself.
   *  It helps solving the common problems in a cached table model:
   *  - Provides a implementation of headerData() that displays the state of the row in the vertical header
   *  - Provides a implementation of flags() (f.ex. removed rows cannot be edited if visible)
   *
   * When the user removes a set of rows, they are not immediatly supressed, but marked as to be removed.
   *  Only when submit() or revert() is called, the cache is updated in the underlaying storage.
   *
   * To avoid keep tracking the state of cache items,
   *  AbstractCachedTableModel only provides a interface to query the cache states.
   *
   * \sa CachedTableModel
   * \sa Mdt::Container::TableCacheOperationMap
   */
  class MDT_ITEMMODEL_EXPORT AbstractCachedTableModel : public AbstractTableModel
  {
   Q_OBJECT

    using ParentClass = AbstractTableModel;

   public:

    /*! \brief Construct a abstract cahed table model
     */
    AbstractCachedTableModel(QObject* parent = nullptr);

    /*! \brief Returns the data for the given role and section in the header with the specified orientation
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /*! \brief Get flags for \a index
     *
     * If \a index is not valid, Qt::NoItemFlags is returned.
     *
     * If this model is read only, QAbstractTableModel::flags() is returned.
     *  If this model is editable, Qt::ItemIsEditable is alos added to QAbstractTableModel::flags().
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    /*! \brief Remove \a count rows starting from \a row
     *
     * Will call removeRowsFromCache() if \a parent is not valid,
     *  otherwise it does nothing and returns false.
     *
     * \pre \a row must be >= 0
     * \pre \a count muste be >= 1
     * \pre \a row + \a count must be in valid range ( 1 <= \a row + \a count <= rowCount() ).
     */
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

//     /*! \brief Set data for \a role at \a index
//      */
//     bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

//    public slots:
// 
//     /*! \brief Submit cached data to the storage
//      */
//     bool submit() override;

   protected:

    /*! \brief Get the count of rows in the cache
     */
    virtual int cachedRowCount() const = 0;

    /*! \brief Insert \a count rows before \a row into the cache
     *
     * This method is called from insertRows() .
     *  The inserted rows should be marked as insert operation.
     *  The implementation has not to worry about calling beginInsertRows() and endInsertRows(),
     *  this is done by insertRows()
     */
    virtual bool insertRowsToCache(int row, int count) = 0;

    /*! \brief Remove \a count rows, starting from \a row , from cache
     *
     * This method is called from removeRows() .
     *  The implementation has not to worry about calling beginRemoveRows() and endRemoveRows(),
     *  this is done by removeRows()
     */
    virtual bool removeRowsFromCache(int row, int count) = 0;

//     /*! \brief Insert the data for each row in \a rowList to the storage
//      *
//      * This implementation will call insertRowToStorage() for each
//      *  row in \a rowList .
//      */
//     virtual bool insertRowsToStorage(const Mdt::Container::RowList & rowList);
// 
//     /*! \brief Insert the data for \a row to the storage
//      */
//     virtual bool insertRowToStorage(int row) = 0;

//     /*! \brief Insert the data for \a row into the storage
//      */
//     virtual bool insertRowIntoStorage(int row) = 0;

    /*! \brief Get the operation at \a row in the cache
     *
     * \pre \a row must be >= 0
     */
    virtual Mdt::Container::TableCacheOperation operationAtRow(int row) const = 0;

   private:

    int rowCountImpl() const override
    {
      return cachedRowCount();
    }

    bool insertRowsImpl(int row, int count) override
    {
      return insertRowsToCache(row, count);
    }

//     bool removeRowsImpl(int row, int count) override
//     {
//       return removeRowsFromCache(row, count);
//     }

//     bool insertRowsIntoStorage(const Mdt::Container::RowList & rows);
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ABSTRACT_CACHED_TABLE_MODEL_H
