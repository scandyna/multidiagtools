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
#ifndef MDT_SQL_SCHEMA_FIELD_TYPE_INFO_LIST_H
#define MDT_SQL_SCHEMA_FIELD_TYPE_INFO_LIST_H

#include "FieldTypeInfo.h"
#include <vector>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief List of FieldTypeInfo
   */
  class FieldTypeInfoList
  {
   public:

    /*! \brief STL-style const iterator
     */
    typedef std::vector<FieldTypeInfo>::const_iterator const_iterator;

    /*! \brief Append a field type info
     */
    void append(const FieldTypeInfo & fti)
    {
      pvInfoList.push_back(fti);
    }

    /*! \brief Get count of elements
     */
    int size() const
    {
      return pvInfoList.size();
    }

    /*! \brief Access field type info at given index
     *
     * \pre index must be valid
     */
    const FieldTypeInfo & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());

      return pvInfoList[index];
    }

    /*! \brief Get begin const iterator
     */
    const_iterator begin() const
    {
      return pvInfoList.cbegin();
    }

    /*! \brief Get end const iterator
     */
    const_iterator end() const
    {
      return pvInfoList.cend();
    }

   private:

    std::vector<FieldTypeInfo> pvInfoList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_TYPE_INFO_LIST_H
