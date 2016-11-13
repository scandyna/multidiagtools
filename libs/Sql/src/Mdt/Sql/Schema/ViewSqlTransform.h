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
#ifndef MDT_SQL_SCHEMA_VIEW_SQL_TRANSFORM_H
#define MDT_SQL_SCHEMA_VIEW_SQL_TRANSFORM_H

#include <QString>

class QSqlDatabase;

namespace Mdt{ namespace Sql{ namespace Schema{

  class View;

  /*! \brief Transform a View to its SQL representation
   */
  class ViewSqlTransform
  {
   public:

    /*! \brief Get SQL string to create view
     *
     * \pre view must not be null
     * \pre db must be valid (must have a driver loaded)
     */
    static QString getSqlToCreateView(const View & view, const QSqlDatabase & db);

    /*! \brief Get SQL string to drop view
     *
     * \pre view must have its name set
     * \pre db must be valid (must have a driver loaded)
     */
    static QString getSqlToDropView(const View & view, const QSqlDatabase & db);
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_VIEW_SQL_TRANSFORM_H
