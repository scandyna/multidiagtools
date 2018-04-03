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
#ifndef MDT_ENTITY_READ_ONLY_TABLE_MODEL_H
#define MDT_ENTITY_READ_ONLY_TABLE_MODEL_H

#include "FieldAt.h"
#include "TableModelRecordAdapter.h"
#include "TypeTraits/IsEntityFieldDef.h"
#include "Mdt/ItemModel/ReadOnlyStlTableModel.h"
#include <type_traits>

namespace Mdt{ namespace Entity{

  /*! \brief Read only table model for STL compliant list of entity data
   *
   * \tparam %DataList Type of a STL compatible container, able to work on BidirectionalIterator .
   *    To be usable, \a %DataList class must provide:
   *     - The cbegin() method
   *     - The cend() method
   *     - value_type : a subclass of DataTemplate .
   *
   * Example for a entity value class based on DataTemplate :
   * \code
   * class MyEntityModel : public Mdt::Entity::ReadOnlyTableModel< std::vector<MyEntityData> >
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   using ParentClass = Mdt::Entity::ReadOnlyTableModel< std::vector<MyEntityData> >;
   *   using ParentClass::ParentClass;
   * };
   * \endcode
   */
  template<typename DataList>
  class ReadOnlyTableModel : public Mdt::ItemModel::ReadOnlyStlTableModel< DataList, TableModelRecordAdapter<typename DataList::value_type> >
  {
   public:

    /*! \brief STL-style value
     */
    using value_type = typename DataList::value_type;

    using entity_def_type = typename value_type::entity_def_type;
    using data_struct_type = typename value_type::data_struct_type;

    using ParentClass = Mdt::ItemModel::ReadOnlyStlTableModel< DataList, TableModelRecordAdapter<typename DataList::value_type> >;
    using ParentClass::ParentClass;
    using ParentClass::headerData;

    /*! \brief Get entity definition
     */
    static const entity_def_type def()
    {
      return value_type::def();
    }

    /*! \brief Get the field index of \a fieldDef
     */
    template<typename FieldDef>
    static int fieldIndex(FieldDef fieldDef) noexcept
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      return value_type::fieldIndex(fieldDef);
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

#endif // #ifndef MDT_ENTITY_READ_ONLY_TABLE_MODEL_H
