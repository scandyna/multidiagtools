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

#include "ForeignKeyAction.h"
#include "ForeignKeySettings.h"
#include <QString>
#include <QStringList>

namespace Mdt{ namespace Sql{ namespace Schema{

  template<typename T>
  class TableTemplate;

  class Table;
  class Index;
  class Field;
  class FieldList;
  class ForeignField;
  class ForeignFieldList;

  /*! \brief Foreign key information for SQL database schema creation
   */
  class ForeignKey
  {
   public:

    /*! \brief Set table name
     *
     * Table name refers to table that contains the foreign key.
     *
     * This is the same as setChildTableName()
     */
    void setTableName(const QString & name)
    {
      mTableName = name;
    }

    /*! \brief Set table
     *
     * Refers to the table that contains this foreign key.
     *
     * This is the same as setChildTable(const Table & table)
     */
    void setTable(const Table & table);

    /*! \brief Set table
     *
     * Refers to the table that contains this foreign key.
     *
     * This is the same as setChildTable(const TableTemplate<T> & table)
     */
    template<typename T>
    void setTable(const TableTemplate<T> & table)
    {
      mTableName = table.tableName();
    }

    /*! \brief Get table name
     *
     * tableName refers to the table that contains this foreign key.
     *
     * This is the same as childTableName()
     */
    QString tableName() const
    {
      return mTableName;
    }

    /*! \brief Set child table name
     *
     * This is the same as setTableName()
     */
    void setChildTableName(const QString & name)
    {
      mTableName = name;
    }

    /*! \brief Set child table
     *
     * This is the same as setTable(const Table & table)
     */
    void setChildTable(const Table & table);

    /*! \brief Set child table
     *
     * This is the same as setTable(const TableTemplate<T> & table)
     */
    template<typename T>
    void setChildTable(const TableTemplate<T> & table)
    {
      mTableName = table.tableName();
    }

    /*! \brief Get child table name
     *
     * This is the same as tableName()
     */
    QString childTableName() const
    {
      return mTableName;
    }

    /*! \brief Set foreign table name
     *
     * This is the same as setParentTableName(const Table & table)
     */
    void setForeignTableName(const QString & name)
    {
      mForeignTableName = name;
    }

    /*! \brief Set foreign table
     *
     * This is the same as setParentTable(const Table & table)
     */
    void setForeignTable(const Table & table);

    /*! \brief Set foreign table
     *
     * This is the same as setParentTable(const TableTemplate & table)
     */
    template<typename T>
    void setForeignTable(const TableTemplate<T> & table)
    {
      mForeignTableName = table.tableName();
    }

    /*! \brief Get foreign table name
     *
     * This is the same as parentTableName()
     */
    QString foreignTableName() const
    {
      return mForeignTableName;
    }

    /*! \brief Set parent table
     *
     * This is the same as setForeignTableName()
     */
    void setParentTableName(const QString & tableName)
    {
      mForeignTableName = tableName;
    }

    /*! \brief Set parent table
     *
     * This is the same as setForeignTable(const Table & table)
     */
    void setParentTable(const Table & table);

    /*! \brief Set parent table
     *
     * This is the same as setForeignTable(const TableTemplate<T> & table)
     */
    template<typename T>
    void setParentTable(const TableTemplate<T> & table)
    {
      mForeignTableName = table.tableName();
    }

    /*! \brief Get parent table name
     *
     * This is the same as foreignTableName()
     */
    QString parentTableName() const
    {
      return mForeignTableName;
    }

    /*! \brief Set fields that links table and foreign table
     *
     * \param field The field in the (child) table that is part of this foreign key
     * \param foreignTableField The field in foreign (parent) table to which field refers to
     * \pre field must not be null
     * \pre foreignTableField must not be a empty field name
     */
    void setFields(const Field & field, const ForeignField & foreignTableField);

    /*! \brief Set fields that links table and foreign table
     *
     * \param fieldList A list of fields in the (child) table that is part of this foreign key
     * \param foreignTableFieldList A list of fields in foreign (parent) table to which fieldList refers to
     * \pre fieldList must not contain a null field
     * \pre foreignTableFieldList must not contain a empty field name
     * \pre fieldList and foreignTableFieldList must be of the same size
     */
    void setFields(const FieldList & fieldList, const ForeignFieldList & foreignTableFieldList);

    /*! \brief Add a couple of relation fields
     *
     * \param parentTableFieldName Field name in parent table (also called referenced table)
     * \param childTableFieldName Field name in child table (also called referencing table)
     * \pre both field name must not be empty
     */
    [[deprecated]]
    void addKeyFields(const ParentTableFieldName & parentTableFieldName, const ChildTableFieldName & childTableFieldName);

    /*! \brief Get field name list of table
     *
     * This is the same as childTableFieldNameList()
     */
    QStringList fieldNameList() const
    {
      return mFieldNameList;
    }

    /*! \brief Get field name list of foreign table
     *
     * This is the same as parentTableFieldNameList()
     */
    QStringList foreignTableFieldNameList() const
    {
      return mForeignTableFieldNameList;
    }

    /*! \brief Get field name list of child table
     *
     * This is the same as fieldNameList()
     */
    QStringList childTableFieldNameList() const
    {
      return mFieldNameList;
    }

    /*! \brief Get field name list of parent table
     *
     * This is the same as foreignTableFieldNameList()
     */
    QStringList parentTableFieldNameList() const
    {
      return mForeignTableFieldNameList;
    }

    /*! \brief Set this foreign key indexed
     *
     * When a ForeignKey is indexed, a index exists for the table that contains this foreign key.
     *  All fields, for this (child) table, that are part of this foreign key, are indexed.
     *  Note: this has no impact on foreign (parent) table.
     */
    void setIndexed(bool indexed)
    {
      mSettings.setIndexed(indexed);
    }

    /*! \brief Check if this foreign key is indexed
     *
     * \sa setIndexed()
     */
    bool isIndexed() const
    {
      return mSettings.isIndexed();
    }

    /*! \brief Get a index for table that contains this foreign key
     *
     * This is the same as getChildTableIndex()
     */
    Index getIndex() const;

    /*! \brief Get a index for child table
     *
     * This is the same as getIndex()
     */
    Index getChildTableIndex() const;

    /*! \brief Set on delete action
     */
    void setOnDeleteAction(ForeignKeyAction action)
    {
      mSettings.setOnDeleteAction(action);
    }

    /*! \brief Get on delete action
     */
    ForeignKeyAction onDeleteAction() const
    {
      return mSettings.onDeleteAction();
    }

    /*! \brief Set on update action
     */
    void setOnUpdateAction(ForeignKeyAction action)
    {
      mSettings.setOnUpdateAction(action);
    }

    /*! \brief Get on update action
     */
    ForeignKeyAction onUpdateAction() const
    {
      return mSettings.onUpdateAction();
    }

    /*! \brief Check if this foreign key is null
     *
     * ForeignKey is null if one mandatory attribute is not set.
     *  Mandatory attributes are:
     *  - Table name (also named child table name)
     *  - Foreign table name (also named parent table name)
     *  - at least one pair of fields must be set
     */
    bool isNull() const
    {
      return ( mTableName.isEmpty() || mForeignTableName.isEmpty() || mForeignTableFieldNameList.isEmpty() || mFieldNameList.isEmpty() );
    }

    /*! \brief Clear
     */
    void clear();

   private:

    ForeignKeySettings mSettings;
    QString mTableName;
    QString mForeignTableName;
    QStringList mFieldNameList;
    QStringList mForeignTableFieldNameList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FOREIGN_KEY_H
