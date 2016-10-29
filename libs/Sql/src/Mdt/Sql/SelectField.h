/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_SQL_SELECT_FIELD_H
#define MDT_SQL_SELECT_FIELD_H

// #include "FieldName.h"
#include <QString>

namespace Mdt{ namespace Sql{

  /*! \brief SelectField represents a field in a SQL SELECT statement
   */
  class SelectField
  {
   public:

    /*! \brief Constructor
     */
//     explicit SelectField(const Field & field, const QString & alias = QString())
//      : pvFieldName(field.name()), pvAlias(alias) {}

    /*! \brief Constructor
     */
//     explicit SelectField(const FieldName & name, const QString alias = QString())
//      : pvFieldName(name.toString()), pvAlias(alias) {}

    /*! \brief Constructor
     */
    explicit SelectField(const QString & fieldName, const QString & alias = QString())
     : pvFieldName(fieldName), pvAlias(alias) {}

    /*! \brief Get field name
     */
    QString fieldName() const
    {
      return pvFieldName;
    }

    /*! \brief Get alias
     */
    QString alias() const
    {
      return pvAlias;
    }

   private:

    QString pvFieldName;
    QString pvAlias;
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SELECT_FIELD_H
