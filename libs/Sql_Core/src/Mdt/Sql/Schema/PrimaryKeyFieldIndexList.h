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
#ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_FIELD_INDEX_LIST_H
#define MDT_SQL_SCHEMA_PRIMARY_KEY_FIELD_INDEX_LIST_H

#include "FieldIndexList.h"
#include "MdtSql_CoreExport.h"

namespace Mdt{ namespace Sql{ namespace Schema{

  class PrimaryKey;
  class AutoIncrementPrimaryKey;
  class FieldList;

  /*! \brief A list of field indexes specific for a primary key
   *
   * PrimaryKeyFieldIndexList is similar to FieldIndexList,
   *  but provides methods specific to primary keys.
   */
  class MDT_SQL_CORE_EXPORT PrimaryKeyFieldIndexList
  {
   public:

    /*! \brief Add \a fieldIndex to this list
     *
     * \pre \a fieldIndex must be >= 0
     * \pre \a fieldIndex must not allready exist in this list
     */
    void addFieldIndex(int fieldIndex)
    {
      Q_ASSERT(fieldIndex >= 0);
      Q_ASSERT(!containsFieldIndex(fieldIndex));

      mList.addFieldIndex(fieldIndex);
    }

    /*! \brief Set \a fieldIndexList to this list
     */
    void setFieldIndexList(const FieldIndexList & fieldIndexList)
    {
      mList = fieldIndexList;
    }

    /*! \brief Get count of field indexes in this list
     */
    int count() const
    {
      return mList.count();
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const
    {
      return mList.isEmpty();
    }

    /*! \brief Check if this list contains \a fieldIndex
     *
     * \pre \a fieldIndex must be >= 0
     */
    bool containsFieldIndex(int fieldIndex) const
    {
      Q_ASSERT(fieldIndex >= 0);

      return mList.containsFieldIndex(fieldIndex);
    }

    /*! \brief Get field index at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < count() ).
     */
    int fieldIndexAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < count());

      return mList.fieldIndexAt(index);
    }

    /*! \brief Get a auto increment primary key from this list
     *
     * \pre this list must contain exactly 1 field index
     * \pre the field index in this list must be in valid range in \a fieldList
     * \pre the type of the field, refered from field index in \a fieldList,
     *   must be of a type compatible to AutoIncrementPrimaryKey .
     */
    AutoIncrementPrimaryKey toAutoIncrementPrimaryKey(const FieldList & fieldList) const;

    /*! \brief Get a primary key from this list
     *
     * \pre this list must not be empty
     * \pre each field index in this list must be in valid range in \a fieldList
     */
    PrimaryKey toPrimaryKey(const FieldList & fieldList) const;

    /*! \brief Clear this list
     */
    void clear()
    {
      mList.clear();
    }

   private:

    FieldIndexList mList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_FIELD_INDEX_LIST_H
