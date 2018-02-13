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
#ifndef MDT_ENTITY_SQL_FIELD_H
#define MDT_ENTITY_SQL_FIELD_H

#include "Mdt/Entity/FieldAttributes.h"
#include "Mdt/Sql/Schema/Field.h"
#include "Mdt/Sql/Schema/FieldType.h"
#include "Mdt/Sql/Schema/FieldTypeMap.h"
#include "Mdt/Sql/Schema/FieldLength.h"
#include "MdtEntity_SqlExport.h"
#include <QString>
#include <QMetaType>
#include <QVariant>
#include <boost/fusion/include/at_key.hpp>
#include <type_traits>

namespace Mdt{ namespace Entity{

  /*! \brief Helper to reflect a entity fielf to a SQL field
   */
  class MDT_ENTITY_SQL_EXPORT SqlField
  {
   public:

    /*! \brief Get QMetaType from a entity field
     */
    template<typename EntityDataStruct, typename EntityFieldDef>
    static constexpr QMetaType::Type qmetaTypeFromEntityField() noexcept
    {
      using fieldTypeRaw = typename boost::fusion::result_of::at_key<EntityDataStruct, EntityFieldDef>::type;
      using fieldType = typename std::remove_reference_t<fieldTypeRaw>;

      return static_cast<QMetaType::Type>( QVariant(fieldType{}).type() );
    }

    /*! \brief Get SQL field type from a entity field
     */
    template<typename EntityDataStruct, typename EntityFieldDef>
    static constexpr Mdt::Sql::Schema::FieldType sqlFieldTypeFromEntityField(const Mdt::Sql::Schema::FieldTypeMap & fieldTypeMap) noexcept
    {
      const auto fieldLength = Mdt::Sql::Schema::FieldLength( EntityFieldDef::fieldAttributes().maxLength() );

      return fieldTypeMap.fieldTypeFromQMetaType( qmetaTypeFromEntityField<EntityDataStruct, EntityFieldDef>(), fieldLength );
    }

    /*! \brief Create a SQL field from a entity field
     *
     * \note For fields that are part of the primary key,
     *   the unique and required flags are not set here.
     *   They are later handled by Mdt::Sql::Schema::Table .
     */
    template<typename EntityDataStruct, typename EntityFieldDef>
    static Mdt::Sql::Schema::Field fromEntityField(const Mdt::Sql::Schema::FieldTypeMap & fieldTypeMap)
    {
      Mdt::Sql::Schema::Field sqlField;
      const FieldAttributes fieldAttributes = EntityFieldDef::fieldAttributes();

      sqlField.setType( sqlFieldTypeFromEntityField<EntityDataStruct, EntityFieldDef>(fieldTypeMap) );
      sqlField.setName( EntityFieldDef::fieldName() );
      sqlField.setRequired( fieldAttributes.isRequired() );
      sqlField.setUnique( fieldAttributes.isUnique() );
      sqlField.setLength( fieldAttributes.maxLength() );
      if(fieldAttributes.hasDefaultValue()){
        sqlField.setDefaultValue( boost::fusion::at_key<EntityFieldDef>(EntityDataStruct{}) );
      }

      return sqlField;
    }

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_SQL_FIELD_H
