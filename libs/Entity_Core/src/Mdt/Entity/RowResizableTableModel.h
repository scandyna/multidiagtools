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
#include "MdtEntity_CoreExport.h"
#include <type_traits>

namespace Mdt{ namespace Entity{

  /*! \brief Row resizable table model for STL compliant list of entity data
   *
   * \tparam DataList Type of a STL compatible container, able to work on BidirectionalIterator .
   *    To be usable, \a DataList class must provide:
   *     - The cbegin() method
   *     - The cend() method
   *     - The begin() method
   *     - The end() method
   *     - A STL compatible insert() method
   *     - A STL compatible erase() method
   *     - value_type : a subclass of DataTemplate .
   */
  template<typename DataList>
  class MDT_ENTITY_CORE_EXPORT RowResizableTableModel : public Mdt::ItemModel::RowResizableStlTableModelTemplate< DataList, TableModelRecordAdapter<typename DataList::value_type> >
  {
   public:

    using def_type = typename DataList::value_type::def_type;
    using data_struct_type = typename DataList::value_type::data_struct_type;

    using ParentClass = Mdt::ItemModel::RowResizableStlTableModelTemplate< DataList, TableModelRecordAdapter<typename DataList::value_type> >;
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
      static const def_type dataDef;
      Q_ASSERT(section >= 0);
      Q_ASSERT(section < fieldCount<def_type>());

      return fieldNameAt(dataDef, section);
    }

    /*! \brief Get column count
     */
    int columnCount() const override
    {
      return fieldCount<def_type>();
    }

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ROW_RESIZABLE_TABLE_MODEL_H
