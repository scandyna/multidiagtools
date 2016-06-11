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

#include "ParentTableFieldName.h"
#include "ChildTableFieldName.h"
#include "Index.h"
#include <QString>
#include <QStringList>
// #include <vector>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*
   * Foreign key item
   */
//   class ForeignKeyItem
//   {
//    public:
// 
//     ForeignKeyItem(const QString & parentTableFieldName, const QString & childTableFieldName)
//      : pvParentTableFieldName(parentTableFieldName),
//        pvChildTableFieldName(childTableFieldName)
//     {
//     }
// 
//     QString parentTableFieldName() const
//     {
//       return pvParentTableFieldName;
//     }
// 
//     QString childTableFieldName() const
//     {
//       return pvChildTableFieldName;
//     }
// 
//    private:
// 
//     QString pvParentTableFieldName;
//     QString pvChildTableFieldName;
//   };

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
     */
    void setParentTable(const Table & table);

    /*! \brief Set parent table
     */
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

    /*! \brief Set child table
     */
    void setChildTable(const Table & table);

    /*! \brief Set child table
     */
    template<typename T>
    void setChildTable(const TableTemplate<T> & table)
    {
      pvChildTableName = table.tableName();
    }

    /*! \brief Get child table name
     */
    QString childTableName() const
    {
      return pvChildTableName;
    }

    /*! \brief Set if a index must be created for child table
     */
    void setCreateChildIndex(bool create)
    {
      pvCreateChildIndex = create;
    }

    /*! \brief Check if a index must be created for child table
     */
    bool createChildIndex() const
    {
      return pvCreateChildIndex;
    }

    /*! \brief Get a index for child table
     */
    Index getChildTableIndex() const;

    /*! \brief Add a couple of relation fields
     *
     * \param parentTableFieldName Field name in parent table (also called referenced table)
     * \param childTableFieldName Field name in child table (also called referencing table)
     * \pre both field name must not be empty
     */
    void addKeyFields(const ParentTableFieldName & parentTableFieldName, const ChildTableFieldName & childTableFieldName);

    /*! \brief Get field name list of parent table
     */
    QStringList parentTableFieldNameList() const
    {
      return pvParentTableFieldNameList;
    }

    /*! \brief Get field name list of child table
     */
    QStringList childTableFieldNameList() const
    {
      return pvChildTableFieldNameList;
    }

    /*! \brief Set on delete action
     */
    void setOnDeleteAction(Action action)
    {
      pvOnDeleteAction = action;
    }

    /*! \brief Get on delete action
     */
    Action onDeleteAction() const
    {
      return pvOnDeleteAction;
    }

    /*! \brief Set on update action
     */
    void setOnUpdateAction(Action action)
    {
      pvOnUpdateAction = action;
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
      return ( pvParentTableName.isEmpty() || pvChildTableName.isEmpty() || pvParentTableFieldNameList.isEmpty() || pvChildTableFieldNameList.isEmpty() );
    }

    /*! \brief Clear
     */
    void clear();

    /*! \brief Get string representation of action
     */
    static QString actionString(Action action);

    /*! \brief Get action from actionStr
     *
     * Checks done on str are not case sensitive.
     *  F.ex. "Cascade" and "CASCADE" represents both Cascade action.
     */
    static Action actionFromString(const QString & actionStr);

   private:

    bool pvCreateChildIndex;
    Action pvOnDeleteAction;
    Action pvOnUpdateAction;
    QString pvParentTableName;
    QString pvChildTableName;
    QStringList pvParentTableFieldNameList;
    QStringList pvChildTableFieldNameList;
//     std::vector<ForeignKeyItem> pvItems;
  };


}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FOREIGN_KEY_H
