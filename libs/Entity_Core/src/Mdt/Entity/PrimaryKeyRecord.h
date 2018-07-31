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
#ifndef MDT_ENTITY_PRIMARY_KEY_RECORD_H
#define MDT_ENTITY_PRIMARY_KEY_RECORD_H

#include "PrimaryKeyRecordField.h"
#include "FieldAttributes.h"
#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityDef.h"
#include "MdtEntity_CoreExport.h"
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <vector>

namespace Mdt{ namespace Entity{

  namespace Impl{

    /*! \internal Functor called by PrimaryKeyRecord::fromEntityData()
     */
    template<typename EntityDataStruct>
    class AddFieldToPrimaryKeyRecordIf;

  } // namespace Impl{

  /*! \brief A primary key with its values
   *
   * \code
   * using namespace Mdt::Entity;
   *
   * PersonData person; // PersonData inherity DataTemplate
   * const auto personPk = PrimaryKeyRecord::fromEntityData(person);
   * \endcode
   */
  class MDT_ENTITY_CORE_EXPORT PrimaryKeyRecord
  {
   public:

    /*! \brief Get count of fields in this primary key record
     */
    int fieldCount() const noexcept
    {
      return mFields.size();
    }

    /*! \brief Check if this primary key record is null
     */
    bool isNull() const noexcept
    {
      return fieldCount() < 1;
    }

    /*!\brief Add a field to this primary key
     */
    void addField(const PrimaryKeyRecordField & field)
    {
      mFields.emplace_back(field);
    }

    /*! \brief Get field at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < fieldCount() ).
     */
    const PrimaryKeyRecordField & fieldAt(int index) const noexcept
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < fieldCount());

      return mFields[index];
    }

    /*! \brief Get a primary key record from \a data
     */
    template<typename EntityData>
    static PrimaryKeyRecord fromEntityData(const EntityData & data)
    {
      Q_UNUSED(data);

      PrimaryKeyRecord pkRecord;
      Impl::AddFieldToPrimaryKeyRecordIf<typename EntityData::data_struct_type> op(pkRecord, data.constDataStruct());
      constexpr typename EntityData::entity_def_type def;

      boost::fusion::for_each(def, op);

      return pkRecord;
    }

   private:

    std::vector<PrimaryKeyRecordField> mFields;
  };

  namespace Impl{

    /*! \internal Functor called by PrimaryKeyRecord::fromEntityData()
     */
    template<typename EntityDataStruct>
    class AddFieldToPrimaryKeyRecordIf
    {
     public:

      AddFieldToPrimaryKeyRecordIf(PrimaryKeyRecord & pkRec, const EntityDataStruct & dataStruct)
       : mPrimaryKeyRecord(pkRec),
         mDataStruct(dataStruct)
      {
      }

      template<typename EntityFieldDef>
      void operator()(const EntityFieldDef & entityFieldDef) const
      {
        if( entityFieldDef.fieldAttributes().isPrimaryKey() ){
          const auto value = boost::fusion::at_key<EntityFieldDef>(mDataStruct);
          mPrimaryKeyRecord.addField( PrimaryKeyRecordField(mCurrentFieldIndex, value) );
        }
        ++mCurrentFieldIndex;
      }
 
     private:

      PrimaryKeyRecord & mPrimaryKeyRecord;
      mutable int mCurrentFieldIndex = 0;
      const EntityDataStruct & mDataStruct;
    };

  } // namespace Impl{

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_PRIMARY_KEY_RECORD_H
