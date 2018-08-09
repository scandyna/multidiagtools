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
#ifndef MDT_ENTITY_ABSTRACT_READ_ONLY_CACHE_TABLE_MODEL_H
#define MDT_ENTITY_ABSTRACT_READ_ONLY_CACHE_TABLE_MODEL_H

#include "AbstractReadOnlyCache.h"
#include "MdtEntity_CoreExport.h"
#include <QAbstractTableModel>
#include <QPointer>
#include <QMetaObject>

namespace Mdt{ namespace Entity{

  /*! \brief Base class to create table models using AbstractReadOnlyCache
   */
  class MDT_ENTITY_CORE_EXPORT AbstractReadOnlyCacheTableModel : public QAbstractTableModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractReadOnlyCacheTableModel(QObject *parent = nullptr);

    AbstractReadOnlyCacheTableModel(const AbstractReadOnlyCacheTableModel &) = delete;
    AbstractReadOnlyCacheTableModel & operator=(const AbstractReadOnlyCacheTableModel &) = delete;
    AbstractReadOnlyCacheTableModel(AbstractReadOnlyCacheTableModel &&) = delete;
    AbstractReadOnlyCacheTableModel & operator=(AbstractReadOnlyCacheTableModel &&) = delete;

    /*! \brief Set \a cache to this model
     *
     * \note This model will not take ownership of \a cache ,
     *    because it could be shared with other part of the application.
     *
     * \sa cache()
     */
    void setCache(AbstractReadOnlyCache *cache);

    /*! \brief Get cache
     *
     * Returns a nullptr if not cache was set
     *
     * \sa setCache()
     */
    AbstractReadOnlyCache *cache() const
    {
      return mCache;
    }

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

//    protected:

//     /*! \brief Get the display role data for the horizontal header
//      *
//      * This method is called by headerData() if all preconditions ar satisfied.
//      *
//      * This default implementation returns the implementation from QAbstractTableModel .
//      *
//      * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
//      */
//     virtual QVariant horizontalHeaderDisplayRoleData(int column) const;

   private:

    QPointer<AbstractReadOnlyCache> mCache;
    QMetaObject::Connection mCacheAboutToBeResetConnection;
    QMetaObject::Connection mCacheResetConnection;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_READ_ONLY_CACHE_TABLE_MODEL_H
