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
#ifndef MDT_ENTITY_TABLE_MODEL_RECORD_ADAPTER_H
#define MDT_ENTITY_TABLE_MODEL_RECORD_ADAPTER_H

#include "ValueAt.h"
#include <QtGlobal>
#include <QVariant>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/empty.hpp>

namespace Mdt{ namespace Entity{

  /*! \brief Class template that provides the requirements for Mdt::ItemModel::AbstractStlTableModel subclasses
   *
   * This implementation works for subclasses of DataTemplate .
   */
  template<typename Data>
  class TableModelRecordAdapter
  {
   public:

    /*! \brief STL compatible value type
     */
    using value_type = QVariant;

    /*! \brief Get the size of \a Data
     */
    static int containerSize(const Data &)
    {
      return boost::fusion::result_of::size<typename Data::data_struct_type>::type::value;
    }

    /*! \brief Check if \a Data is empty
     */
    static bool containerIsEmpty(const Data &)
    {
      return boost::fusion::result_of::empty<typename Data::data_struct_type>::type::value;
    }

    /*! \brief Get value at \a index in \a data
     *
     * \pre \a index must be in valid range ( 0 <= \a index < containerSize(data) )
     */
    static value_type valueAtIndex(const Data & data, int index)
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < containerSize(data));

      return valueAt(data.constDataStruct(), index);
    }

    /*! \brief Set \a value at \a index in \a data
     *
     * \pre \a index must be in valid range ( 0 <= \a index < containerSize(data) )
     */
    static void setValueAtIndex(Data & data, int index, const value_type & value)
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < containerSize(data));

      setValueAt(data.dataStruct(), index, value);
    }

    /*! \brief Return a initialized \a Data with \a count copies of \a value
     *
     * \pre \a count must be >= 1
     */
    static Data initializeContainer(int count, const value_type & value)
    {
      Q_ASSERT(count >= 1);

      Data data;
      const int size = containerSize(data);
      for(int i=0; i < size; ++i){
        setValueAtIndex(data, i, value);
      }
      return data;
    }

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_TABLE_MODEL_RECORD_ADAPTER_H
