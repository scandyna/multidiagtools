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
#ifndef MDT_SQL_SELECT_FIELD_LIST_H
#define MDT_SQL_SELECT_FIELD_LIST_H

#include "SelectField.h"
#include "SelectFieldItem.h"
#include <vector>

namespace Mdt{ namespace Sql{

//   struct SelectFieldListItem
//   {
//     SelectField selectField;
//     QString tableName;
// 
//     SelectFieldListItem(const QString & tbl, const SelectField & fld)
//      : selectField(fld), tableName(tbl) {}
//   };

  /*! \brief List of SelectFieldItem
   */
  class SelectFieldList
  {
   public:

    /*! \brief Add a field by specifying table and field
     *
     * \pre table must not be null
     * \pre field must not be null
     */
    void addField(const SelectTable & table, const FieldName & field, const QString & fieldAlias = QString())
    {
      Q_ASSERT(!table.isNull());
      Q_ASSERT(!field.isNull());
      mItemList.emplace_back(table, field, fieldAlias);
    }

    /*! \brief Add a field by specifying field
     *
     * \pre field must not be null
     */
    void addField(const FieldName & field, const QString & fieldAlias = QString())
    {
      Q_ASSERT(!field.isNull());
      mItemList.emplace_back(field, fieldAlias);
    }

    /*! \brief Add all fields for table (the * in SQL)
     *
     * \pre table must not be null
     */
    void addAllFields(const SelectTable & table)
    {
      Q_ASSERT(!table.isNull());
      mItemList.emplace_back(table, SelectAllField());
    }

    /*! \brief Add all fields (the * in SQL, not table defined)
     */
    void addAllFields()
    {
      mItemList.emplace_back(SelectAllField());
    }

    /*! \brief Add a raw SQL string
     *
     * \pre sql must not be a empty string
     */
    void addRawSqlFieldExpression(const QString & sql)
    {
      Q_ASSERT(!sql.isEmpty());
      mItemList.emplace_back(SelectFieldRawSql(sql));
    }

    /*! \brief Add a item
     */
    [[deprecated]]
    void append(const QString & tableName, const SelectField & selectField)
    {
//       pvItemList.emplace_back(tableName, selectField);
    }

    /*! \brief Get count of elements
     */
    int size() const
    {
      return mItemList.size();
    }

    /*! \brief Check if field list is empty
     */
    bool isEmpty() const
    {
      return mItemList.empty();
    }

    /*! \brief Get select field item at index
     *
     * \pre index must be in valid range
     */
    const SelectFieldItem & itemAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());
      return mItemList[index];
    }

    /*! \brief Get select field at index
     *
     * \pre index must be valid
     */
    [[deprecated]]
    const SelectField & selectFieldAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());
//       return pvItemList[index].selectField;
    }

    /*! \brief Get table name or alias at index
     *
     * \pre index must be in valid range
     */
    QString tableNameAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());
      return mItemList[index].tableName();
    }

    /*! \brief Clear
     */
    void clear()
    {
      mItemList.clear();
    }

   private:

//     std::vector<SelectFieldListItem> pvItemList;
    std::vector<SelectFieldItem> mItemList;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SELECT_FIELD_LIST_H
