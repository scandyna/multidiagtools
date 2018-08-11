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
   * };
   * \endcode
   */
  class MDT_ENTITY_CORE_EXPORT AbstractEditableCache : public AbstractReadOnlyCache
  {
   Q_OBJECT

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

   signals:

    /*! \brief This signal is emitted after some operation changed in this cache for some rows
     *
     * \sa operationAtRow
     */
    void operationAtRowsChanged(int firstRow, int lastRow);

   private:

    Mdt::Container::TableCacheOperationMap mOperationMap;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_EDITABLE_CACHE_H
