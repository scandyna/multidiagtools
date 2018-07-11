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
#ifndef MDT_ENTITY_QUERY_ENTITY_H
#define MDT_ENTITY_QUERY_ENTITY_H

#include "TypeTraits/IsEntity.h"
#include "TypeTraits/IsEntityFieldDef.h"
#include "Mdt/QueryExpression/SelectEntity.h"
#include "Mdt/QueryExpression/SelectField.h"
#include <QString>

namespace Mdt{ namespace Entity{

  /*! \brief Helper class to create queries using EntitySelectQuery
   *
   * \sa EntitySelectQuery
   */
  template<typename Entity>
  class QueryEntity
  {
    static_assert( TypeTraits::IsEntity<Entity>::value, "Entity must be a entity type" );

   public:

    using entity_def_type = typename Entity::def_type;

    /*! \brief Construct a query entity
     */
    QueryEntity() = default;

    /*! \brief Construct a query entity with a alias
     *
     * \pre \a alias must nut be empty
     */
    QueryEntity(const QString & alias)
     : mAlias(alias.trimmed())
    {
      Q_ASSERT(!mAlias.isEmpty());
    }

    /*! \brief Get the select entity representation of this query entity
     */
    Mdt::QueryExpression::SelectEntity toSelectEntity() const
    {
      return Mdt::QueryExpression::SelectEntity( Mdt::QueryExpression::EntityName(def().entityName()), mAlias );
    }

    /*! \brief Create a select field
     */
    template<typename FieldDef>
    Mdt::QueryExpression::SelectField makeSelectField(const FieldDef & fieldDef) const
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );

      return Mdt::QueryExpression::SelectField( toSelectEntity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()) );
    }

    /*! \brief Create a select field with a field alias
     *
     * \pre \a fieldAlias must not be empty
     */
    template<typename FieldDef>
    Mdt::QueryExpression::SelectField makeSelectField(const FieldDef & fieldDef, const QString & fieldAlias) const
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "FieldDef must be a entity field definition type" );
      Q_ASSERT(!fieldAlias.trimmed().isEmpty());

      return Mdt::QueryExpression::SelectField( toSelectEntity(), Mdt::QueryExpression::FieldName(fieldDef.fieldName()), fieldAlias );
    }

    /*! \brief Get entity definition
     */
    static const entity_def_type def()
    {
      return Entity::def();
    }


   private:

    QString mAlias;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_QUERY_ENTITY_H
