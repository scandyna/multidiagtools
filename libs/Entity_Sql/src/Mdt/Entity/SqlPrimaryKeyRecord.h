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
#ifndef MDT_ENTITY_SQL_PRIMARY_KEY_RECORD_H
#define MDT_ENTITY_SQL_PRIMARY_KEY_RECORD_H

// #include "SqlPrimaryKey.h"
#include "Mdt/Entity/PrimaryKey.h"
#include "Mdt/Entity/ValueAt.h"
#include "Mdt/Sql/PrimaryKeyRecord.h"
#include "MdtEntity_SqlExport.h"

namespace Mdt{ namespace Entity{

  /*! \brief Helper to get a SQL primary record key from a entity
   */
  class MDT_ENTITY_SQL_EXPORT SqlPrimaryKeyRecord
  {
   public:

    /*! \brief Get a SQL primary key record from entity data
     */
    template<typename EntityData>
    static Mdt::Sql::PrimaryKeyRecord fromEntityData(const EntityData & data)
    {
      using entity_type = typename EntityData::entity_template_type;
      using entity_def_type = typename EntityData::entity_def_type;

      Mdt::Sql::PrimaryKeyRecord sqlPkRecord;

      const auto entityPk = PrimaryKey::fromEntity<entity_type>();
      for(int i = 0; i < entityPk.fieldCount(); ++i){
        const auto field = entityPk.fieldAt(i);
        const auto fieldName = Mdt::Sql::FieldName( field.template fieldName<entity_def_type>() );
        const auto value = valueAt( data.constDataStruct(), field.fieldIndex() );
        sqlPkRecord.addValue(fieldName, value);
      }

      return sqlPkRecord;
    }
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_SQL_PRIMARY_KEY_RECORD_H
