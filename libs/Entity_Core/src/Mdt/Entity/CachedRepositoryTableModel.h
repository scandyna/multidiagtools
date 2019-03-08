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
#ifndef MDT_ENTITY_CACHED_REPOSITORY_TABLE_MODEL_H
#define MDT_ENTITY_CACHED_REPOSITORY_TABLE_MODEL_H

#include "RepositoryHandle.h"
#include "Mdt/Error.h"
#include "Mdt/ItemModel/AbstractCachedTableModel.h"
#include <QObject>
#include <QLatin1String>

namespace Mdt{ namespace Entity{

  /*! \brief Table model that can act directly on a repository based on AbstractCachedRepository
   *
   *
   * The model can be used like this in the application:
   * \code
   * EditPersonTableModel model;
   * auto personRepository = PersonRepository::make<SqlPersonRepository>();
   * model.setRepository(personRepository);
   * \endcode
   *
   * To create a cached repository table model for a single entity,
   *  using CachedEntityRepositoryTableModel can need less work to implement.
   */
  template<typename RepositoryInterface>
  class CachedRepositoryTableModel : public Mdt::ItemModel::AbstractCachedTableModel
  {
    using ParentClass = Mdt::ItemModel::AbstractCachedTableModel;

   public:

    using ParentClass::ParentClass;

    using record_type = typename RepositoryInterface::record_type;

    /*! \brief Set a repository to this table model
     */
    void setRepository(const RepositoryHandle<RepositoryInterface> & repositoryHandle)
    {
      beginResetModel();

      disconnect(mCacheAboutToBeResetConnection);
      disconnect(mCacheResetConnection);
      disconnect(mVerticalHeaderDataChangedConnection);
      disconnect(mDataChangedConnection);
      disconnect(mRowsAboutToBeInsertedConnection);
      disconnect(mRowsInsertedConnection);
      disconnect(mRowsAboutToBeRemovedConnection);
      disconnect(mRowsRemovedConnection);

      mRepositoryHandle = repositoryHandle;

      mCacheAboutToBeResetConnection =
        connect( &mRepositoryHandle.repository(), &RepositoryInterface::cacheAboutToBeReset, this, &CachedRepositoryTableModel::beginResetModel );
      mCacheResetConnection =
        connect( &mRepositoryHandle.repository(), &RepositoryInterface::cacheReset, this, &CachedRepositoryTableModel::endResetModel );
      mVerticalHeaderDataChangedConnection =
        connect( &mRepositoryHandle.repository(), &RepositoryInterface::operationAtRowsChanged, this, &CachedRepositoryTableModel::emitVerticalHeaderDataChanged );
      mDataChangedConnection =
        connect( &mRepositoryHandle.repository(), &RepositoryInterface::dataAtRowsChanged, this, &CachedRepositoryTableModel::emitDataAtRowsChanged );
      mRowsAboutToBeInsertedConnection =
        connect( &mRepositoryHandle.repository(), &RepositoryInterface::rowsAboutToBeInserted, this, &CachedRepositoryTableModel::beginInsertRows );
      mRowsInsertedConnection =
        connect( &mRepositoryHandle.repository(), &RepositoryInterface::rowsInserted, this, &CachedRepositoryTableModel::endInsertRows );
      mRowsAboutToBeRemovedConnection =
        connect( &mRepositoryHandle.repository(), &RepositoryInterface::rowsAboutToBeRemoved, this, &CachedRepositoryTableModel::beginRemoveRows );
      mRowsRemovedConnection =
        connect( &mRepositoryHandle.repository(), &RepositoryInterface::rowsRemoved, this, &CachedRepositoryTableModel::endRemoveRows );

      endResetModel();
    }

    /*! \brief Insert \a count rows before \a row into the model
     *
     * \pre \a row must be in valid range ( 0 <= \a row <= rowCount() )
     * \pre \a count muste be >= 1
     */
    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override
    {
      if(parent.isValid()){
        return false;
      }
      if(mRepositoryHandle.isNull()){
        return false;
      }
      Q_ASSERT(row >= 0);
      Q_ASSERT(row <= rowCount());
      Q_ASSERT(count >= 0);

      mRepositoryHandle.repository().insertRecords(row, count, record_type());

      return true;
    }

