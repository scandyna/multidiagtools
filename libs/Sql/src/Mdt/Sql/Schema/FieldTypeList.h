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
#ifndef MDT_SQL_SCHEMA_FIELD_TYPE_LIST_H
#define MDT_SQL_SCHEMA_FIELD_TYPE_LIST_H

#include "FieldType.h"
#include "MdtSqlExport.h"
#include <QtGlobal>
#include <vector>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief List of FieldType
   */
  class MDT_SQL_EXPORT FieldTypeList
  {
   public:

    /*! \brief STL-style const iterator
     */
    typedef std::vector<FieldType>::const_iterator const_iterator;

    /*! \brief Append a field type
     */
    void append(FieldType ft)
    {
      pvFieldTypeList.push_back(ft);
    }

    /*! \brief Get count of elements
     */
    int size() const
    {
      return pvFieldTypeList.size();
    }

    /*! \brief Get field type at given index
     *
     * \pre index must be valid
     */
    FieldType at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());

      return pvFieldTypeList[index];
    }

    /*! \brief Get begin const iterator
     */
    const_iterator begin() const
    {
      return pvFieldTypeList.cbegin();
    }

    /*! \brief Get end const iterator
     */
    const_iterator end() const
    {
      return pvFieldTypeList.cend();
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvFieldTypeList.clear();
    }

   private:

    std::vector<FieldType> pvFieldTypeList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_TYPE_LIST_H
