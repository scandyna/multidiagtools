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
#ifndef MDT_CONTAINER_TABLE_CACHE_ROW_TRANSACTION_LIST_H
#define MDT_CONTAINER_TABLE_CACHE_ROW_TRANSACTION_LIST_H

#include "TableCacheRowTransaction.h"
#include "Vector.h"

namespace Mdt{ namespace Container{

  /*! \brief A list of table cache row transactions
   */
  using TableCacheRowTransactionList = Vector<TableCacheRowTransaction>;

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_TABLE_CACHE_ROW_TRANSACTION_LIST_H
