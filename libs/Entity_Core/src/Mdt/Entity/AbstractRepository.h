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
#ifndef MDT_ENTITY_ABSTRACT_REPOSITORY_H
#define MDT_ENTITY_ABSTRACT_REPOSITORY_H

#include "MdtEntity_CoreExport.h"
#include "Mdt/Error.h"
#include <QObject>

namespace Mdt{ namespace Entity{

  /*! \brief Base class to create a repository
   */
  class MDT_ENTITY_CORE_EXPORT AbstractRepository : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    AbstractRepository(QObject *parent = nullptr);

    AbstractRepository(const AbstractRepository &) = delete;
    AbstractRepository & operator=(const AbstractRepository &) = delete;

    AbstractRepository(AbstractRepository &&) = delete;
    AbstractRepository & operator=(AbstractRepository &&) = delete;

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   signals:

    /*! \brief This signal is emitted before the cache is cleared
     *
     * \note This signal is only emitted if the repository has a cache
     * \sa AbstractCachedRepository
     */
    void cacheAboutToBeReset();

    /*! \brief This signal is emitted after the cache has been reset
     *
     * \note This signal is only emitted if the repository has a cache
     * \sa AbstractCachedRepository
     */
    void cacheReset();

    /*! \brief This signal is emitted after some table cache operation changed for some rows
     *
     * \note This signal is only emitted if the repository has a cache
     * \note This signal is not emitted after inserting or removing rows
     * \sa AbstractCachedRepository
     */
    void operationAtRowsChanged(int firstRow, int lastRow);

    /*! \brief This signal is emitted after some data has been set some rows
     *
     * \note This signal is only emitted if the repository has a cache
     * \note This signal is not emitted after inserting or removing rows
     * \sa AbstractCachedRepository
     */
    void dataAtRowsChanged(int firstRow, int lastRow);

    /*! \brief This signal is emitted just before rows are inserted to the cache
     *
     * \note This signal is only emitted if the repository has a cache
     * \sa AbstractCachedRepository
     */
    void rowsAboutToBeInserted(int firstRow, int lastRow);

    /*! \brief This signal is emitted after rows have been inserted to the cache
     *
     * \note This signal is only emitted if the repository has a cache
     * \sa AbstractCachedRepository
     */
    void rowsInserted();

    /*! \brief This signal is emitted just before rows are removed from the cache
     *
     * \note This signal is only emitted if the repository has a cache
     * \sa AbstractCachedRepository
     */
    void rowsAboutToBeRemoved(int firstRow, int lastRow);

    /*! \brief This signal is emitted after rows have been removed from the cache
     *
     * \note This signal is only emitted if the repository has a cache
     * \sa AbstractCachedRepository
     */
    void rowsRemoved();

   protected:

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error);

    /*! \brief Begin a cache reset
     */
    void beginResetCache();

    /*! \brief Complete a cache reset
     */
    void endResetCache();

    /*! \brief Emit the operationAtRowsChanged() signal
     */
    void emitOperationAtRowsChanged(int firstRow, int lastRow);

    /*! \brief Emit the operationAtRowsChanged() signal
     */
    void emitOperationAtRowChanged(int row);

    /*! \brief Emit the dataAtRowsChanged() signal
     */
    void emitDataAtRowsChanged(int firstRow, int lastRow);

    /*! \brief Emit the dataAtRowsChanged() signal
     */
    void emitDataAtRowChanged(int row);

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

   private:

    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_REPOSITORY_H
