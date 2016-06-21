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
#ifndef MDT_SQL_SCHEMA_SELECT_FIELD_LIST_H
#define MDT_SQL_SCHEMA_SELECT_FIELD_LIST_H

#include "SelectField.h"
#include <vector>

namespace Mdt{ namespace Sql{ namespace Schema{

  struct SelectFieldListItem
  {
    SelectField selectField;
    QString tableName;

    SelectFieldListItem(const QString & tbl, const SelectField & fld)
     : selectField(fld), tableName(tbl) {}
  };

  /*! \brief List of SelectField
   */
  class SelectFieldList
  {
   public:

    /*! \brief Add a item
     */
    void append(const QString & tableName, const SelectField & selectField)
    {
      pvItemList.emplace_back(tableName, selectField);
    }

    /*! \brief Get count of elements
     */
    int size() const
    {
      return pvItemList.size();
    }

    /*! \brief Check if field list is empty
     */
    bool isEmpty() const
    {
      return pvItemList.empty();
    }

    /*! \brief Get select field at index
     *
     * \pre index must be valid
     */
    const SelectField & selectFieldAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());
      return pvItemList[index].selectField;
    }

    /*! \brief Get table name at index
     *
     * \pre index must be valid
     */
    const QString & tableNameAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());
      return pvItemList[index].tableName;
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvItemList.clear();
    }

   private:

    std::vector<SelectFieldListItem> pvItemList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_SELECT_FIELD_LIST_H
