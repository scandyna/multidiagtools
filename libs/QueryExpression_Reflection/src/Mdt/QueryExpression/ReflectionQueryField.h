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
#ifndef MDT_QUERY_EXPRESSION_REFLECTION_QUERY_FIELD_H
#define MDT_QUERY_EXPRESSION_REFLECTION_QUERY_FIELD_H

// #include "FieldAlias.h"
// #include "EntityAlias.h"
#include "Mdt/QueryExpression/QueryField.h"
#include "Mdt/Reflection/FieldAlgorithm.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsField.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Represents a field for a query expression
   *
   * \pre \a Field must be a field defined in a struct definition associated with a reflected struct
   */
  template<typename Field>
  class ReflectionQueryField : public QueryField
  {
    static_assert( Mdt::Reflection::TypeTraits::IsField<Field>::value ,
                   "Field must be a field defined in a struct definition associated with a reflected struct" );

    using BaseClass = QueryField;

   public:

//     /*! \brief Construct a null query field
//      */
//     ReflectionQueryField() noexcept = default;

    /*! \brief Copy construct a query field from \a other
     */
    ReflectionQueryField(const ReflectionQueryField & other) = default;

    /*! \brief Copy assign \a other to this query field
     */
    ReflectionQueryField & operator=(const ReflectionQueryField & other) = default;

    /*! \brief Move construct a query field from \a other
     */
    ReflectionQueryField(ReflectionQueryField && other) noexcept = default;

    /*! \brief Move assign \a other to this query field
     */
    ReflectionQueryField & operator=(ReflectionQueryField && other) noexcept = default;

    /*! \brief Construct a query field with a optional field alias
     */
    ReflectionQueryField(const FieldAlias & fieldAlias = FieldAlias())
     : BaseClass( makeQueryEntity(), fieldNameQString(), fieldAlias  )
    {
    }

    /*! \brief Construct a query field with a entity alias and a optional field alias
     */
    ReflectionQueryField(const EntityAlias & entityAlias, const FieldAlias & fieldAlias = FieldAlias())
     : BaseClass( makeQueryEntity(entityAlias), fieldNameQString(), fieldAlias  )
    {
    }

   private:

    static QueryEntity makeQueryEntity(const EntityAlias & entityAlias = EntityAlias())
    {
      return QueryEntity( QString::fromLatin1(Mdt::Reflection::nameFromField<Field>()), entityAlias );
    }

    static QString fieldNameQString()
    {
      return QString::fromLatin1( Mdt::Reflection::fieldName<Field>() );
    }

//     /*! \brief Construct a query field
//      *
//      * \pre \a entity must not be null
//      * \pre \a fieldName must not be empty
//      */
//     ReflectionQueryField(const QueryEntity & entity, const QString & fieldName, const FieldAlias & fieldAlias = FieldAlias())
//     {
//       Q_ASSERT(!entity.isNull());
//     }

//     /*! \brief Set the entity name and alias
//      *
//      * \pre \a name must not be empty
//      */
//     void setNameAndAlias(const QString & name, const EntityAlias & alias = EntityAlias())
//     {
//       mName = name.trimmed();
//       mAlias = alias.toString();
//       Q_ASSERT(!mName.isEmpty());
//     }

//     /*! \brief Check if this query entity is null
//      */
//     bool isNull() const noexcept
//     {
//       return mName.isEmpty();
//     }
// 
//     /*! \brief Check if the alias was set
//      */
//     bool hasAlias() const noexcept
//     {
//       return !mAlias.isEmpty();
//     }
// 
//     /*! \brief Get the entity name
//      */
//     QString name() const
//     {
//       return mName;
//     }
// 
//     /*! \brief Get the entity alias
//      */
//     QString alias() const
//     {
//       return mAlias;
//     }
// 
//     /*! \brief Get the entity alias if set, otherwise the name
//      */
//     QString aliasOrName() const
//     {
//       if(mAlias.isEmpty()){
//         return mName;
//       }
//       return mAlias;
//     }
// 
//     /*! \brief Clear this query entity
//      */
//     void clear()
//     {
//       mName.clear();
//       mAlias.clear();
//     }

//   private:
// 
//     QString mName;
//     QString mAlias;
  };
}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_REFLECTION_QUERY_FIELD_H
