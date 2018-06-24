/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_QUERY_EXPRESSION_ENTITY_AND_FIELD_TERMINAL_H
#define MDT_QUERY_EXPRESSION_ENTITY_AND_FIELD_TERMINAL_H

#include "EntityName.h"
#include "FieldName.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>
#include <boost/proto/expr.hpp>
#include <boost/proto/extends.hpp>
#include <boost/proto/operators.hpp>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Tag to differentiate EntityAndFieldTerminal
   *
   * When using default defined Comparison grammar
   *  and ComparisonSqlTransform transform,
   *  this tag must be used.
   *
   * To use a other tag, grammar and transform
   *  must be completely redefined.
   */
  struct DefaultEntityAndFieldTag
  {
  };

  /*! \brief EntityAndField holds a entity name and a field name
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT EntityAndField
  {
   public:

    /*! \brief Construct a table/field by defining table name and field name
     */
    EntityAndField(const EntityName & en, const FieldName & fn)
    : mEntityName(en.toString()),
      mFieldName(fn.toString())
    {
    }

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return mEntityName;
    }

    /*! \brief Get field name
     */
    QString fieldName() const
    {
      return mFieldName;
    }

   private:

    QString mEntityName;
    QString mFieldName;
  };

  /*! \brief EntityAndFieldTerminal is a terminal using entity name and field name
   *
   * \tparam Tag Type to make target terminal a unique type
   */
  template<typename Tag = DefaultEntityAndFieldTag, typename Domain = boost::proto::default_domain>
  struct EntityAndFieldTerminal : boost::proto::extends<
                                      boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<EntityAndField> >,
                                      EntityAndFieldTerminal< Tag, Domain >,
                                      Domain
                                    >
  {
   private:

    using terminal_type = boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<EntityAndField> >;
    using TableFieldTerminal_t = EntityAndFieldTerminal< Tag, Domain >;
    using base_type = boost::proto::extends< terminal_type, TableFieldTerminal_t, Domain >;

   public:

    using value_type = EntityAndField;
    using reference = value_type &;
    using const_reference = const value_type &;

    /*
     * This will define operator()= , which permit to give better error messages with grammars.
     * (Without this, error could look like 'error: no match for operator= ......')
     */
    BOOST_PROTO_EXTENDS_USING_ASSIGN(EntityAndFieldTerminal)

    /*! \brief Construct a terminal by defining entity name and field name
     */
    EntityAndFieldTerminal(const EntityName & en, const FieldName & fn)
     : base_type(terminal_type::make( EntityAndField(en, fn) ))
    {
    }
  };

  /*! \brief EntityAndField is a terminal of a query expression
   */
  

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_ENTITY_AND_FIELD_TERMINAL_H
