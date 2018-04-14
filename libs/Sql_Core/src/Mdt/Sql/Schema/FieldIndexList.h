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
#ifndef MDT_SQL_SCHEMA_FIELD_INDEX_LIST_H
#define MDT_SQL_SCHEMA_FIELD_INDEX_LIST_H

#include "MdtSql_CoreExport.h"
#include <QtGlobal>
#include <vector>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief A list of field indexes
   */
  class MDT_SQL_CORE_EXPORT FieldIndexList
  {
   public:

    /*! \brief STL compatible value type
     */
    using value_type = std::vector<int>::value_type;

    /*! \brief STL style const iterator
     */
    using const_iterator = std::vector<int>::const_iterator;

    /*! \brief Add \a fieldIndex to this list
     *
     * \pre \a fieldIndex must be >= 0
     * \pre \a fieldIndex must not allready exist in this list
     */
    void addFieldIndex(int fieldIndex);

    /*! \brief Get count of field indexes in this list
     */
    int count() const
    {
      return mList.size();
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const
    {
      return mList.empty();
    }

    /*! \brief Check if this list contains \a fieldIndex
     *
     * \pre \a fieldIndex must be >= 0
     */
    bool containsFieldIndex(int fieldIndex) const;

    /*! \brief Get field index at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < count() ).
     */
    int fieldIndexAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < count());

      return mList[index];
    }

    /*! \brief Clear this list
     */
    void clear();

    /*! \brief Get a STL style const iterator to the first item in this list
     */
    const_iterator begin() const
    {
      return mList.cbegin();
    }

    /*! \brief Get a STL style const iterator past the end in this list
     */
    const_iterator end() const
    {
      return mList.cend();
    }

    /*! \brief Get a STL style const iterator to the first item in this list
     */
    const_iterator cbegin() const
    {
      return mList.cbegin();
    }

    /*! \brief Get a STL style const iterator past the end in this list
     */
    const_iterator cend() const
    {
      return mList.cend();
    }

   private:

    std::vector<int> mList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_INDEX_LIST_H
