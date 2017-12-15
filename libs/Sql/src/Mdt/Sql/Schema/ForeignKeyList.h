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
#ifndef MDT_SQL_SCHEMA_FOREIGN_KEY_LIST_H
#define MDT_SQL_SCHEMA_FOREIGN_KEY_LIST_H

#include "ForeignKey.h"
#include <QVector>

namespace Mdt{ namespace Sql{ namespace Schema{

  class Field;
  class FieldList;
  class ForeignTable;
  class ForeignField;
  class ForeignFieldList;

  /*! \brief List of ForeignKey
   */
  class ForeignKeyList
  {
   public:

    /*! \brief STL-style const iterator
     */
    typedef QVector<ForeignKey>::const_iterator const_iterator;

    /*! \brief Append a foreign key
     */
    void append(const ForeignKey & fk)
    {
      mForeignKeyList.append(fk);
    }

    /*! \brief Add a foreign key
     *
     * \param tableName Name of the table that contains this foreign key (also named child table)
     * \param field The field of table (designed by tableName) that will be part of the foreign key
     * \param foreignTable The table to which the foreign key refers
     * \param foreignField The field, in foreignTable, to which the foreign key refers
     * \param settings The settings for the foreign key
     * \pre field must not be null
     * \pre foreignTable's name must not be empty
     * \pre foreignField's name must not be empty
     */
    void addForeignKey(const QString & tableName, const Field & field, const ForeignTable & foreignTable, const ForeignField & foreignField, const ForeignKeySettings & settings);

    /*! \brief Add a foreign key
     *
     * \param tableName Name of the table that contains this foreign key (also named child table)
     * \param fieldList A list of fields of table (designed by tableName) that will be part of the foreign key
     * \param foreignTable The table to which the foreign key refers
     * \param foreignFieldList A list of fields, in foreignTable, to which the foreign key refers
     * \param settings The settings for the foreign key
     * \pre fieldList must not contain any null field
     * \pre foreignTable's name must not be empty
     * \pre foreignFieldList must not contains any field with a empty field name
     * \pre fieldList and foreignFieldList must be of same size
     */
    void addForeignKey(const QString & tableName, const FieldList & fieldList, const ForeignTable & foreignTable, const ForeignFieldList & foreignFieldList, const ForeignKeySettings & settings);

    /*! \brief Update (child) table name for each foreign key in this list
     *
     * This is the same as updateChildTableName()
     */
    void updateTableName(const QString & tableName);

    /*! \brief Update child table name for each foreign key in this list
     *
     * This is the same as updateTableName()
     */
    void updateChildTableName(const QString & tableName);

    /*! \brief Get count of elements
     */
    int size() const
    {
      return mForeignKeyList.size();
    }

    /*! \brief Check if list is empty
     */
    bool isEmpty() const
    {
      return mForeignKeyList.isEmpty();
    }

    /*! \brief Get foreign key at index
     *
     * \pre index must be valid
     */
    const ForeignKey & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());

      return mForeignKeyList.at(index);
    }

    /*! \brief Get foreign key that references table designed by tableName
     *
     * Note that searching tableName is done in a case insensitive way.
     */
    ForeignKey foreignKeyReferencing(const QString & tableName) const;

    /*! \brief Get begin const iterator
     */
    const_iterator begin() const
    {
      return mForeignKeyList.cbegin();
    }

    /*! \brief Get end const iterator
     */
    const_iterator end() const
    {
      return mForeignKeyList.cend();
    }

    /*! \brief Clear
     */
    void clear()
    {
      mForeignKeyList.clear();
    }

   private:

    QVector<ForeignKey> mForeignKeyList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FOREIGN_KEY_LIST_H
