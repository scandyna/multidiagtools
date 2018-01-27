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
#ifndef MDT_ENTITY_DATA_RECORD_ITERATOR_H
#define MDT_ENTITY_DATA_RECORD_ITERATOR_H

#include "ValueAt.h"
#include "MdtEntity_CoreExport.h"
#include <QVariant>
// #include <cstddef>  // For std::ptrdiff_t

namespace Mdt{ namespace Entity{

  /*! \brief Runtime iterator for a Boost Fusion Sequence
   *
   * \todo Obselete (not properly implementable)
   *
   * \tparam FusionSequence A Boost Fusion Sequence that can be accessed with boost::fusion::at_c() .
   *    It is required that each member of the sequence can be handled by QVariant.
   *    See also the QMetaType and Q_DECLARE_METATYPE() documentation.
   */
  template<typename FusionSequence>
  class MDT_ENTITY_CORE_EXPORT DataRecordIterator
  {
   public:

    /*! \brief STL style value type
     */
    using value_type = QVariant;

    /*! \todo Define difference_type
     */
    //typedef std::ptrdiff_t difference_type;

    /*! \brief STL style reference
     */
    using reference = value_type &;

    /*! \brief STL style pointer
     */
    using pointer = value_type *;

    /*! \brief Construct a iterator on \a sequence
     */
    DataRecordIterator(FusionSequence & sequence)
    {
    }

    /*! \brief Copy construct a iterator from \a other
     */
    DataRecordIterator(const DataRecordIterator & other) = default;

    /*! \brief Copy assign \a other to this iterator
     */
    DataRecordIterator & operator=(const DataRecordIterator & other) = default;

    /*! \brief Increment this iterator (pre-increment)
     */
    DataRecordIterator & operator++()
    {
    }

    /*! \brief Increment this iterator (post-increment)
     *
     * \sa operator++()
     */
    DataRecordIterator operator++(int)
    {
      DataRecordIterator tmp(*this);
      ++*this;

      return tmp;
    }

    /*! \brief Dereference this iterator
     */
    reference operator*()
    {
    }

    /*! \brief Dereference this iterator
     */
    const reference operator*() const
    {
    }

    /*! \brief
     */
    friend
    bool operator==(const DataRecordIterator & a, const DataRecordIterator & b)
    {
    }

    /*! \brief
     */
    friend
    bool operator!=(const DataRecordIterator & a, const DataRecordIterator & b)
    {
      return !(a == b);
    }

  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_DATA_RECORD_ITERATOR_H
