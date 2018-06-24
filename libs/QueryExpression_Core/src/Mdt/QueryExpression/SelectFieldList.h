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
#ifndef MDT_QUERY_EXPRESSION_SELECT_FIELD_LIST_H
#define MDT_QUERY_EXPRESSION_SELECT_FIELD_LIST_H

#include "FieldName.h"
#include "EntityName.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief A list of fields in a SelectQuery
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT SelectFieldList
  {
   public:

    /*! \brief Add a field named \a fieldName to this list
     *
     * \pre \a fieldName must not be null
     */
    void addField(const FieldName & fieldName, const QString & fieldAlias = QString());

    /*! \brief Add a field composed by ...
     */
    void addField(const EntityName & entityName, const FieldName & fieldName, const QString & fieldAlias = QString());

   private:

    
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_SELECT_FIELD_LIST_H
