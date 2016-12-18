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
#ifndef MDT_ITEM_MODEL_EXPRESSION_COLUMN_TERMINAL_H
#define MDT_ITEM_MODEL_EXPRESSION_COLUMN_TERMINAL_H

#include <QtGlobal>
#include <boost/proto/expr.hpp>
#include <boost/proto/extends.hpp>
#include <boost/proto/operators.hpp>

namespace Mdt{ namespace ItemModel{ namespace Expression{

  /*! \brief Data container for ColumnTerminal
   */
  template<typename Tag>
  class ColumnData
  {
   public:

    /*! \brief Construct data
     *
     * \pre columnIndex must be >= 0
     */
    constexpr explicit ColumnData(int columnIndex) noexcept
     : mColumnIndex(columnIndex)
    {
      Q_ASSERT(mColumnIndex >= 0);
    }

    /*! \brief Get column index
     */
    constexpr int columnIndex() const noexcept
    {
      return mColumnIndex;
    }

   private:

    int mColumnIndex;
  };

  /*! \brief Terminal for filter expressions that acts on a item model
   */
  template<typename Tag, typename Domain = boost::proto::default_domain>
  struct ColumnTerminal : boost::proto::extends<
                                  boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<ColumnData<Tag> > >,
                                  ColumnTerminal< Tag, Domain >,
                                  Domain
                                >
  {
   private:

    typedef boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<ColumnData<Tag> > > terminal_type;
    typedef ColumnTerminal< Tag, Domain >                                                           ColumnTerminal_t;
    typedef boost::proto::extends< terminal_type, ColumnTerminal_t, Domain >                        base_type;

   public:

    typedef ColumnData<Tag>          value_type;
    typedef ColumnData<Tag> &        reference;
    typedef const ColumnData<Tag> &  const_reference;
    /*
     * This will define operator()= , which permit to give better error messages with grammars.
     * (Without this, error could look like 'error: no match for operator= ......'
     */
    BOOST_PROTO_EXTENDS_USING_ASSIGN(ColumnTerminal)

    /*! \brief Construct a terminal by defining column index
     */
    explicit ColumnTerminal(int columnIndex)
     : base_type(terminal_type::make( ColumnData<Tag>(columnIndex) ))
    {
    }

  };


}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{

#endif // #ifndef MDT_ITEM_MODEL_EXPRESSION_COLUMN_TERMINAL_H
