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
#ifndef MDT_SQL_SELECT_TABLE_H
#define MDT_SQL_SELECT_TABLE_H

#include "Schema/ForeignKeyList.h"
#include "MdtSqlExport.h"
#include <QString>

namespace Mdt{ namespace Sql{

  class TableName;

  namespace Schema{

    template<typename T>
    class TableTemplate;

    class Table;

  } // namespace Schema{

  /*! \brief Table used in a SQL SELECT statement
   *
   * In a SQL SELECT statement, mainly in a FROM clause,
   *  a table can be designed by its table name or a alias.
   *  SelectTable represents this.
   */
  class MDT_SQL_EXPORT SelectTable
  {
   public:

    /*! \brief Default constructor
     */
    SelectTable() {}

    /*! \brief Constructor
     */
    explicit SelectTable(const Schema::Table & table, const QString & alias = QString());

    /*! \brief Constructor
     */
    template <typename T>
    explicit SelectTable(const Schema::TableTemplate<T> & table, const QString & alias = QString())
     : mTableName(table.tableName()),
       mAlias(alias),
       mForeignKeyList(table.foreignKeyList())
    {
    }

    /*! \brief Constructor
     */
    explicit SelectTable(const TableName & name, const QString & alias = QString());

    /*! \brief Set table name
     */
    void setTableName(const QString & name)
    {
      mTableName = name;
    }

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return mTableName;
    }

    /*! \brief Set alias
     */
    void setAlias(const QString & alias)
    {
      mAlias = alias;
    }

    /*! \brief Get table name alias
     */
    QString alias() const
    {
      return mAlias;
    }

    /*! \brief Get alias if set, or table name else
     */
    QString aliasOrTableName() const
    {
      if(mAlias.isEmpty()){
        return mTableName;
      }else{
        return mAlias;
      }
    }

    /*! \brief Get foreign key that references table
     *
     * If no foreign key was found, a null one is returned.
     */
    Schema::ForeignKey foreignKeyReferencing(const SelectTable & table) const
    {
      return mForeignKeyList.foreignKeyReferencing(table.mTableName);
    }

    /*! \brief Check if at least 1 foreign is defined
     */
    bool hasForeignKey() const
    {
      return !mForeignKeyList.isEmpty();
    }

    /*! \brief Check if this view table is null
     *
     * SelectTable is null if table name is not set
     */
    bool isNull() const
    {
      return mTableName.isEmpty();
    }

    /*! \brief Clear
     */
    void clear()
    {
      mTableName.clear();
      mAlias.clear();
    }

   private:

    QString mTableName;
    QString mAlias;
    Schema::ForeignKeyList mForeignKeyList;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SELECT_TABLE_H
