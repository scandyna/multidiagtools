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
#include "Error.h"
#include <QSqlQuery>
#include <QSqlDatabase>

namespace mdt{ namespace sql{ namespace error{

/*! \brief Get a mdtError from given QSqlError
  */
mdtError fromQSqlError(const QSqlError & sqlError)
{
  return mdtErrorNewT(QSqlError::ErrorType, sqlError.type(), sqlError.text(), mdtError::Error, "QtSql");
//   mdtError error;
//   error.setError<QSqlError::ErrorType>(sqlError.type(), sqlError.text(), mdtError::Error);
//   return error;
}

mdtError fromQSqlQuery(const QSqlQuery & query, const QString & file, int line)
{
  mdtError error;
  auto sqlError = query.lastError();

  error.setError<QSqlError::ErrorType>(sqlError.type(), sqlError.text(), mdtError::Error);
  error.setSource(file, line, "QSqlQuery", "");

  return error;
}

mdtError fromQSqlDatabase(const QSqlDatabase & db, const QString & file, int line)
{
  mdtError error;
  auto sqlError = db.lastError();

  error.setError<QSqlError::ErrorType>(sqlError.type(), sqlError.text(), mdtError::Error);
  error.setSource(file, line, "QSqlDatabase", "");

  return error;
}

}}} // namespace mdt{ namespace sql{ namespace error{
