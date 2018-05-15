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
#ifndef MDT_ENTITY_ABSTRACT_CACHED_ENTITY_REPOSITORY_H
#define MDT_ENTITY_ABSTRACT_CACHED_ENTITY_REPOSITORY_H

#include "AbstractCachedRepository.h"
#include "FieldAt.h"
#include "ValueAt.h"
#include "TypeTraits/IsEntityDef.h"

namespace Mdt{ namespace Entity{

  /*! \brief Base class to create a cached repository
   *
   * This is similar to AbstractCachedRepository,
   *  but reuses attributes defined with MDT_ENTITY_DEF() .
   *
   * \code
   * class AbstractPersonRepository : public Mdt::Entity::AbstractCachedEntityRepository<PersonData>
   * {
   *  public:
   *
   *   bool fetchRecords(int count) override;
   * };
   * \endcode
   */
  template<typename EntityData>
  class AbstractCachedEntityRepository : public AbstractCachedRepository<EntityData>
  {
   public:

    using entity_data_type = EntityData;
    using entity_def_type = typename entity_data_type::entity_def_type;
    using data_struct_type = typename entity_data_type::data_struct_type;

    static_assert( TypeTraits::IsEntityDef<entity_def_type>::value, "EntityData must have a entity definition" );

    using ParentClass = AbstractCachedRepository<EntityData>;
    using ParentClass::rowCount;
    using ParentClass::constRecordAt;
    using ParentClass::recordAt;

    /*! \brief Get count of columns
     */
    int columnCount() const override
    {
      return fieldCount<entity_def_type>();
    }

    /*! \brief Get data at \a row and \a column
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    QVariant data(int row, int column) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());

      return valueAt( constRecordAt(row).constDataStruct(), column );
    }

   private:

    /*! \brief Set \a data at \a row and \a column
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() ).
     * \pre \a column must be in valid range ( 0 <= \a column < columnCount() ).
     */
    void setDataToCache(int row, int column, const QVariant & data) override
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      Q_ASSERT(column >= 0);
      Q_ASSERT(column < columnCount());

      setValueAt( recordAt(row).dataStruct(), column, data );
    }

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_CACHED_ENTITY_REPOSITORY_H
