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
#ifndef MDT_QUERY_EXPRESSION_ENTITY_ALIAS_H
#define MDT_QUERY_EXPRESSION_ENTITY_ALIAS_H

#include "MdtQueryExpression_CoreExport.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Wrapper for compile time checking
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT EntityAlias
  {
   public:

    /*! \brief Construct a null entity alias
     */
    EntityAlias() noexcept = default;

    /*! \brief Copy construct a entity alias from \a other
     */
    EntityAlias(const EntityAlias & other) = default;

    /*! \brief Copy assign \a other to this entity alias
     */
    EntityAlias & operator=(const EntityAlias & other) = default;

    /*! \brief Move construct a entity alias from \a other
     */
    EntityAlias(EntityAlias && other) noexcept = default;

    /*! \brief Move assign \a other to this entity alias
     */
    EntityAlias & operator=(EntityAlias && other) noexcept = default;

    /*! \breif Construct explicitly from a entity alias
     */
    explicit EntityAlias(const QString & alias)
     : mEntityAlias (alias.trimmed()) {}

    /*! \brief Check if this entity alias is null
     */
    bool isNull() const
    {
      return mEntityAlias.isEmpty();
    }

    /*! \brief Get filed name as string
     */
    QString toString() const
    {
      return mEntityAlias;
    }

   private:

    QString mEntityAlias;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // MDT_QUERY_EXPRESSION_ENTITY_ALIAS_H
