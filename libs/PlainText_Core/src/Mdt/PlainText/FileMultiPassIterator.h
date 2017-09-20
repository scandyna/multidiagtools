/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_PLAIN_TEXT_FILE_MULTI_PASS_ITERATOR_H
#define MDT_PLAIN_TEXT_FILE_MULTI_PASS_ITERATOR_H

#include "FileInputIterator.h"
#include <boost/spirit/include/support_multi_pass.hpp>

namespace Mdt{ namespace PlainText{

  // Namespace to define FileMultiPassIterator without exposing boost::spirit
  namespace Impl{

    using namespace boost::spirit;
    using namespace boost::spirit::iterator_policies;

    using Policy = default_policy<first_owner, no_check, input_iterator, split_std_deque>;
    using MultiPassIterator = multi_pass<Mdt::PlainText::FileInputIterator, Policy>;

  } // namespace Impl{

  /*! \brief File parser multi pass iterator
   *
   * This iterator uses Boost.Spirt's multi pass API
   *  to map FileInputIterator (witch is a single pass input iterator)
   *  to a multi pass iterator, that is required by Spirit parsers.
   */
  using FileMultiPassIterator = Impl::MultiPassIterator;

  /*! \brief Make a FileMultiPassIterator
   */
  inline
  FileMultiPassIterator makeFileMultiPassIterator(FileInputIterator & it)
  {
    return boost::spirit::make_multi_pass<Impl::Policy, FileInputIterator>(it);
  }

  /*! \brief Make a const FileMultiPassIterator
   */
  inline
  FileMultiPassIterator makeFileMultiPassIterator(const FileInputIterator & it)
  {
    return boost::spirit::make_multi_pass<Impl::Policy, FileInputIterator>(it);
  }

//   /*! \brief Make a EOF FileMultiPassIterator
//    */
//   template<typename InputIterator>
//   inline
//   FileMultiPassIterator makeFileMultiPassIterator()
//   {
//     return boost::spirit::make_multi_pass<Impl::Policy, InputIterator>(InputIterator());
//   }

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_TEXT_FILE_MULTI_PASS_ITERATOR_H
