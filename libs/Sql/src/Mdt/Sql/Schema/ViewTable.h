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
#ifndef MDT_SQL_SCHEMA_VIEW_TABLE_H
#define MDT_SQL_SCHEMA_VIEW_TABLE_H

#include <QString>

namespace Mdt{ namespace Sql{
  class TableName;
}}

namespace Mdt{ namespace Sql{ namespace Schema{

  template<typename T>
  class TableTemplate;

  class Table;

  /*! \brief SQL table used by View
   */
  class ViewTable
  {
   public:

    /*! \brief Default constructor
     */
    ViewTable() {}

    /*! \brief Constructor
     */
    explicit ViewTable(const Table & table, const QString & alias = QString());

    /*! \brief Constructor
     */
    template <typename T>
    explicit ViewTable(const TableTemplate<T> & table, const QString & alias = QString())
     : pvTableName(table.tableName()),
       pvAlias(alias)
    {
    }

    /*! \brief Constructor
     */
    explicit ViewTable(const TableName & name, const QString & alias = QString());

    /*! \brief Set table name
     */
    void setTableName(const QString & name)
    {
      pvTableName = name;
    }

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return pvTableName;
    }

    /*! \brief Set alias
     */
    void setAlias(const QString & alias)
    {
      pvAlias = alias;
    }

    /*! \brief Get table name alias
     */
    QString alias() const
    {
      return pvAlias;
    }

    /*! \brief Get alias if set, or table name else
     */
    QString aliasOrTableName() const
    {
      if(pvAlias.isEmpty()){
        return pvTableName;
      }else{
        return pvAlias;
      }
    }

    /*! \brief Check if this view table is null
     *
     * ViewTable is null if table name is not set
     */
    bool isNull() const
    {
      return pvTableName.isEmpty();
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvTableName.clear();
      pvAlias.clear();
    }

   private:

    QString pvTableName;
    QString pvAlias;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_VIEW_TABLE_H
