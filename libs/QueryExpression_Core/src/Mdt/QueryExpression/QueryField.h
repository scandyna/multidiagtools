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
#ifndef MDT_QUERY_EXPRESSION_QUERY_FIELD_H
#define MDT_QUERY_EXPRESSION_QUERY_FIELD_H

#include "FieldAlias.h"
#include "EntityAlias.h"
#include "QueryEntity.h"
#include "SelectAllField.h"
#include "QueryFieldVariant.h"
#include "MdtQueryExpression_CoreExport.h"
#include <boost/proto/expr.hpp>
#include <boost/proto/extends.hpp>
#include <boost/proto/operators.hpp>
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Represents a field for a query expression
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT QueryField
  {
   public:

//     /*! \brief Construct a null query field
//      */
//     QueryField() noexcept = default;

    /*! \brief Copy construct a query field from \a other
     */
    QueryField(const QueryField & other) = default;

    /*! \brief Copy assign \a other to this query field
     */
    QueryField & operator=(const QueryField & other) = default;

    /*! \brief Move construct a query field from \a other
     */
    QueryField(QueryField && other) noexcept = default;

    /*! \brief Move assign \a other to this query field
     */
    QueryField & operator=(QueryField && other) noexcept = default;

    /*! \brief Construct a select all field
     */
    QueryField(const SelectAllField & field)
    {
    }

    /*! \brief Construct a query field
     *
     * \pre \a fieldName must not be empty
     */
    QueryField(const QString & fieldName, const FieldAlias & fieldAlias = FieldAlias())
    {
//       Q_ASSERT(!mName.isEmpty());
    }

    /*! \brief Construct a query field
     *
     * \pre \a entity must not be null
     * \pre \a fieldName must not be empty
     */
    QueryField(const QueryEntity & entity, const QString & fieldName, const FieldAlias & fieldAlias = FieldAlias())
    {
      Q_ASSERT(!entity.isNull());
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

    /*! \brief Check if this query entity is null
     */
    bool isNull() const noexcept
    {
//       return mName.isEmpty();
    }

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
}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_QUERY_FIELD_H
