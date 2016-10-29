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
  class SelectTable
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
     : pvTableName(table.tableName()),
       pvAlias(alias)
    {
    }

    /*! \brief Constructor
     */
    explicit SelectTable(const TableName & name, const QString & alias = QString());

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

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SELECT_TABLE_H
