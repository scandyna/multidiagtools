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
#ifndef MDT_QUERY_EXPRESSION_SELECT_ALL_FIELD_H
#define MDT_QUERY_EXPRESSION_SELECT_ALL_FIELD_H

#include "SelectEntity.h"
#include "MdtQueryExpression_CoreExport.h"

namespace Mdt{ namespace QueryExpression{

  /*! \brief Representation to select all fields used in SelectQuery
   *
   * \sa SelectQuery
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT SelectAllField
  {
   public:

    /*! \brief Construct a select all field
     *
     * This would be equivalent to * in SQL.
     */
    SelectAllField() noexcept = default;

    /*! \brief Construct a select all field
     *
     * This would be equivalent to entity.* in SQL.
     *
     * \pre \a entity must not be null
     */
    SelectAllField(const SelectEntity & entity)
     : mEntity(entity)
    {
      Q_ASSERT(!mEntity.isNull());
    }

    /*! \brief Copy construct a select all field from \a other
     */
    SelectAllField(const SelectAllField & other) = default;

    /*! \brief Copy assign \a other to this select all field
     */
    SelectAllField & operator=(const SelectAllField & other) = default;

    /*! \brief Move construct a select  all field from \a other
     */
    SelectAllField(SelectAllField && other) = default;

    /*! \brief Move assign \a other to this select all field
     */
    SelectAllField & operator=(SelectAllField && other) = default;

    /*! \brief Get the entity alias if set, otherwise the name
     */
    QString entityAliasOrName() const
    {
      return mEntity.aliasOrName();
    }

   private:

    SelectEntity mEntity;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SELECT_ALL_FIELD_H
