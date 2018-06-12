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

    /*! \brief Cancel the removal of \a count rows starting from \a row
     *
     * This default implementation does nothing.
     *
     * \pre \a row must be >= 0
     * \pre \a count muste be >= 1
     * \pre \a row + \a count must be in valid range ( 1 <= \a row + \a count <= rowCount() ).
     */
    virtual void cancelRemoveRows(int row, int count);

   public slots:

    /*! \brief Begins a row insertion operation
     */
    void beginInsertRows(int firstRow, int lastRow);

    /*! \brief End a row insertion operation
     */
    void endInsertRows();

    /*! \brief Begins a row removal operation
     */
    void beginRemoveRows(int firstRow, int lastRow);

    /*! \brief End a row removal operation
     */
    void endRemoveRows();

   protected:

    /*! \brief Get the count of rows in the cache
     */
    virtual int cachedRowCount() const = 0;

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
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ABSTRACT_CACHED_TABLE_MODEL_H
