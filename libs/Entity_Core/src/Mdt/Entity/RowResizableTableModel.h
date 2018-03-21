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
#ifndef MDT_ENTITY_ROW_RESIZABLE_TABLE_MODEL_H
#define MDT_ENTITY_ROW_RESIZABLE_TABLE_MODEL_H

#include "FieldAt.h"
#include "TableModelRecordAdapter.h"
#include "Mdt/ItemModel/RowResizableStlTableModelTemplate.h"
#include <type_traits>

namespace Mdt{ namespace Entity{

  /*! \brief Row resizable table model for STL compliant list of entity data
   *
   * \tparam DataList Type of a STL compatible container, able to work on BidirectionalIterator .
   *    To be usable, \a %DataList class must provide:
   *     - The cbegin() method
   *     - The cend() method
   *     - The begin() method
   *     - The end() method
   *     - A STL compatible insert() method
   *     - A STL compatible erase() method
   *     - value_type : a subclass of DataTemplate .
   *
   * Example for a entity value class based on DataTemplate :
   * \code
   * class MyEntityModel : public Mdt::Entity::RowResizableTableModel< std::vector<MyEntityData> >
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   using ParentClass = Mdt::Entity::RowResizableTableModel< std::vector<MyEntityData> >;
   *   using ParentClass::ParentClass;
   * };
   * \endcode
   *
   * It is also possible to use the DataList class template:
   * \code
   * using MyEntityDataList = Mdt::Entity::DataList<MyEntityData>;
   *
   * class MyEntityModel : public Mdt::Entity::RowResizableTableModel<MyEntityDataList>
   * {
   *  Q_OBJECT
   *
   *  public:
   *
   *   using ParentClass = Mdt::Entity::RowResizableTableModel<MyEntityDataList>;
   *   using ParentClass::ParentClass;
   * };
   * \endcode
   */
  template<typename DataList>
  class RowResizableTableModel : public Mdt::ItemModel::RowResizableStlTableModelTemplate< DataList, TableModelRecordAdapter<typename DataList::value_type> >
  {
   public:

    /*! \brief STL-style value
     */
    using value_type = typename DataList::value_type;

    using entity_def_type = typename value_type::entity_def_type;
    using data_struct_type = typename value_type::data_struct_type;

    using ParentClass = Mdt::ItemModel::RowResizableStlTableModelTemplate< DataList, TableModelRecordAdapter<typename DataList::value_type> >;
    using ParentClass::ParentClass;
    using ParentClass::headerData;

    /*! \brief Get entity definition
     */
    static const entity_def_type def()
    {
      return value_type::def();
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
      static const entity_def_type dataDef;
      Q_ASSERT(section >= 0);
      Q_ASSERT(section < fieldCount<entity_def_type>());

      return fieldNameAt(def(), section);
    }

    /*! \brief Get column count
     */
    int columnCount() const override
    {
      return fieldCount<entity_def_type>();
    }

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ROW_RESIZABLE_TABLE_MODEL_H
