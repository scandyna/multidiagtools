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
#ifndef MDT_QUERY_EXPRESSION_ENTITY_NAME_H
#define MDT_QUERY_EXPRESSION_ENTITY_NAME_H

#include "MdtQueryExpression_CoreExport.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Wrapper for compile time checking
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT EntityName
  {
   public:

    /*! \breif Construct explicitly from a field name
     */
    explicit EntityName(const QString & name)
     : mEntityName(name) {}

    /*! \brief Check if this field name is null
     */
    bool isNull() const
    {
      return mEntityName.isEmpty();
    }

    /*! \brief Get filed name as string
     */
    QString toString() const
    {
      return mEntityName;
    }
    
   private:

    QString mEntityName;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // MDT_QUERY_EXPRESSION_ENTITY_NAME_H
