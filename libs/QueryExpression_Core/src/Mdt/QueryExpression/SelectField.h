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
#ifndef MDT_QUERY_EXPRESSION_SELECT_FIELD_H
#define MDT_QUERY_EXPRESSION_SELECT_FIELD_H

#include "FieldName.h"
#include "SelectEntity.h"
#include "EntityAndField.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>
#include <boost/variant.hpp>
#include <boost/proto/expr.hpp>
#include <boost/proto/extends.hpp>
#include <boost/proto/operators.hpp>

namespace Mdt{ namespace QueryExpression{

  /*! \internal Tag for SelectField
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT SelectAllField
  {
  };

  /*! \internal Tag for SelectField
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT SelectFieldTag
  {
  };

  /*! \internal Data variant for SelectField
   */
  using SelectFieldVariant = boost::variant<SelectAllField, EntityAndField>;

  /*! \brief Represents a field and the optional field alias and maybe a SelectEntity
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT SelectField : boost::proto::extends<
                                                        boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<SelectFieldVariant> >,
                                                        SelectField,
                                                        boost::proto::default_domain
                                                      >
  {
   private:

    using Domain = boost::proto::default_domain;
    using Expression = boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<SelectFieldVariant> >;
    using ParentClass = boost::proto::extends< Expression, SelectField, Domain >;

   public:

    using value_type = SelectFieldVariant;
    using reference = value_type &;
    using const_reference = const value_type &;

    /*! \brief Construct a select all field
     */
    SelectField(SelectAllField);

    /*! \brief Construct a select field with a field name and a optional field alias
     *
     * \pre \a fieldName must not be null
     */
    SelectField(const FieldName & fieldName, const QString & fieldAlias = QString());

    /*! \brief Construct a select field with a select entity, a field name and a optional field alias
     *
     * \pre \a entity must not be null
     * \pre \a fieldName must not be null
     */
    SelectField(const SelectEntity & entity, const FieldName & fieldName, const QString & fieldAlias = QString());

    /*! \brief Copy construct a select field from \a other
     */
    SelectField(const SelectField & other) = default;

    /*! \brief Copy assign \a other to this select field
     */
    SelectField & operator=(const SelectField & other) = default;

    /*! \brief Move construct a select field from \a other
     */
    SelectField(SelectField && other) = default;

    /*! \brief Move assign \a other to this select field
     */
    SelectField & operator=(SelectField && other) = default;

    /*! \internal Access internal variant of this select field
     */
    const SelectFieldVariant & internalVariant() const
    {
      return boost::proto::value(*this);
    }
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SELECT_FIELD_H
