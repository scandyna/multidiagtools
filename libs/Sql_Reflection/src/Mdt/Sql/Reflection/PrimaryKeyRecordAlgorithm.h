/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_SQL_REFLECTION_PRIMARY_KEY_RECORD_ALGORITHM_H
#define MDT_SQL_REFLECTION_PRIMARY_KEY_RECORD_ALGORITHM_H

#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/PrimaryKeyAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsReflectedStructAssociatedWithPrimaryKey.h"
#include "Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.h"
#include "Mdt/Sql/DeleteStatement.h"
#include "Mdt/Sql/PrimaryKeyRecord.h"
#include "Mdt/Sql/FieldName.h"
#include <QVariantList>
#include <QLatin1String>
#include <QString>

namespace Mdt{ namespace Sql{ namespace Reflection{

  /*! \brief Check that a primary key record has the correct count of fields regarding the defined primary key
   *
   * \pre \a PrimaryKey must be primary key class for a reflected struct
   */
  template<typename PrimaryKey>
  bool primaryKeyRecordHasCorrectFieldCount(const Mdt::Sql::PrimaryKeyRecord & pkRecord) noexcept
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key class for a reflected struct" );

    return pkRecord.fieldCount() == Mdt::Reflection::primaryKeyFieldCount<PrimaryKey>();
  }

  /*! \brief Check that a primary key record has the correct field name list fro the define primary key
   *
   * Will also check that the order of the fields are correct
   *
   * \pre \a PrimaryKey must be primary key class for a reflected struct
   */
  template<typename PrimaryKey>
  bool primaryKeyRecordHasCorrectFieldNameList(const Mdt::Sql::PrimaryKeyRecord & pkRecord) noexcept
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key class for a reflected struct" );
    Q_ASSERT(primaryKeyRecordHasCorrectFieldCount<PrimaryKey>(pkRecord));

    const auto pkrFieldNameList = pkRecord.toFieldNameList();
    const auto pkFieldNameList = Mdt::Reflection::fieldNameListFromPrimaryKey<PrimaryKey>();
    Q_ASSERT(pkrFieldNameList.size() == pkFieldNameList.size());
    for(int i = 0; i < pkFieldNameList.size(); ++i){
      if( QString::compare(pkrFieldNameList.at(i), pkFieldNameList.at(i), Qt::CaseInsensitive) != 0 ){
        return false;
      }
    }

    return true;
  }

  namespace Impl{

    class AddFieldAndValueToPrimaryKeyRecordFromValueList
    {
     public:

      AddFieldAndValueToPrimaryKeyRecordFromValueList(Mdt::Sql::PrimaryKeyRecord & pkr, const QVariantList & values)
       : mPkr(pkr),
         mValues(values)
      {
      }

      template<typename Field>
      void operator()(Field)
      {
        using Mdt::Sql::FieldName;

        Q_ASSERT(mIndex < mValues.count());

        mPkr.addValue( FieldName(QString::fromLatin1(Mdt::Reflection::fieldName<Field>())), mValues.at(mIndex) );
        ++mIndex;
      }

     private:

      int mIndex = 0;
      Mdt::Sql::PrimaryKeyRecord & mPkr;
      const QVariantList & mValues;
    };

  } // namespace Impl{

  /*! \brief Get a SQL primary key record from a list of values
   *
   * \pre \a PrimaryKey must be primary key class for a reflected struct
   * \pre \a pkValues must contain the same count of values than the count of fields defined in \a PrimaryKey
   */
  template<typename PrimaryKey>
  Mdt::Sql::PrimaryKeyRecord primaryKeyRecordFromValues(const QVariantList & pkValues)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key class for a reflected struct" );
    Q_ASSERT( pkValues.count() == Mdt::Reflection::primaryKeyFieldCount<PrimaryKey>() );

    Mdt::Sql::PrimaryKeyRecord pkr;
    Impl::AddFieldAndValueToPrimaryKeyRecordFromValueList f(pkr, pkValues);

    Mdt::Reflection::forEachPrimaryKeyField<PrimaryKey>(f);
    Q_ASSERT( primaryKeyRecordHasCorrectFieldNameList<PrimaryKey>(pkr) );

    return pkr;
  }

  namespace Impl{

    class AddFieldAndValueToPrimaryKeyRecordFromStruct
    {
     public:

      AddFieldAndValueToPrimaryKeyRecordFromStruct(Mdt::Sql::PrimaryKeyRecord & pkr)
       : mPkr(pkr)
      {
      }

      template<typename Field, typename Value>
      void operator()(const Field &, const Value & value)
      {
        using Mdt::Sql::FieldName;

        mPkr.addValue( FieldName(QString::fromLatin1(Mdt::Reflection::fieldName<Field>())), value );
      }

     private:

      Mdt::Sql::PrimaryKeyRecord & mPkr;
    };

  } // namespace Impl{

  /*! \brief Get a SQL primary key record from a reflected struct
   *
   * \pre \a PrimaryKey must be primary key class for a reflected struct
   * \pre \a Struct must be a reflected struct associated with \a PrimaryKey
   */
  template<typename PrimaryKey, typename Struct>
  Mdt::Sql::PrimaryKeyRecord primaryKeyRecordFromReflected(const Struct & data)
  {
    static_assert( Mdt::Reflection::TypeTraits::IsPrimaryKeyClass<PrimaryKey>::value,
                   "PrimaryKey must be a primary key class for a reflected struct" );
    static_assert( Mdt::Reflection::TypeTraits::IsReflectedStructAssociatedWithPrimaryKey<Struct, PrimaryKey>::value,
                   "Struct must be a reflected struct associated with PrimaryKey" );

    Mdt::Sql::PrimaryKeyRecord pkr;
    Impl::AddFieldAndValueToPrimaryKeyRecordFromStruct f(pkr);

    Mdt::Reflection::forEachPrimaryKeyFieldAndValue<PrimaryKey>(data, f);
    Q_ASSERT( primaryKeyRecordHasCorrectFieldNameList<PrimaryKey>(pkr) );

    return pkr;
  }

}}} // namespace Mdt{ namespace Sql{ namespace Reflection{


#endif // #ifndef MDT_SQL_REFLECTION_PRIMARY_KEY_RECORD_ALGORITHM_H
