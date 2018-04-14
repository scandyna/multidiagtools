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
#ifndef MDT_SQL_SCHEMA_FIELD_LIST_H
#define MDT_SQL_SCHEMA_FIELD_LIST_H

#include "Field.h"
#include "MdtSql_CoreExport.h"
#include <QVector>
#include <QStringList>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief List of fields
   */
  class MDT_SQL_CORE_EXPORT FieldList
  {
   public:

    /*! \brief STL-style const iterator
     */
    typedef QVector<Field>::const_iterator const_iterator;

    /*! \brief Construct a empty field list
     */
    FieldList() = default;

    /*! \brief Construct a list with fields in fieldList
     */
    template<typename...Ts>
    FieldList(const Ts & ...fieldList)
    {
      addFieldList(fieldList...);
    }

    /*! \brief Add field to this list
     *
     * \pre field must not be null
     */
    void addField(const Field & field)
    {
      Q_ASSERT(!field.isNull());
      pvFieldList.append(field);
    }

    /*! \brief Append a field
     *
     * \pre field must not be null
     */
    void append(const Field & field)
    {
      Q_ASSERT(!field.isNull());
      pvFieldList.append(field);
    }

    /*! \brief Get count of elements
     */
    int size() const
    {
      return pvFieldList.size();
    }

    /*! \brief Check if field list is empty
     */
    bool isEmpty() const
    {
      return pvFieldList.isEmpty();
    }

    /*! \brief Get field at given index
     *
     * \pre index must be valid
     */
    const Field & at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());

      return pvFieldList.at(index);
    }

    /*! \brief Access field at index for modification
     *
     * \pre index must be valid
     */
    Field & operator[](int index)
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());

      return pvFieldList[index];
    }

    /*! \brief Get index of field with fieldName
     *
     * If fieldName not exists in this table,
     *  -1 is returned.
     *
     * Note that field names are compared in a case insensitive way.
     *  For exapmple, Id_PK is the same field as ID_PK
     */
    int fieldIndex(const QString & fieldName) const;

    /*! \brief Check if a field with fieldName exists in this list
     *
     * Note that field names are compared in a case insensitive way.
     *  For exapmple, Id_PK is the same field as ID_PK
     */
    bool contains(const QString & fieldName) const
    {
      return (fieldIndex(fieldName) > -1);
    }

    /*! \brief Get begin const iterator
     */
    const_iterator begin() const
    {
      return pvFieldList.cbegin();
    }

    /*! \brief Get end const iterator
     */
    const_iterator end() const
    {
      return pvFieldList.cend();
    }

    /*! \brief Get a list of field names
     */
    QStringList toFieldNameList() const;

    /*! \brief Clear
     */
    void clear()
    {
      pvFieldList.clear();
    }

   private:

    /*! \brief Add a list of fields (this is the terminal for variadic addFieldList() )
     */
    void addFieldList()
    {
    }

    /*! \brief Add a list of fields
     */
    template<typename T, typename...Ts>
    void addFieldList(const T & field, const Ts & ...fieldList)
    {
      addField(field);
      addFieldList(fieldList...);
    }

    QVector<Field> pvFieldList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_LIST_H
