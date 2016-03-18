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
#ifndef MDT_SQL_ERROR_H
#define MDT_SQL_ERROR_H

#include "mdtError.h"
#include <QSqlError>
#include <QSqlQuery>

#define ErrorFromQsqlQuery(query) mdt::sql::error::fromQsqlQuery(query, __FILE__, __LINE__)

namespace mdt{ namespace sql{ namespace error{

  /*! \brief Get a mdtError from given QSqlError
   */
  [[deprecated]]
  mdtError fromQSqlError(const QSqlError & sqlError);

  /*! \brief Get a mdtError from last error of given query
   */
  mdtError fromQsqlQuery(const QSqlQuery & query, const QString & file = "", int line = 0);

}}} // namespace mdt{ namespace sql{ namespace error{

#endif // #ifndef MDT_SQL_ERROR_H
