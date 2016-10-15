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
#ifndef MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_TABLE_FIELD_TERMINAL_H
#define MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_TABLE_FIELD_TERMINAL_H

#include "Mdt/Sql/TableName.h"
#include "Mdt/Sql/FieldName.h"
#include <QString>
#include <boost/proto/extends.hpp>
#include <boost/proto/operators.hpp>

namespace Mdt{ namespace Sql{ namespace Expression{ namespace JoinConstraint{

  /*! \brief Wrapper for a terminal using table name and field name
   *
   * \tparam Tag Type to make target terminal a unique type
   */
  template<typename Tag, typename Domain = boost::proto::default_domain>
  struct TableFieldTerminal : boost::proto::extends<
                                      boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<Tag> >,
                                      TableFieldTerminal< Tag, Domain >,
                                      Domain
                                    >
  {
   private:

    typedef boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<Tag> > terminal_type;
    typedef TableFieldTerminal< Tag, Domain >                                                TableFieldTerminal_t;
    typedef boost::proto::extends< terminal_type, TableFieldTerminal_t, Domain >             base_type;

   public:

    typedef typename boost::proto::result_of::value<terminal_type>::type          value_type;
    typedef typename boost::proto::result_of::value<terminal_type &>::type        reference;
    typedef typename boost::proto::result_of::value<const terminal_type &>::type  const_reference;

    /*
     * This will define operator()= , which permit to give better error messages with grammars.
     * (Without this, error could look like 'error: no match for operator= ......'
     */
    BOOST_PROTO_EXTENDS_USING_ASSIGN(TableFieldTerminal)

    /*! \brief Construct a terminal by defining table name and field name
     */
    TableFieldTerminal(const TableName & tn, const FieldName & fn)
    : base_type(terminal_type::make(Tag())),
      mTableName(tn.toString()),
      mFieldName(fn.toString())
    {
    }

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return mTableName;
    }

    /*! \brief Get field name
     */
    QString fieldName() const
    {
      return mFieldName;
    }

   private:

    QString mTableName;
    QString mFieldName;
  };

}}}} // namespace Mdt{ namespace Sql{ namespace Expression{ namespace JoinConstraint{

#endif // #ifndef MDT_SQL_EXPRESSION_JOIN_CONSTRAINT_TABLE_FIELD_TERMINAL_H
