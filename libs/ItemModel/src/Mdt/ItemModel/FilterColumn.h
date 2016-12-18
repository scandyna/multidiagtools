/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_FILTER_COLUMN_H
#define MDT_ITEM_MODEL_FILTER_COLUMN_H

#include "Expression/ColumnTerminal.h"

#include <QtGlobal>
#include <boost/proto/expr.hpp>
#include <boost/proto/extends.hpp>
#include <boost/proto/operators.hpp>

namespace Mdt{ namespace ItemModel{

  /*! \brief Data container for FilterColumn
   */
//   class FilterColumnData
//   {
//    public:
// 
//     /*! \brief Construct data
//      *
//      * \pre columnIndex must be >= 0
//      */
//     constexpr explicit FilterColumnData(int columnIndex) noexcept
//      : mColumnIndex(columnIndex)
//     {
//       Q_ASSERT(mColumnIndex >= 0);
//     }
// 
//     /*! \brief Get column index
//      */
//     constexpr int columnIndex() const noexcept
//     {
//       return mColumnIndex;
//     }
// 
//    private:
// 
//     int mColumnIndex;
//   };

  /*! \brief Implementation for FilterColumn
   */
//   template<typename Domain = boost::proto::default_domain>
//   struct FilterColumnTerminal : boost::proto::extends<
//                                   boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<FilterColumnData> >,
//                                   FilterColumnTerminal< Domain >,
//                                   Domain
//                                 >
//   {
//    private:
// 
//     typedef boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<FilterColumnData> > terminal_type;
//     typedef FilterColumnTerminal< Domain >                                                                FilterColumnTerminal_t;
//     typedef boost::proto::extends< terminal_type, FilterColumnTerminal_t, Domain >                        base_type;
// 
//    public:
// 
//     typedef FilterColumnData          value_type;
//     typedef FilterColumnData &        reference;
//     typedef const FilterColumnData &  const_reference;
//     /*
//      * This will define operator()= , which permit to give better error messages with grammars.
//      * (Without this, error could look like 'error: no match for operator= ......'
//      */
//     BOOST_PROTO_EXTENDS_USING_ASSIGN(FilterColumnTerminal)
// 
//     /*! \brief Construct a terminal by defining column index
//      */
//     explicit FilterColumnTerminal(int columnIndex)
//      : base_type(terminal_type::make( FilterColumnData(columnIndex) ))
//     {
//     }
// 
//   };

  struct FilterColumnTag
  {
  };

  using FilterColumnData = Expression::ColumnData<FilterColumnTag>;

  /*! \brief FilterColumn is a (boost:proto) terminal that represents a column index in a item model, used for FilterExpression
   */
  using FilterColumn = Expression::ColumnTerminal<FilterColumnTag>;
//   using FilterColumn = FilterColumnTerminal<>;

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_FILTER_COLUMN_H
