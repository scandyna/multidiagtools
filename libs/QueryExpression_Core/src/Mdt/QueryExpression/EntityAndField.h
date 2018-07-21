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
#ifndef MDT_QUERY_EXPRESSION_ENTITY_AND_FIELD_H
#define MDT_QUERY_EXPRESSION_ENTITY_AND_FIELD_H

#include "FieldName.h"
#include "SelectEntity.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Represents a field and the optional field alias and maybe a SelectEntity
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT EntityAndField
  {
   public:

    /*! \brief Construct with a field name and a optional field alias
     *
     * \pre \a fieldName must not be null
     */
    EntityAndField(const FieldName & fieldName, const QString & fieldAlias = QString());

    /*! \brief Construct with a select entity, a field name and a optional field alias
     *
     * \pre \a entity must not be null
     * \pre \a fieldName must not be null
     */
    EntityAndField(const SelectEntity & entity, const FieldName & fieldName, const QString & fieldAlias = QString());

    /*! \brief Copy construct from \a other
     */
    EntityAndField(const EntityAndField & other) = default;

    /*! \brief Copy assign \a other to this
     */
    EntityAndField & operator=(const EntityAndField & other) = default;

    /*! \brief Move construct from \a other
     */
    EntityAndField(EntityAndField && other) = default;

    /*! \brief Move assign \a other to this
     */
    EntityAndField & operator=(EntityAndField && other) = default;

    /*! \brief Check if a entity was defined
     */
    bool hasEntity() const noexcept
    {
      return !mEntity.isNull();
    }

    /*! \brief Get the entity name
     */
    QString entityName() const
    {
      return mEntity.name();
    }

    /*! \brief Get the entity alias
     */
    QString entityAlias() const
    {
      return mEntity.alias();
    }

    /*! \brief Get the entity alias if set, otherwise the name
     */
    QString entityAliasOrName() const
    {
      return mEntity.aliasOrName();
    }

    /*! \brief Check if a field alias was set
     */
    bool hasFieldAlias() const noexcept
    {
      return !mFieldAlias.isEmpty();
    }

    /*! \brief Get the field name
     */
    QString fieldName() const
    {
      return mFieldName;
    }

    /*! \brief Get the field alias
     */
    QString fieldAlias() const
    {
      return mFieldAlias;
    }

    /*! \brief Get the field alias if set, otherwise the name
     */
    QString fieldAliasOrName() const
    {
      if(!mFieldAlias.isEmpty()){
        return mFieldAlias;
      }
      return mFieldName;
    }

   private:

    QString mFieldName;
    QString mFieldAlias;
    SelectEntity mEntity;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_ENTITY_AND_FIELD_H