    /*! \brief Remove \a count rows starting from \a row
     *
     * \pre \a row must be >= 0
     * \pre \a count muste be >= 1
     * \pre \a row + \a count must be in valid range ( 1 <= \a row + \a count <= rowCount() ).
     */
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override
    {
      if(parent.isValid()){
        return false;
      }
      Q_ASSERT(row >= 0);
      Q_ASSERT(count >= 1);
      Q_ASSERT( (row+count) <= rowCount() );
      Q_ASSERT(!mRepositoryHandle.isNull());

      mRepositoryHandle.repository().removeRecords(row, count);

      return true;
    }

    /*! \brief Cancel the removal of \a count rows starting from \a row
     *
     * \pre \a row must be >= 0
     * \pre \a count muste be >= 1
     * \pre \a row + \a count must be in valid range ( 1 <= \a row + \a count <= rowCount() ).
     */
    void cancelRemoveRows(int row, int count) override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(count >= 1);
      Q_ASSERT( (row+count) <= rowCount() );
      Q_ASSERT(!mRepositoryHandle.isNull());

      mRepositoryHandle.repository().cancelRemoveRecords(row, count);
    }

    /*! \brief Submit changes to the storage
     *
     * \note QTableView will call submit() on row change,
     *   and ignores if it succeeded or failed.
     *   This is why we use submitChanges() here.
     */
    bool submitChanges()
    {
      return mRepositoryHandle.repository().submitChanges();
    }

    /*! \brief Get last error
     *
     * This implementation returns last error from repository.
     */
    virtual Mdt::Error lastError() const
    {
      return mRepositoryHandle.constRepository().lastError();
    }

   protected:

    QVariant displayRoleData(int row, int column) const override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row <= rowCount());
      Q_ASSERT(column >= 0);
      Q_ASSERT(column <= columnCount());

      return mRepositoryHandle.constRepository().data(row, column);
    }

    bool setEditRoleData(int row, int column, const QVariant& value) override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row <= rowCount());
      Q_ASSERT(column >= 0);
      Q_ASSERT(column <= columnCount());

      mRepositoryHandle.repository().setData(row, column, value);

      return true;
    }

   private:

    int cachedRowCount() const override
    {
      if(mRepositoryHandle.isNull()){
        return 0;
      }
      return mRepositoryHandle.constRepository().rowCount();
    }

    int columnCountImpl() const override
    {
      if(mRepositoryHandle.isNull()){
        return 0;
      }
      return mRepositoryHandle.constRepository().columnCount();
    }

    Mdt::Container::TableCacheOperation operationAtRow(int row) const override
    {
      Q_ASSERT(!mRepositoryHandle.isNull());

      return mRepositoryHandle.constRepository().operationAtRow(row);
    }

    void emitVerticalHeaderDataChanged(int firstRow, int lastRow)
    {
      ParentClass::emitVerticalHeaderDataChanged(firstRow, lastRow);
    }

    void emitDataAtRowsChanged(int firstRow, int lastRow)
    {
      ParentClass::emitDataAtRowsChanged(firstRow, lastRow);
    }

    RepositoryHandle<RepositoryInterface> mRepositoryHandle;
    QMetaObject::Connection mCacheAboutToBeResetConnection;
    QMetaObject::Connection mCacheResetConnection;
    QMetaObject::Connection mVerticalHeaderDataChangedConnection;
    QMetaObject::Connection mDataChangedConnection;
    QMetaObject::Connection mRowsAboutToBeInsertedConnection;
    QMetaObject::Connection mRowsInsertedConnection;
    QMetaObject::Connection mRowsAboutToBeRemovedConnection;
    QMetaObject::Connection mRowsRemovedConnection;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_CACHED_REPOSITORY_TABLE_MODEL_H
