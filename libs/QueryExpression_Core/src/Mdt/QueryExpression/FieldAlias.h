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
#ifndef MDT_QUERY_EXPRESSION_FIELD_ALIAS_H
#define MDT_QUERY_EXPRESSION_FIELD_ALIAS_H

#include "MdtQueryExpression_CoreExport.h"
#include <QString>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Wrapper for compile time checking
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT FieldAlias
  {
   public:

    /*! \brief Construct a null field alias
     */
    FieldAlias() noexcept = default;

    /*! \brief Copy construct a field alias from \a other
     */
    FieldAlias(const FieldAlias & other) = default;

    /*! \brief Copy assign \a other to this field alias
     */
    FieldAlias & operator=(const FieldAlias & other) = default;

    /*! \brief Move construct a field alias from \a other
     */
    FieldAlias(FieldAlias && other) noexcept = default;

    /*! \brief Move assign \a other to this field alias
     */
    FieldAlias & operator=(FieldAlias && other) noexcept = default;

    /*! \breif Construct explicitly a field alias
     */
    explicit FieldAlias(const QString & alias)
     : mFieldAlias (alias.trimmed()) {}

    /*! \brief Check if this field alias is null
     */
    bool isNull() const
    {
      return mFieldAlias.isEmpty();
    }

    /*! \brief Get filed alias as string
     */
    QString toString() const
    {
      return mFieldAlias;
    }

   private:

    QString mFieldAlias;
  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // MDT_QUERY_EXPRESSION_FIELD_ALIAS_H
