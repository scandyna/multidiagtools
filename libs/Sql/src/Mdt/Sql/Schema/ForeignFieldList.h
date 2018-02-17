/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_SQL_SCHEMA_FOREIGN_FIELD_LIST_H
#define MDT_SQL_SCHEMA_FOREIGN_FIELD_LIST_H

#include "Field.h"
#include "AutoIncrementPrimaryKey.h"
#include "MdtSqlExport.h"
#include <QString>
#include <QStringList>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Wrapper to pack a list of fields of foreign table when declaring foreign key
   */
  class MDT_SQL_EXPORT ForeignFieldList
  {
   public:

    ForeignFieldList() = delete;

    /*! \brief Construct a foreign field list
     *
     * \pre fieldList must contain 1 or more fields
     * \pre Each field in fieldList must have a non empty field name
     */
    template<typename...Ts>
    explicit ForeignFieldList(const Ts & ...fieldList)
    {
      static_assert( sizeof...(fieldList) > 0 , "A ForeignFieldList can not be empty" );
      mFieldNameList.reserve(sizeof...(fieldList));
      addFieldList(fieldList...);
    }

    /*! \brief Get count of filed names in this list
     */
    int size() const
    {
      return mFieldNameList.size();
    }

    /*! \brief Get list of field names
     */
    QStringList fieldNameList() const
    {
      return mFieldNameList;
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

    void addField(const QString & fieldName)
    {
      Q_ASSERT(!fieldName.isEmpty());
      mFieldNameList.append(fieldName);
    }

    void addField(const Field & field)
    {
      Q_ASSERT(!field.name().isEmpty());
      mFieldNameList.append(field.name());
    }

    void addField(const AutoIncrementPrimaryKey & pk)
    {
      Q_ASSERT(!pk.isNull());
      mFieldNameList.append(pk.fieldName());
    }

    QStringList mFieldNameList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FOREIGN_FIELD_LIST_H
