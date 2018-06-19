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
#ifndef MDT_QUERY_EXPRESSION_FIELD_NAME_H
#define MDT_QUERY_EXPRESSION_FIELD_NAME_H

#include "MdtQueryExpression_CoreExport.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Wrapper for compile time checking
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT FieldName
  {
   public:

    /*! \breif Construct explicitly from a field name
     */
    explicit FieldName(const QString & name)
     : mFieldName(name) {}

    /*! \brief Check if this field name is null
     */
    bool isNull() const
    {
      return mFieldName.isEmpty();
    }

    /*! \brief Get filed name as string
     */
    QString toString() const
    {
      return mFieldName;
    }

    /*! \brief Check if field names \a a and \a b are equal
     *
     * The comparison is done in a case insensitive way,
     *  for example id_pk is the same as Id_PK .
     */
    friend
    bool operator==(const FieldName & a, const FieldName & b)
    {
      return (QString::compare(a.mFieldName, b.mFieldName, Qt::CaseInsensitive) == 0);
    }
    
   private:

    QString mFieldName;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // MDT_QUERY_EXPRESSION_FIELD_NAME_H
