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
#ifndef MDT_CONTAINER_TABLE_CACHE_TRANSACTION_H
#define MDT_CONTAINER_TABLE_CACHE_TRANSACTION_H

#include "MdtContainerExport.h"
#include <QtGlobal>

namespace Mdt{ namespace Container{

  /*! \brief Represents a transaction in a table cache
   */
  class MDT_CONTAINER_EXPORT TableCacheTransaction
  {
   public:

    /*! \brief Construct a null transaction
     */
    constexpr TableCacheTransaction() noexcept = default;

    /*! \brief Construct a transaction with a id
     *
     * \pre \a id must be > 0
     */
    constexpr explicit TableCacheTransaction(int id) noexcept
     : mId(id)
    {
      Q_ASSERT(id > 0);
    }

    /*! \brief Copy construct a transaction from \a other
     */
    constexpr TableCacheTransaction(const TableCacheTransaction & other) noexcept = default;

    /*! \brief Copy assign \a other to this transaction
     */
    constexpr  TableCacheTransaction & operator=(const TableCacheTransaction & other) noexcept = default;

    /*! \brief Move construct a transaction from \a other
     */
    constexpr TableCacheTransaction(TableCacheTransaction && other) noexcept = default;

    /*! \brief Move assign \a other to this transaction
     */
    constexpr  TableCacheTransaction & operator=(TableCacheTransaction && other) noexcept = default;

    /*! \brief Get the id of this transaction
     */
    constexpr int id() const noexcept
    {
      return mId;
    }

    /*! \brief Check if this transaction is null
     */
    constexpr bool isNull() const noexcept
    {
      return mId < 1;
    }

   private:

    int mId = 0;
  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_TABLE_CACHE_TRANSACTION_H
