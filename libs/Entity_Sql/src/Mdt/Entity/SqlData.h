/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ENTITY_SQL_DATA_H
#define MDT_ENTITY_SQL_DATA_H

// #include "Mdt/Entity/DataTemplate.h"
#include "Mdt/Entity/FieldAt.h"
#include "Mdt/Entity/TypeTraits/IsEntityFieldDef.h"
#include "MdtEntity_SqlExport.h"
#include <QSqlRecord>
#include <QVariant>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/sequence.hpp>
#include <type_traits>

// #include <QDebug>

namespace Mdt{ namespace Entity{

  namespace SqlDataImpl{

    template<typename EntityDataStruct>
    struct AddFieldData
    {
      AddFieldData(const QSqlRecord & sqlRecord)
       : mSqlRecord(sqlRecord)
      {
      }

      template<typename EntityFieldDef>
      void operator()(const EntityFieldDef &) const
      {
        static_assert( TypeTraits::IsEntityFieldDef<EntityFieldDef>::value, "EntityFieldDef must be a entity field definition type" );

        using fieldTypeRaw = typename boost::fusion::result_of::at_key<EntityDataStruct, EntityFieldDef>::type;
        using fieldType = typename std::remove_reference_t<fieldTypeRaw>;

        Q_ASSERT(mColumn < mSqlRecord.count());

        boost::fusion::at_key<EntityFieldDef>(mDataStruct) = mSqlRecord.value(mColumn).value<fieldType>();
        ++mColumn;
      }

      mutable EntityDataStruct mDataStruct;
      const QSqlRecord & mSqlRecord;
      mutable int mColumn = 0;
    };

  } // namespace SqlDataImpl{

  /*! \brief Helper class to convert SQL record to or from entity data
   */
  class MDT_ENTITY_SQL_EXPORT SqlData
  {
   public:

    /*! \brief Get entity data from \a record
     *
     * \note This method assumes that \a record reflects the exact
     *   layout of the entity data (count of fields and order of fields).
     * \pre field count in \a record must be equal to the field count of the entity data
     */
    template<typename EntityData>
    static EntityData entityDataFromQSqlRecord(const QSqlRecord & record)
    {
      using entity_def = typename EntityData::entity_def_type;
      using data_struct = typename EntityData::data_struct_type;

      Q_ASSERT( record.count() == fieldCount<entity_def>() );

      EntityData data;
      SqlDataImpl::AddFieldData<data_struct> addFieldData(record);
      boost::fusion::for_each(data.def(), addFieldData);
      data.setDataStruct(addFieldData.mDataStruct);
      
      return data;
    }
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_SQL_DATA_H
