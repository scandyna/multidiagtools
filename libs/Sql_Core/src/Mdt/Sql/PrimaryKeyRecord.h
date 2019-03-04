/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_SQL_PRIMARY_KEY_RECORD_H
#define MDT_SQL_PRIMARY_KEY_RECORD_H

#include "FieldName.h"
#include "FieldNameValueMap.h"
// #include "Schema/AutoIncrementPrimaryKey.h"
// #include "Schema/PrimaryKey.h"
#include "MdtSql_CoreExport.h"
#include <QVariant>
#include <QVariantList>
#include <QStringList>

namespace Mdt{ namespace Sql{

  /*! \brief Holds values associated to their field names for a primary key
   */
  class MDT_SQL_CORE_EXPORT PrimaryKeyRecord
  {
   public:

//     /*! \brief Construct a null record
//      */
//     PrimaryKeyRecord() = default;
// 
//     /*! \brief Construct a record with \a primaryKey and \a value
//      *
//      * \pre \a primaryKey must not be null
//      */
//     PrimaryKeyRecord(const Schema::AutoIncrementPrimaryKey & primaryKey, const QVariant & value);
// 
//     /*! \brief Construct a record with \a primaryKey and \a values
//      *
//      * \pre \a primaryKey must not be null
//      * \pre count of fields in \a primaryKey must match count of values in \a values
//      */
//     PrimaryKeyRecord(const Schema::PrimaryKey & primaryKey, const QVariantList & values);

    /*! \brief Add a value to this record
     *
     * \pre \a fieldName must not be null
     * \pre \a fieldName must not allready exist in this record
     */
    void addValue(const FieldName & fieldName, const QVariant & value);

    /*! \brief Get the value for \a fieldName
     *
     * If no value exists for \a fieldName in this record,
     *  a null QVariant is returned.
     *
     * \pre \a fieldName must not be empty
     */
    QVariant value(const QString & fieldName) const;

    /*! \brief Get field name at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < fieldCount() ) .
     */
    QString fieldNameAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < fieldCount());

      return mMap.fieldNameAt(index);
    }

    /*! \brief Get the value at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < fieldCount() ) .
     */
    QVariant valueAt(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < fieldCount());

      return mMap.valueAt(index);
    }

    /*! \brief Check if this record is null
     */
    bool isNull() const
    {
      return mMap.isEmpty();
    }

    /*! \brief Get the count of fields in this record
     */
    int fieldCount() const
    {
      return mMap.size();
    }

    /*! \brief Get a list of field names this record is containing
     */
    QStringList toFieldNameList() const
    {
      return mMap.toFieldNameList();
    }

    /*! \brief Get a list of values this record is containing
     */
    QVariantList toValueList() const
    {
      return mMap.toValueList();
    }

   private:

    FieldNameValueMap mMap;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_PRIMARY_KEY_RECORD_H
