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
#ifndef MDT_SQL_SCHEMA_FOREIGN_KEY_H
#define MDT_SQL_SCHEMA_FOREIGN_KEY_H

#include "Table.h"
#include "ParentTableFieldName.h"
#include "ChildTableFieldName.h"
#include <QString>
#include <vector>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*
   * Foreign key item
   */
  class ForeignKeyItem
  {
   public:

    ForeignKeyItem(const QString & parentTableFieldName, const QString & childTableFieldName)
     : pvParentTableFieldName(parentTableFieldName),
       pvChildTableFieldName(childTableFieldName)
    {
    }

    QString parentTableFieldName() const
    {
      return pvParentTableFieldName;
    }

    QString childTableFieldName() const
    {
      return pvChildTableFieldName;
    }

   private:

    QString pvParentTableFieldName;
    QString pvChildTableFieldName;
  };

  template<typename T>
  class TableTemplate;

  class Table;

  /*! \brief Foreign key information for SQL database schema creation
   */
  class ForeignKey
  {
   public:

    /*! \brief Foreign key action
     */
    enum Action
    {
      NoAction,   /*!< No action */
      Restrict,   /*!< Restrict */
      SetNull,    /*!< Set NULL */
      SetDefault, /*!< Set default */
      Cascade     /*!< Cascade */
    };

    /*! \brief Default constructor
     */
    ForeignKey()
    : pvCreateChildIndex(false),
      pvOnDeleteAction(NoAction),
      pvOnUpdateAction(NoAction)
    {
    }

    /*! \brief Set parent table
     *
     * \tparam T Must be of type Table or derived from TableTemplate
     */
//     template<typename T>
//     void setParentTable(const T & table)
//     {
//       pvParentTableName = table.tableName();
//     }

    void setParentTable(const Table & table);

    template<typename T>
    void setParentTable(const TableTemplate<T> & table)
    {
      pvParentTableName = table.tableName();
    }

    /*! \brief Get parent table name
     */
    QString parentTableName() const
    {
      return pvParentTableName;
    }

    /*! \brief Get child table name
     */
    QString childTableName() const
    {
      return pvChildTableName;
    }

    /*! \brief Check if a index must be created for child table
     */
    bool createChildIndex() const
    {
      return pvCreateChildIndex;
    }

    /*! \brief Get on delete action
     */
    Action onDeleteAction() const
    {
      return pvOnDeleteAction;
    }

    /*! \brief Get on update action
     */
    Action onUpdateAction() const
    {
      return pvOnUpdateAction;
    }

    /*! \brief Check if this foreign key is null
     *
     * ForeignKey is null if one mandatory attribute is not set.
     *  Mandatory attributes are:
     *  - parentTableName
     *  - childTableName
     *  - at least one pair of fields must be set
     */
    bool isNull() const
    {
      return ( pvParentTableName.isEmpty() || pvChildTableName.isEmpty() );
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvCreateChildIndex = false;
      pvOnDeleteAction = NoAction;
      pvOnUpdateAction = NoAction;
      pvParentTableName.clear();
      pvChildTableName.clear();
    }

   private:

    bool pvCreateChildIndex;
    Action pvOnDeleteAction;
    Action pvOnUpdateAction;
    QString pvParentTableName;
    QString pvChildTableName;
    std::vector<ForeignKeyItem> pvItems;
  };


}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FOREIGN_KEY_H
