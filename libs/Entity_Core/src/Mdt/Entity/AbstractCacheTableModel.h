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
#ifndef MDT_ENTITY_ABSTRACT_CACHE_TABLE_MODEL_H
#define MDT_ENTITY_ABSTRACT_CACHE_TABLE_MODEL_H

#include "MdtEntity_CoreExport.h"
#include <QAbstractTableModel>
#include <QMetaObject>
#include <QPointer>

namespace Mdt{ namespace Entity{

  /*! \brief Common base for AbstractReadOnlyCacheTableModel and EditableCacheTableModel
   */
  class AbstractCacheTableModel : public QAbstractTableModel
  {
   Q_OBJECT

    using ParentClass = QAbstractTableModel;

   public:

    /*! \brief Constructor
     */
    explicit AbstractCacheTableModel(QObject *parent = nullptr);

   protected:

    /*! \brief Disconnect common signals and slots
     */
    void disconnectCommonSignalsAndSlots();

    /*! \brief Connect common signals and slots
     */
    template<typename Cache, typename Model>
    void connectCommonSignalsAndSlots(QPointer<Cache> cache, Model * const model)
    {
      mCacheAboutToBeResetConnection =
        connect(cache, &Cache::cacheAboutToBeReset, model, &Model::beginResetModel);
      mCacheResetConnection =
        connect(cache, &Cache::cacheReset, model, &Model::endResetModel);
      mDataAtRowsChangedConnection =
        connect(cache, &Cache::dataAtRowsChanged, model, &Model::emitDataAtRowsChanged);
      mRowsAboutToBeInsertedConnection =
        connect(cache, &Cache::rowsAboutToBeInserted, model, &Model::beginInsertRows);
      mRowsInsertedConnection =
        connect(cache, &Cache::rowsInserted, model, &Model::endInsertRows);

    }

   protected slots:

    /*! \brief Emit the dataChanged() signal
     *
     * This helper can be used when a range of rows has changed.
     *
     * The role argument of dataChanged() will not be populated,
     *  meaning that all roles should be considered modified.
     *
     * \pre \a firstRow must be in valid range ( 0 <= \a firstRow < rowCount() )
     * \pre \a lastRow must be in valid range ( 0 <= \a lastRow < rowCount() )
     */
    void emitDataAtRowsChanged(int firstRow, int lastRow);

    /*! \brief Begins a row insertion operation
     *
     * \pre \a firstRow must be in valid range ( 0 <= \a firstRow <= rowCount() )
     * \pre \a lastRow must be >= \a firstRow
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

   private:

    QMetaObject::Connection mDataAtRowsChangedConnection;
    QMetaObject::Connection mCacheResetConnection;
    QMetaObject::Connection mCacheAboutToBeResetConnection;
    QMetaObject::Connection mRowsAboutToBeInsertedConnection;
    QMetaObject::Connection mRowsInsertedConnection;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_CACHE_TABLE_MODEL_H
