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
#ifndef MDT_ENTITY_VALUE_AT_H
#define MDT_ENTITY_VALUE_AT_H

#include <QVariant>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/at.hpp>
#include <initializer_list>
#include <utility>
#include <type_traits>

/*
 * Techniques to access a Boost Fusion Sequence with a runtime index was inspirated from:
 * - https://codereview.stackexchange.com/questions/51407/stdtuple-foreach-implementation
 * - https://www.reddit.com/r/cpp/comments/6vyqra/variadic_switch_case/
 */

namespace Mdt{ namespace Entity{

  namespace Impl{

    template<typename FusionSequence, int I>
    void setValueAtIndex(FusionSequence & sequence, const QVariant & value)
    {
      using ref_type = typename boost::fusion::result_of::at_c<FusionSequence, I>::type;
      using type = typename std::remove_reference<ref_type>::type;

      boost::fusion::at_c<I>(sequence) = value.value<type>();
    }

    template<typename FusionSequence, int... Is>
    void setValueAt(FusionSequence & sequence, int index, const QVariant & value, std::integer_sequence<int, Is...>)
    {
      (void)std::initializer_list<int> {(index == Is ? ( setValueAtIndex<FusionSequence, Is>(sequence, value) ),0 : 0)...};
    }

    template<typename FusionSequence, int I>
    QVariant valueAtIndex(const FusionSequence & sequence)
    {
      return boost::fusion::at_c<I>(sequence);
    }

    template<typename FusionSequence, int... Is>
    QVariant valueAt(const FusionSequence & sequence, int index, std::integer_sequence<int, Is...>)
    {
      QVariant value;
      (void)std::initializer_list<int> {(index == Is ? (value = valueAtIndex<FusionSequence, Is>(sequence)),0 : 0)...};
      return value;
    }

  } // namespace Impl{

  /*! \brief Set \a value in \a sequence at \a index
   *
   * This function can be used to set the value of a Boost Fusion Sequence
   *  while the index is only known at runtime.
   *
   * \tparam FusionSequence A Boost Fusion Sequence that can be accessed with boost::fusion::at_c() .
   * \pre \a index must be in valid range ( 0 <= \a index < boost::fusion::result_of::size() ).
   * \note This only works if all types in \a sequence can be handled by QVariant.
   *    See the QMetaType and Q_DECLARE_METATYPE() documentation.
   * \note If the index is known at compile time, boost::fusion::at_c() should be used.
   * \note To iterate all members of \a sequence , use boost::fusion::for_each().
   */
  template<typename FusionSequence>
  void setValueAt(FusionSequence & sequence, int index, const QVariant & value)
  {
    constexpr int size = boost::fusion::result_of::size<FusionSequence>::type::value;

    Q_ASSERT(index >= 0);
    Q_ASSERT(index < size);

    Impl::setValueAt(sequence, index, value, std::make_integer_sequence<int, size>{});
  }

  /*! \brief Get the value in \a sequence at \a index
   *
   * This function can be used to get the value of a Boost Fusion Sequence
   *  while the index is only known at runtime.
   *
   * \tparam FusionSequence A Boost Fusion Sequence that can be accessed with boost::fusion::at_c() .
   * \pre \a index must be in valid range ( 0 <= \a index < boost::fusion::result_of::size() ).
   * \note This only works if all types in \a sequence can be handled by QVariant.
   *    See the QMetaType and Q_DECLARE_METATYPE() documentation.
   * \note If the index is known at compile time, boost::fusion::at_c() should be used.
   * \note To iterate all members of \a sequence , use boost::fusion::for_each().
   */
  template<typename FusionSequence>
  QVariant valueAt(const FusionSequence & sequence, int index)
  {
    constexpr int size = boost::fusion::result_of::size<FusionSequence>::type::value;

    Q_ASSERT(index >= 0);
    Q_ASSERT(index < size);

    return Impl::valueAt(sequence, index, std::make_integer_sequence<int, size>{});
  }

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_VALUE_AT_H
