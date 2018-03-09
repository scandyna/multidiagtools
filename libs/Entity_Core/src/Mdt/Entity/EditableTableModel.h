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
#ifndef MDT_ENTITY_EDITABLE_TABLE_MODEL_H
#define MDT_ENTITY_EDITABLE_TABLE_MODEL_H

#include "FieldAt.h"
#include "TableModelRecordAdapter.h"
#include "TypeTraits/IsEntityDef.h"
#include "Mdt/ItemModel/EditableStlTableModel.h"
#include <type_traits>

namespace Mdt{ namespace Entity{

  /*! \brief Editable table model for STL compliant list of entity data
   *
   * \tparam DataList Type of a STL compatible container, able to work on BidirectionalIterator .
   *    To be usable, \a DataList class must provide:
   *     - The cbegin() method
   *     - The cend() method
   *     - The begin() method
   *     - The end() method
   *     - value_type : a subclass of DataTemplate .
   *
   * Example for a entity value class based on DataTemplate :
   * \code
   * class MyEntityModel : public Mdt::Entity::EditableTableModel< std::vector<MyEntityData> >
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   using ParentClass = Mdt::Entity::EditableTableModel< std::vector<MyEntityData> >;
   *   using ParentClass::ParentClass;
   * };
   * \endcode
   */
  template<typename DataList>
  class EditableTableModel : public Mdt::ItemModel::EditableStlTableModel< DataList, TableModelRecordAdapter<typename DataList::value_type> >
  {
   public:

    using entity_def_type = typename DataList::value_type::entity_def_type;
    using data_struct_type = typename DataList::value_type::data_struct_type;

    using ParentClass = Mdt::ItemModel::EditableStlTableModel< DataList, TableModelRecordAdapter<typename DataList::value_type> >;
    using ParentClass::ParentClass;
    using ParentClass::headerData;

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
      static const entity_def_type dataDef;
      Q_ASSERT(section >= 0);
      Q_ASSERT(section < fieldCount(dataDef));

      return fieldNameAt(dataDef, section);
    }
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_EDITABLE_TABLE_MODEL_H
