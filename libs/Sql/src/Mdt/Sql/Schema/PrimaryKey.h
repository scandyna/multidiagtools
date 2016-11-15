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
#ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_H
#define MDT_SQL_SCHEMA_PRIMARY_KEY_H

#include "Field.h"
#include <QStringList>
#include <type_traits>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Primary key
   */
  class PrimaryKey
  {
   public:

    /*! \brief Construct a empty primary key
     */
    PrimaryKey() = default;

    /*! \brief Construct a primary key with a list of fields
     *
     * \pre No field in fieldList must be null
     * \pre Each field in fieldList must have a different name
     */
    template<typename...Ts>
    PrimaryKey(const Ts & ...fieldList)
    {
      mFieldNameList.reserve(sizeof...(fieldList));
      addFieldList(fieldList...);
    }

    /*! \brief Add a field to this primary key
     *
     * \pre field must not be null
     * \pre No field with the same name as field must allready been set to this primary key
     */
    void addField(const Field & field)
    {
      Q_ASSERT(!field.isNull());
      Q_ASSERT(!contains(field.name()));
      mFieldNameList.append(field.name());
    }

    /*! \brief Add a field to this primary key
     *
     * \pre name must not be empty
     * \pre No field with the same name must allready been set to this primary key
     */
    void addFieldName(const QString & name)
    {
      Q_ASSERT(!name.isEmpty());
      Q_ASSERT(!contains(name));
      mFieldNameList.append(name);
    }

    /*! \brief Get field count
     */
    int fieldCount() const
    {
      return mFieldNameList.count();
    }

    /*! \brief Check if a field with fieldName exists in this primary key
     *
     * Note that field names are compared in a case insensitive way.
     *  For exapmple, Id_PK is the same field as ID_PK
     */
    bool contains(const QString & fieldName) const
    {
      return mFieldNameList.contains(fieldName, Qt::CaseInsensitive);
    }

    /*! \brief Get field name list
     */
    QStringList fieldNameList() const
    {
      return mFieldNameList;
    }

    /*! \brief Check if primary key is null
     */
    bool isNull() const
    {
      return mFieldNameList.isEmpty();
    }

    /*! \brief Clear
     */
    void clear()
    {
      mFieldNameList.clear();
    }

   private:

    /*! \brief Helper to add fields with variadic function
     */
    void addFieldList()
    {
    }

    /*! \brief Helper to add fields with variadic function
     */
    template<typename T, typename...Ts>
    void addFieldList(const T & field, const Ts & ...fieldList)
    {
      static_assert( std::is_same<T, Field>::value , "Only field of type Field can be added to PrimaryKey" );
      addField(field);
      addFieldList(fieldList...);
    }

//     template<typename...Ts>
//     void addFieldList(const Field & field, const Ts & ...fieldList)
//     {
//       addField(field);
//       addFieldList(fieldList...);
//     }

    QStringList mFieldNameList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_H
