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

#include "RowResizableTableModel.h"
#include "CachedRepositoryStlTableProxy.h"

#include "RepositoryHandle.h"
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
  //class CachedRepositoryTableModel : public RowResizableTableModel< CachedRepositoryStlTableProxy<RepositoryInterface> >
  class CachedRepositoryTableModel : public Mdt::ItemModel::AbstractCachedTableModel
  {
   public:

    using ParentClass = Mdt::ItemModel::AbstractCachedTableModel;
    using ParentClass::ParentClass;

    using record_type = typename RepositoryInterface::record_type;

//     using ParentClass = RowResizableTableModel< CachedRepositoryStlTableProxy<RepositoryInterface> >;
//     using ParentClass::ParentClass;
//     using ParentClass::table;
//     using ParentClass::beginResetModel;
//     using ParentClass::endResetModel;
//     using ParentClass::connect;
//     using ParentClass::disconnect;
//     using ParentClass::rowCount;

    /*! \brief Set a repository to this table model
     */
    void setRepository(const RepositoryHandle<RepositoryInterface> & repositoryHandle)
    {
      beginResetModel();

      disconnect(mCacheAboutToBeResetConnection);
      disconnect(mCacheResetConnection);
//       disconnect(mVerticalHeaderDataChangedConnection);

      mRepositoryHandle = repositoryHandle;

      mCacheAboutToBeResetConnection =
        connect( &mRepositoryHandle.repository(), &RepositoryInterface::cacheAboutToBeReset, this, &CachedRepositoryTableModel::beginResetModel );
      mCacheResetConnection =
        connect( &mRepositoryHandle.repository(), &RepositoryInterface::cacheReset, this, &CachedRepositoryTableModel::endResetModel );

      endResetModel();

//       table().setRepository(repository);
//       mCacheAboutToBeResetConnection =
//         connect( &table().repository().repository(), &RepositoryInterface::cacheAboutToBeReset, this, &CachedRepositoryTableModel::beginResetModel );
//       mCacheResetConnection =
//         connect( &table().repository().repository(), &RepositoryInterface::cacheReset, this, &CachedRepositoryTableModel::endResetModel );
//       mVerticalHeaderDataChangedConnection =
//         connect( &table().repository().repository(), &RepositoryInterface::operationAtRowsChanged, this, &CachedRepositoryTableModel::emitVerticalHeaderDataChanged );
//       endResetModel();
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

    bool insertRowsToCache(int row, int count) override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row <= rowCount());
      Q_ASSERT(count >= 1);

      if(mRepositoryHandle.isNull()){
        return false;
      }
      mRepositoryHandle.repository().insertRecords(row, count, record_type());

      return true;
    }

    Mdt::Container::TableCacheOperation operationAtRow(int row) const override
    {
      Q_ASSERT(!mRepositoryHandle.isNull());

      return mRepositoryHandle.constRepository().operationAtRow(row);
    }

    RepositoryHandle<RepositoryInterface> mRepositoryHandle;
    QMetaObject::Connection mCacheAboutToBeResetConnection;
    QMetaObject::Connection mCacheResetConnection;
//     QMetaObject::Connection mVerticalHeaderDataChangedConnection;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_CACHED_REPOSITORY_TABLE_MODEL_H
