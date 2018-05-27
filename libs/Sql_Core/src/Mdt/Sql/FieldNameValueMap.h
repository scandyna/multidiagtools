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
#ifndef MDT_SQL_FIELD_NAME_VALUE_MAP_H
#define MDT_SQL_FIELD_NAME_VALUE_MAP_H

#include "FieldName.h"
#include "MdtSql_CoreExport.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantList>
#include <utility>
#include <vector>

namespace Mdt{ namespace Sql{

  /*! \brief Stores values associated with field names
   *
   * \note Field names are compared in case insensitive way.
   *   For example, id_pk is the same as Id_PK .
   */
  class MDT_SQL_CORE_EXPORT FieldNameValueMap
  {
   public:

    /*! \brief Add a value to this map
     *
     * \pre \a fieldName must not be null
     * \pre \a fieldName must not allready exist in this map
     */
    void addValue(const FieldName & fieldName, const QVariant & value);

    /*! \brief Get the value for \a fieldName
     *
     * If no value exists for \a fieldName in this map,
     *  a null QVariant is returned.
     *
     * \pre \a fieldName must not be empty
     */
    QVariant value(const QString & fieldName) const;

    /*! \brief Check if this map contains \a fieldName
     *
     * \pre \a fieldName must not be empty
     */
    bool containsFieldName(const QString & fieldName) const;

    /*! \brief Get the count of field name / value pairs in this map
     */
    int size() const
    {
      return mMap.size();
    }

    /*! \brief Check if this map is empty
     */
    bool isEmpty() const
    {
      return mMap.empty();
    }

    /*! \brief Clear this map
     */
    void clear();

    /*! \brief Get a list of field names this map is containing
     *
     * \note The order of field names in the returned list
     *    is the result of consecutive calls to addValue() .
     */
    QStringList toFieldNameList() const;

    /*! \brief Get a list of values this map is containing
     *
     * \note The order of values in the returned list
     *    is the result of consecutive calls to addValue() .
     */
    QVariantList toValueList() const;

   private:

    using MapItem = std::pair<QString, QVariant>;
    using Map = std::vector<MapItem>;
    using const_iterator = Map::const_iterator;

    static bool compareFieldNames(const QString & a, const QString & b);
    const_iterator findIteratorForFieldName(const QString & fieldName) const;
    Map  mMap;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_FIELD_NAME_VALUE_MAP_H
