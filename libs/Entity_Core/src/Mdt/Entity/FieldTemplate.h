/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ENTITY_FIELD_TEMPLATE_H
#define MDT_ENTITY_FIELD_TEMPLATE_H

#include <QDebug>

namespace Mdt{ namespace Entity{

  /*! \brief Class template to create a entity field
   *
   * This class provides the common stuff,
   *  like constructors, assignment and cast operators.
   *
   * To create a entity field,
   *  the MDT_ENTITY_FIELD() macro should be used.
   */
  template<typename T, typename Derived>
  class FieldTemplate
  {
  public:

    /*! \brief Default constructor
     */
    FieldTemplate() = default;

    /*! \brief Copy construct a field from \a other
     */
    FieldTemplate(const FieldTemplate & other) = default;

    /*! \brief Move construct a field from \a other
     */
    FieldTemplate(FieldTemplate && other) = default;

    /*! \brief Construct a field with its value set to \a value
     */
    FieldTemplate(const T & value)
     : mValue(value)
    {
    }

    /*! \brief Construct a field with its value set to \a value
     *
     * This overload permits MDT_ENTITY_FIELD() to use __VA_ARGS__ ,
     *  so the default value can be optional.
     */
    template<typename...Ts>
    FieldTemplate(const T & value, const Ts & ...args)
    : FieldTemplate(value)
    {
      static_assert(sizeof...(args) == 0, "a entity column only supports a default value");
    }

    /*! \brief Copy assign \a other to this field
     */
    FieldTemplate & operator=(const FieldTemplate & other) = default;

    /*! \brief Move assign \a other to this field
     */
    FieldTemplate & operator=(FieldTemplate && other) = default;

    /*! \brief Copy \a value to this field
     */
    Derived & operator=(const T & value)
    {
      qDebug() << "operator=(value&)";
      mValue = value;
      return static_cast<Derived&>(*this);
    }

    /*! \brief Move \a value to this field
     */
    Derived & operator=(T && value)
    {
      qDebug() << "operator=(value&&)";
      mValue = value;
      return static_cast<Derived&>(*this);
    }

    /*! \brief Cast this field to the value
     */
    operator T &()
    {
      return mValue;
    }

    /*! \brief Cast this field to the value
     */
    operator const T &() const
    {
      return mValue;
    }

  private:

    T mValue;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_FIELD_TEMPLATE_H
