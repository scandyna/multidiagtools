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
#ifndef MDT_QUERY_EXPRESSION_SELECT_ENTITY_H
#define MDT_QUERY_EXPRESSION_SELECT_ENTITY_H

#include "EntityName.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Represents a entity name and its optional alias
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT SelectEntity
  {
   public:

    /*! \brief Constructor
     *
     * \pre \a name must not be null
     */
    SelectEntity(const EntityName & name, const QString & alias = QString());

    /*! \brief Get the entity name
     */
    QString name() const
    {
      return mName;
    }

    /*! \brief Get the entity alias
     */
    QString alias() const
    {
      return mAlias;
    }

    /*! \brief Get the entity alias if set, otherwise the name
     */
    QString aliasOrName() const
    {
      if(mAlias.isEmpty()){
        return mName;
      }
      return mAlias;
    }

  private:

    QString mName;
    QString mAlias;
  };
}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SELECT_ENTITY_H
