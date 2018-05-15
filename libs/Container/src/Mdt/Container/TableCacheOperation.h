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
#ifndef MDT_CONTAINER_TABLE_CACHE_OPERATION_H
#define MDT_CONTAINER_TABLE_CACHE_OPERATION_H

namespace Mdt{ namespace Container{

  /*! \brief Table cache operation
   */
  enum class TableCacheOperation
  {
    None,     /*!< No operation on the element */
    Insert,   /*!< The element was inserted to the cache */
    Update,   /*!< The element was updated in the cache */
    Delete    /*!< The element was deleted from the cache */
  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_TABLE_CACHE_OPERATION_H
