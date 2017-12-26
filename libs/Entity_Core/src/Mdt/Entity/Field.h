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
#ifndef MDT_ENTITY_FIELD_H
#define MDT_ENTITY_FIELD_H

#include "FieldTemplate.h"
#include <boost/preprocessor/cat.hpp>

namespace Mdt{ namespace Entity{

  /*! \brief Create and instanciate a field class
   *
   * \code
   * struct ClientDataStruct
   * {
   *   MDT_ENTITY_FIELD(qlonlong, id, 0)
   *   MDT_ENTITY_FIELD(QString, firstName)
   * };
   * \endcode
   *
   * The ClientDataStruct will contains 2 nested classes,
   *  that provides the filed's value, by implicit cast,
   *  and the field name:
   * \code
   * ClientDataStruct data;
   * qDebug() << data.id.fieldName() << ": " << data.id;
   * \endcode
   * will output: id: 0
   *
   * The fields of ClientDataStruct can be accessed for read and write:
   * \code
   * data.id = 25;
   * data.firstName = "Some name";
   * \endcode
   *
   * \deprecated
   */
  #define MDT_ENTITY_FIELD(Type, name, ...)                                                               \
    class BOOST_PP_CAT(name, Field) : public Mdt::Entity::FieldTemplate<Type, BOOST_PP_CAT(name, Field)>  \
    {                                                                                                     \
    public:                                                                                               \
      using FieldTemplate<Type, BOOST_PP_CAT(name, Field)>::operator=;                                    \
      using FieldTemplate<Type, BOOST_PP_CAT(name, Field)>::FieldTemplate;                                \
      static const QString fieldName()                                                                    \
      {                                                                                                   \
        return QString::fromUtf8(#name);                                                                  \
      }                                                                                                   \
    };                                                                                                    \
    BOOST_PP_CAT(name, Field) name{__VA_ARGS__};

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_FIELD_H
