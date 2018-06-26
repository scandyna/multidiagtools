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
#ifndef MDT_SQL_SELECT_FIELD_ITEM_H
#define MDT_SQL_SELECT_FIELD_ITEM_H

#include "SelectField.h"
#include "SelectTable.h"
#include "FieldName.h"
#include <QString>
#include <boost/variant.hpp>

namespace Mdt{ namespace Sql{

  /*! \brief Tag for SelectFieldItem
   */
  struct SelectAllField
  {
  };

  /*! \brief Raw SQL container for SelectFieldItem
   */
  struct SelectFieldRawSql
  {
    explicit SelectFieldRawSql(const QString & sql)
     : sql(sql)
    {
    }

    QString sql;
  };

  /*! \brief A item of SelectFieldList
   */
  class SelectFieldItem
  {
   public:

    /*! \brief Construct a item by specifying table and field
     *
     * \pre table must not be null
     * \pre field must not be null
     */
    SelectFieldItem(const SelectTable & table, const FieldName & field, const QString & fieldAlias = QString());

    /*! \brief Construct a item by specifying field
     *
     * \pre field must not be null
     */
    SelectFieldItem(const FieldName & field, const QString & fieldAlias = QString());

    /*! \brief Construct a item to select all fields in table
     *
     * \pre table must not be null
     */
    SelectFieldItem(const SelectTable & table, const SelectAllField & allField);

    /*! \brief Construct a item to select all fields (no table defined)
     */
    SelectFieldItem(const SelectAllField & allField);

    /*! \brief Construct a item for custom SQL
     *
     * \pre SQL must not be a empty string
     */
    SelectFieldItem(const SelectFieldRawSql & sql);

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return mTableName;
    }

    /*! \internal Access item variant
     *
     * Used by transforms and unit tests.
     */
    const auto & item() const
    {
      return mItem;
    }

   private:

    QString mTableName;
    boost::variant<SelectField, SelectAllField, SelectFieldRawSql> mItem;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SELECT_FIELD_ITEM_H
