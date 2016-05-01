/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_CABLE_LIST_UNIT_CONNECTION_PK_LIST_H
#define MDT_CABLE_LIST_UNIT_CONNECTION_PK_LIST_H

#include "UnitConnectionPk.h"
#include <vector>

namespace Mdt{ namespace CableList{

  /*! \brief List of unit connection Primary Key (PK)
   */
  class UnitConnectionPkList
  {
   public:

    /*! \brief STL const iterator
     */
    typedef std::vector<UnitConnectionPk>::const_iterator const_iterator;

    /*! \brief Get a const_iterator to the begin of PK list
     */
    const_iterator begin() const
    {
      return pvPkList.begin();
    }

    /*! \brief Get a const_iterator to the end of PK list
     */
    const_iterator end() const
    {
      return pvPkList.end();
    }

    /*! \brief Get a const_iterator to the begin of PK list
     */
    const_iterator cbegin() const
    {
      return pvPkList.cbegin();
    }

    /*! \brief Get a const_iterator to the end of PK list
     */
    const_iterator cend() const
    {
      return pvPkList.cend();
    }

    /*! \brief Append a PK
     */
    void append(UnitConnectionPk pk)
    {
      pvPkList.push_back(pk);
    }

    /*! \brief Get PK ad index
     *
     * \pre index must be in valid range
     */
    UnitConnectionPk at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());
      return pvPkList[index];
    }

    /*! \brief Get PK list size
     */
    int size() const
    {
      return pvPkList.size();
    }

    /*! \brief Check if PK list is empty
     */
    bool isEmpty() const
    {
      return pvPkList.empty();
    }

    /*! \brief Clear PK list
     */
    void clear()
    {
      pvPkList.clear();
    }

   private:

    std::vector<UnitConnectionPk> pvPkList;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_UNIT_CONNECTION_PK_LIST_H
