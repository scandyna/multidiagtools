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
#ifndef MDT_SQL_COPIER_COPY_HELPER_H
#define MDT_SQL_COPIER_COPY_HELPER_H

#include "TableMapping.h"
#include "RelatedTableInsertExpression.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlDriver>

namespace mdt{ namespace sql{ namespace copier{

  /*! \internal Field name list container
   */
  struct CopyHelperFieldNameLists
  {
    QStringList rtiExpFieldNameList;
    QStringList fieldMapFieldNameList;
    QStringList fixedValueFieldNameList;
  };

  /*! \brief Helper class for data copy
   */
  class CopyHelper
  {
   public:

    /*! \brief Get SQL to select data in source table
     */
    static QString getSourceTableSelectSql(const TableMapping * const tm, const QSqlDatabase & sourceDb);

    /*! \brief Get list of destination field names for related table insert expression
     */
//     static QStringList getExpressionDestinationFieldNameList(const TableMapping * const tm, const RelatedTableInsertExpression & exp);

    /*! \brief Get list of destination field names
     */
    static CopyHelperFieldNameLists getDestinationFieldNameLists(const TableMapping *const tm);

    /*! \brief Get SQL for insertion into destination table
     */
    static QString getDestinationInsertSql(const TableMapping * const tm, const QSqlDatabase & destinationDb);

  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_COPY_HELPER_H
