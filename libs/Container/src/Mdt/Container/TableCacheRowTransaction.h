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
#ifndef MDT_CONTAINER_TABLE_CACHE_ROW_TRANSACTION_H
#define MDT_CONTAINER_TABLE_CACHE_ROW_TRANSACTION_H

#include "TableCacheTransaction.h"
#include "MdtContainerExport.h"
#include <QtGlobal>

namespace Mdt{ namespace Container{

  /*! \brief A transaction for a row in a table cache
   */
  class MDT_CONTAINER_EXPORT TableCacheRowTransaction
  {
   public:

    /*! \brief Construct a null transaction
     */
    constexpr TableCacheRowTransaction() noexcept = default;

    /*! \brief Copy construct a transaction from \a other
     */
    constexpr TableCacheRowTransaction(const TableCacheRowTransaction & other) noexcept = default;

    /*! \brief Copy assign \a other to this transaction
     */
    constexpr  TableCacheRowTransaction & operator=(const TableCacheRowTransaction & other) noexcept = default;

    /*! \brief Move construct a transaction from \a other
     */
    constexpr TableCacheRowTransaction(TableCacheRowTransaction && other) noexcept = default;

    /*! \brief Move assign \a other to this transaction
     */
    constexpr  TableCacheRowTransaction & operator=(TableCacheRowTransaction && other) noexcept = default;

    /*! \brief Get the row
     */
    constexpr int row() const noexcept
    {
      return mRow;
    }

    /*! \brief Get transaction
     */
    constexpr TableCacheTransaction transaction() const noexcept
    {
      return mTransaction;
    }

    /*! \brief Get transaction id
     */
    constexpr int transactionId() const noexcept
    {
      return mTransaction.id();
    }

    /*! \brief Check if this transaction is null
     */
    constexpr bool isNull() const noexcept
    {
      if(mRow < 0){
        return true;
      }
      if(mTransaction.isNull()){
        return true;
      }
      return false;
    }

   private:

    int mRow = -1;
    TableCacheTransaction mTransaction;
  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_TABLE_CACHE_ROW_TRANSACTION_H
