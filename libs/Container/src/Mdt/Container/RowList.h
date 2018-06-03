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
#ifndef MDT_CONTAINER_ROW_LIST_H
#define MDT_CONTAINER_ROW_LIST_H

#include "RowColumnListBase.h"
#include "MdtContainerExport.h"
#include <QMetaType>
#include <initializer_list>

namespace Mdt{ namespace Container{

  /*! \brief A list of row indexes
   */
  class MDT_CONTAINER_EXPORT RowList : public RowColumnListBase
  {
   public:

    /*! \brief Construct a empty list
     */
    RowList() noexcept = default;

    /*! \brief Construct a list of rows
     *
     * \pre Each row in \a list must be >= 0
     * \pre Each row in \a list must be unique
     */
    explicit RowList(std::initializer_list<int> list)
     : RowColumnListBase(list)
    {}

    /*! \brief Get the smallest row
     *
     * Returns -1 if this list is empty.
     */
    int smallestRow() const
    {
      return smallest();
    }

    /*! \brief Get the greatest row
     *
     * Returns -1 if this list is empty.
     */
    int greatestRow() const
    {
      return greatest();
    }

  };

}} // namespace Mdt{ namespace Container{
Q_DECLARE_METATYPE(Mdt::Container::RowList)

#endif // #ifndef MDT_CONTAINER_ROW_LIST_H
