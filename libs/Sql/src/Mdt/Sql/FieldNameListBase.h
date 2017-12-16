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
#ifndef MDT_SQL_FIELD_NAME_LIST_BASE_H
#define MDT_SQL_FIELD_NAME_LIST_BASE_H

#include <QStringList>

namespace Mdt{ namespace Sql{

  namespace Schema{
    class AutoIncrementPrimaryKey;
  }
  class FieldName;

  /*! \brief Base class for list of field names
   */
  template<typename Derived>
  class FieldNameListBase
  {
   public:

    /*! \brief Construct a list of field names
     */
    template<typename...Ts>
    FieldNameListBase(const Ts & ...fieldList)
    {
      mList.reserve(sizeof...(fieldList));
      addFieldList(fieldList...);
    }

    /*! \brief Get count of elemenst
     */
    int size() const
    {
      return mList.size();
    }

    /*! \brief Check if this list is empty
     */
    bool isEmpty() const
    {
      return mList.isEmpty();
    }

    /*! \brief Get field name at index
     *
     * \pre index must be in valid range
     */
    QString at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());
      return mList.at(index);
    }

   private:

    void addFieldList()
    {
    }

    template<typename T, typename...Ts>
    void addFieldList(const T & field, const Ts & ...fieldList)
    {
      addField(field);
      addFieldList(fieldList...);
    }

    void addField(const FieldName & field)
    {
      mList.append(field.toString());
    }

//     void addField(const Schema::AutoIncrementPrimaryKey & pk)
//     {
//       mList.append(pk.fieldName());
//     }

    QStringList mList;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_FIELD_NAME_LIST_BASE_H
