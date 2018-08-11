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
#ifndef MDT_ENTITY_ABSTRACT_EDITABLE_CACHE_TABLE_MODEL_H
#define MDT_ENTITY_ABSTRACT_EDITABLE_CACHE_TABLE_MODEL_H

#include "AbstractEditableCache.h"
#include "MdtEntity_CoreExport.h"
#include <QAbstractTableModel>
#include <QPointer>
#include <QMetaObject>

namespace Mdt{ namespace Entity{

  /*! \brief Base class to create table models using AbstractEditableCache
   */
  class AbstractEditableCacheTableModel : public QAbstractTableModel
  {
   Q_OBJECT

    using ParentClass = QAbstractTableModel;

   public:

    /*! \brief Constructor
     */
    explicit AbstractEditableCacheTableModel(QObject *parent = nullptr);

    AbstractEditableCacheTableModel(const AbstractEditableCacheTableModel &) = delete;
    AbstractEditableCacheTableModel & operator=(const AbstractEditableCacheTableModel &) = delete;
    AbstractEditableCacheTableModel(AbstractEditableCacheTableModel &&) = delete;
    AbstractEditableCacheTableModel & operator=(AbstractEditableCacheTableModel &&) = delete;

    /*! \brief Set \a cache to this model
     *
     * \note This model will not take ownership of \a cache ,
     *    because it could be shared with other part of the application.
     *
     * \sa cache()
     */
    void setCache(AbstractEditableCache *cache);

    /*! \brief Get cache
     *
     * Returns a nullptr if not cache was set
     *
     * \sa setCache()
     */
    AbstractEditableCache *cache() const
    {
      return mCache;
    }

    /*! \brief Get flags for \a index
     */
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    /*! \brief Get count of rows
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get count of columns
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get the header data at \a section for \a orientation and \a role
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /*! \brief Get the data at \a index for \a role
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    /*! \brief Set data for \a role at \a index to \a value
     */
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

   private slots:

    void onDataAtRowsChanged(int firstRow, int lastRow);
    void onOperationAtRowsChanged(int firstRow, int lastRow);

   private:

    QPointer<AbstractEditableCache> mCache;
    QMetaObject::Connection mCacheAboutToBeResetConnection;
    QMetaObject::Connection mCacheResetConnection;
    QMetaObject::Connection mDataChangedConnection;
    QMetaObject::Connection mOperationAtRowsChangedConnection;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_EDITABLE_CACHE_TABLE_MODEL_H
