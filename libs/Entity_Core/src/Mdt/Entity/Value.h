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
#ifndef MDT_ENTITY_VALUE_H
#define MDT_ENTITY_VALUE_H

#include "FieldAttributes.h"
#include "TypeTraits/IsEntityFieldDef.h"
#include "TypeTraits/IsEntity.h"
#include "TypeTraits/FieldDef.h"
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <type_traits>

// #include <QDebug>

namespace Mdt{ namespace Entity{

  /*! \brief Get the value for a field in a data struct
   */
  template<typename EntityDataStruct, typename EntityFieldDef>
  const auto & value(const EntityDataStruct & dataStruct)
  {
    static_assert( TypeTraits::IsEntityFieldDef<EntityFieldDef>::value , "EntityFieldDef must be a field definition" );

    return boost::fusion::at_key<EntityFieldDef>(dataStruct);
  }

  /*! \brief Set the value for a field in a data struct
   */
  template<typename EntityDataStruct, typename EntityFieldDef, typename Value>
  void setValue(EntityDataStruct & dataStruct, const Value & value)
  {
    static_assert( TypeTraits::IsEntityFieldDef<EntityFieldDef>::value , "EntityFieldDef must be a field definition" );

    boost::fusion::at_key<EntityFieldDef>(dataStruct) = value;
  }

  namespace Impl{

    /*! \internal Functor called from uniqueIdValue()
     */
    template<typename EntityDataStruct, typename UniqueId>
    struct GetUniqueIdIfPk
    {
      GetUniqueIdIfPk(const EntityDataStruct & dataStruct)
       : mDataStruct(dataStruct)
      {
      }

      template<typename EntityFieldDef>
      typename std::enable_if< TypeTraits::isIntegralPrimaryKey<EntityDataStruct, EntityFieldDef>(), void >::type
      operator()(const EntityFieldDef &) const
      {
        Q_ASSERT(!mIdWasFound);
        id = UniqueId( value<EntityDataStruct, EntityFieldDef>(mDataStruct) );
        mIdWasFound = true;
      }

      template<typename EntityFieldDef>
      typename std::enable_if< !TypeTraits::isIntegralPrimaryKey<EntityDataStruct, EntityFieldDef>(), void >::type
      operator()(const EntityFieldDef &) const
      {
      }

      mutable UniqueId id;

     private:

      mutable bool mIdWasFound = false;
      const EntityDataStruct & mDataStruct;
    };

  } // namespace Impl{

  /*! \brief Get the value of the unique id in \a data
   *
   * \tparam UniqueId value object that wraps \a id
   *    Must be callable like:
   *    \code
   *    UniqueId(id).value()
   *    \endcode
   *
   * \sa IntegralUniqueIdTemplate
   */
  template<typename EntityData, typename UniqueId>
  UniqueId uniqueIdValue(const EntityData & data)
  {
    static_assert( TypeTraits::IsEntity<typename EntityData::entity_template_type>::value, "EntityData must be based on a entity" );

    using EntityDef = typename EntityData::entity_def_type;
    using EntityDataStruct = typename EntityData::data_struct_type;

    Impl::GetUniqueIdIfPk<EntityDataStruct, UniqueId> op(data.constDataStruct());
    constexpr EntityDef entityDef;
    boost::fusion::for_each(entityDef, op);

    return op.id;
  }

  namespace Impl{

    /*! \internal Functor called from setUniqueIdValue()
     */
    template<typename EntityDataStruct, typename UniqueId>
    struct SetUniqueIdIfPk
    {
      SetUniqueIdIfPk(EntityDataStruct & dataStruct, UniqueId id)
       : mDataStruct(dataStruct),
         mId(id)
      {
      }

      template<typename EntityFieldDef>
      typename std::enable_if< TypeTraits::isIntegralPrimaryKey<EntityDataStruct, EntityFieldDef>(), void >::type
      operator()(const EntityFieldDef &) const
      {
        Q_ASSERT(!mIdWasFound);
        setValue<EntityDataStruct, EntityFieldDef>(mDataStruct, mId.value());
        mIdWasFound = true;
      }

      template<typename EntityFieldDef>
      typename std::enable_if< !TypeTraits::isIntegralPrimaryKey<EntityDataStruct, EntityFieldDef>(), void >::type
      operator()(const EntityFieldDef &) const
      {
      }

     private:

      mutable bool mIdWasFound = false;
      EntityDataStruct & mDataStruct;
      UniqueId mId;
    };

  } // namespace Impl{

  /*! \brief Set the value of the unique id in \a data
   *
   * \tparam UniqueId value object that wraps \a id
   *    Must be callable like:
   *    \code
   *    UniqueId(id)
   *    \endcode
   *
   * \sa IntegralUniqueIdTemplate
   */
  template<typename EntityData, typename UniqueId>
  void setUniqueIdValue(EntityData & data, UniqueId id)
  {
    using EntityDef = typename EntityData::entity_def_type;
    using EntityDataStruct = typename EntityData::data_struct_type;

    Impl::SetUniqueIdIfPk<EntityDataStruct, UniqueId> op(data.dataStruct(), id);
    constexpr EntityDef entityDef;
    boost::fusion::for_each(entityDef, op);
  }

  /*! \brief Check if a unique id is null
   *
   * Returns true if the value refered by EntityFieldDef
   *  in \a dataStruct is part of the primary key,
   *  is integral and is null.
   *
   * \tparam UniqueId value object that wraps \a id
   *    Must be callable like:
   *    \code
   *    UniqueId(id).isNull()
   *    \endcode
   *
   * \sa IntegralUniqueIdTemplate
   */
  template<typename EntityDataStruct, typename EntityFieldDef, typename UniqueId>
  typename std::enable_if< TypeTraits::isIntegralPrimaryKey<EntityDataStruct, EntityFieldDef>(), bool >::type
  isNullUniqueId(const EntityDataStruct & dataStruct) noexcept
  {
    static_assert( TypeTraits::IsEntityFieldDef<EntityFieldDef>::value , "EntityFieldDef must be a field definition" );

    return UniqueId( value<EntityDataStruct, EntityFieldDef>(dataStruct) ).isNull();
  }

  template<typename EntityDataStruct, typename EntityFieldDef, typename UniqueId>
  typename std::enable_if< !TypeTraits::isIntegralPrimaryKey<EntityDataStruct, EntityFieldDef>(), bool >::type
  isNullUniqueId(const EntityDataStruct &) noexcept
  {
    return false;
  }

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_VALUE_H
