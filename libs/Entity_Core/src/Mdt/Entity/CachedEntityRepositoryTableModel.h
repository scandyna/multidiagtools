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
#ifndef MDT_ENTITY_CACHED_ENTITY_REPOSITORY_TABLE_MODEL_H
#define MDT_ENTITY_CACHED_ENTITY_REPOSITORY_TABLE_MODEL_H

#include "CachedRepositoryTableModel.h"
#include "FieldAt.h"
#include "TypeTraits/IsEntityFieldDef.h"

namespace Mdt{ namespace Entity{

  /*! \brief Table model that can act directly on a repository based on AbstractCachedEntityRepository
   *
   * Use CachedEntityRepositoryTableModel to provide access to 1 entity:
   * \code
   * class EditPersonTableModel : public Mdt::Entity::CachedEntityRepositoryTableModel<AbstractPersonRepository>
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   using ParentClass = Mdt::Entity::CachedEntityRepositoryTableModel<AbstractPersonRepository>;
   *   using ParentClass::ParentClass;
   * };
   * \endcode
   *
   * The model can be used like this in the application:
   * \code
   * EditPersonTableModel model;
   * auto personRepository = PersonRepository::make<SqlPersonRepository>();
   * model.setRepository(personRepository);
   * \endcode
   */
  template<typename EntityRepositoryInterface>
  class CachedEntityRepositoryTableModel : public CachedRepositoryTableModel<EntityRepositoryInterface>
  {
    using ParentClass = CachedRepositoryTableModel<EntityRepositoryInterface>;

   public:

    using entity_data_type = typename EntityRepositoryInterface::entity_data_type;
    using entity_def_type = typename entity_data_type::entity_def_type;
    using data_struct_type = typename entity_data_type::data_struct_type;

    using ParentClass::ParentClass;

    /*! \brief Get entity definition
     */
    static const entity_def_type def()
    {
      return entity_def_type{};
    }

    /*! \brief Get the field index of \a fieldDef
     */
    template<typename FieldDef>
    static int fieldIndex(FieldDef fieldDef) noexcept
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      return entity_data_type::fieldIndex(fieldDef);
    }

    /*! \brief Get header data for \a section, \a orientation and \a role
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
      if(orientation != Qt::Horizontal){
        return ParentClass::headerData(section, orientation, role);
      }
      if(role != Qt::DisplayRole){
        return ParentClass::headerData(section, orientation, role);
      }
      Q_ASSERT(section >= 0);
      Q_ASSERT(section < fieldCount( def() ));

      return fieldNameAt(def(), section);
    }

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_CACHED_ENTITY_REPOSITORY_TABLE_MODEL_H
