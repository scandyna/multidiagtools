/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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

#include "FieldAlias.h"
#include "QueryEntity.h"
#include "EntityAndField.h"
#include "SelectAllField.h"
#include "SelectFieldVariant.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>
#include <boost/variant.hpp>

namespace Mdt{ namespace QueryExpression{

  class QueryField;

  /*! \internal Tag for SelectField
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT SelectFieldTag
  {
  };

  /*! \internal Represents a field in a select statement
   *
   * A select field can be represented by specifying simply the field name:
   * \code
   * SelectField name("name");
   * \endcode
   *
   * a field alias can also be provided:
   * \code
   * SelectField name("name", FieldAlias("N"));
   * \endcode
   *
   * for select statement that refers to more than 1 entity,
   *  the entity the field refers to should be specified:
   * \code
   * QueryEntity person("Person");
   *
   * SelectField personName(person, "name");
   * \endcode
   *
   * A select field can also represent all fields (the * in SQL):
   * \code
   * SelectField allField(SelectAllField{});
   * \endcode
   *
   * the entity can also be specified:
   * \code
   * QueryEntity person("Person");
   *
   * SelectField personAllFields(SelectAllField{person});
   * \endcode
   *
   * A select field can also be constructed from a QueryField.
   */
  struct MDT_QUERYEXPRESSION_CORE_EXPORT SelectField
  {
   public:

    using value_type = SelectFieldVariant;
    using reference = value_type &;
    using const_reference = const value_type &;

    /*! \brief Construct a null select field
     */
    SelectField() noexcept = default;

    /*! \brief Construct a select all field
     */
    SelectField(const SelectAllField & field);

    /*! \brief Construct a select field with a field name and a optional field alias
     *
     * \pre \a fieldName must not be empty
     */
    SelectField(const QString & fieldName, const FieldAlias & fieldAlias = FieldAlias());

    /*! \brief Construct a select field with a query entity, a field name and a optional field alias
     *
     * \pre \a entity must not be null
     * \pre \a fieldName must not be empty
     */
    SelectField(const QueryEntity & entity, const QString & fieldName, const FieldAlias & fieldAlias = FieldAlias());

    /*! \brief Construct a select field from a query field
     *
     * \pre \a field must not be null
     * \todo precondition is deprecaed
     */
    SelectField(const QueryField & field);

    /*! \brief Copy construct a select field from \a other
     */
    SelectField(const SelectField & other) = default;

    /*! \brief Copy assign \a other to this select field
     */
    SelectField & operator=(const SelectField & other) = default;

    /*! \brief Move construct a select field from \a other
     */
    SelectField(SelectField && other) noexcept = default;

    /*! \brief Move assign \a other to this select field
     */
    SelectField & operator=(SelectField && other) noexcept = default;

    /*! \brief Check if this select field is null
     */
    bool isNull() const
    {
      return internalVariant().isNull();
    }

    /*! \internal Access internal variant of this select field
     */
    const SelectFieldVariant & internalVariant() const
    {
      return mVariant;
    }

   private:

    SelectFieldVariant mVariant;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SELECT_FIELD_H
