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
#ifndef MDT_QUERY_EXPRESSION_REFLECTION_QUERY_ENTITY_H
#define MDT_QUERY_EXPRESSION_REFLECTION_QUERY_ENTITY_H

#include "Mdt/QueryExpression/QueryEntity.h"
#include "Mdt/QueryExpression/EntityAlias.h"
#include "Mdt/Reflection/StructAlgorithm.h"
#include "Mdt/Reflection/TypeTraits/IsStructDef.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{ namespace Reflection{

  /*! \brief Represents a entity name and its optional alias
   *
   * \pre \a StructDef must be a struct definition assiocated with a reflected struct
   *
   * \todo Seems not usefull + is it should be used, rename -> QueryEntityReflection
   */
  template<typename StructDef>
  class [[deprecated]] QueryEntity : public Mdt::QueryExpression::QueryEntity
  {
    using BaseClass = Mdt::QueryExpression::QueryEntity;

   public:

    using BaseClass::BaseClass;
    using BaseClass::operator=;

//     /*! \brief Construct a null query entity
//      */
//     QueryEntity() noexcept = default;
// 
//     /*! \brief Copy construct a query entity from \a other
//      */
//     QueryEntity(const QueryEntity & other) = default;
// 
//     /*! \brief Copy assign \a other to this query entity
//      */
//     QueryEntity & operator=(const QueryEntity & other) = default;
// 
//     /*! \brief Move construct a query entity from \a other
//      */
//     QueryEntity(QueryEntity && other) = default;
// 
//     /*! \brief Move assign \a other to this query entity
//      */
//     QueryEntity & operator=(QueryEntity && other) = default;

    /*! \brief Construct a query entity
     */
    QueryEntity(const EntityAlias & alias = EntityAlias())
     : BaseClass(QString::fromLocal8Bit(Mdt::Reflection::nameFromStructDef<StructDef>()), alias)
    {
      static_assert( Mdt::Reflection::TypeTraits::IsStructDef<StructDef>::value,
                     "StructDef must be a struct definition assiocated with a reflected struct" );
    }

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
// 
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
// 
//   private:
// 
//     QString mName;
//     QString mAlias;
  };
}}} // namespace Mdt{ namespace QueryExpression{ namespace Reflection{

#endif // #ifndef MDT_QUERY_EXPRESSION_REFLECTION_QUERY_ENTITY_H
