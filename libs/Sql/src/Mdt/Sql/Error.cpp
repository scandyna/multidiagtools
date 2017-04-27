/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include <QSqlQueryModel>
#include <QString>
#include <QObject>
#include <QMetaObject>

#include <QDebug>

namespace Mdt{ namespace Sql{

Mdt::Error Error::fromQSqlQuery(const QSqlQuery & query, const QString & file, int line, const QString & className, const QString & functionName)
{
  Mdt::Error error;
  auto sqlError = query.lastError();
  QString msg;

  msg = tr("Reported from QSqlQuery: %1").arg(sqlError.text());
  if(sqlError.type() == QSqlError::StatementError){
    msg += tr(" , SQL: %1").arg(query.lastQuery());
  }
  error.setError<QSqlError::ErrorType>(sqlError.type(), msg, levelFromQSqlErrorType(sqlError.type()));
  error.setSource(file, line, className, functionName);

  return error;
}

Mdt::Error Sql::Error::fromQSqlQuery(const QSqlQuery& query, const QString& file, int line, const QObject*const obj, const QString& functionName)
{
  Q_ASSERT(obj != nullptr);
  Q_ASSERT(obj->metaObject() != nullptr);

  return fromQSqlQuery(query, file, line, obj->metaObject()->className(), functionName);
}

Mdt::Error Error::fromQSqlQueryModel(const QSqlQueryModel & queryModel, const QString & file, int line, const QString & className, const QString & functionName)
{
  Mdt::Error error;
  auto sqlError = queryModel.lastError();
  QString msg;

  msg = tr("Reported from QSqlQueryModel: %1").arg(sqlError.text());
  if(sqlError.type() == QSqlError::StatementError){
    msg += tr(" , SQL: %1").arg(queryModel.query().lastQuery());
  }
  error.setError<QSqlError::ErrorType>(sqlError.type(), msg, levelFromQSqlErrorType(sqlError.type()));
  error.setSource(file, line, className, functionName);

  return error;
}

Mdt::Error Sql::Error::fromQSqlQueryModel(const QSqlQueryModel*const queryModel, const QString& file, int line, const QString& className, const QString& functionName)
{
  Q_ASSERT(queryModel != nullptr);

  return fromQSqlQueryModel(*queryModel, file, line, className, functionName);
}

Mdt::Error Sql::Error::fromQSqlQueryModel(const QSqlQueryModel & queryModel, const QString& file, int line, const QObject*const obj, const QString& functionName)
{
  Q_ASSERT(obj != nullptr);
  Q_ASSERT(obj->metaObject() != nullptr);

  return fromQSqlQueryModel(queryModel, file, line, obj->metaObject()->className(), functionName);
}

Mdt::Error Sql::Error::fromQSqlQueryModel(const QSqlQueryModel * const queryModel, const QString& file, int line, const QObject*const obj, const QString& functionName)
{
  Q_ASSERT(queryModel != nullptr);

  return fromQSqlQueryModel(*queryModel, file, line, obj, functionName);
}

Mdt::Error Sql::Error::fromQSqlDatabase(const QSqlDatabase & db, const QString& file, int line, const QString& className, const QString& functionName)
{
  Mdt::Error error;
  auto sqlError = db.lastError();
  QString msg;

  msg = tr("Reported from QSqlDatabase: %1").arg(sqlError.text());
  error.setError<QSqlError::ErrorType>(sqlError.type(), msg, levelFromQSqlErrorType(sqlError.type()));
  error.setSource(file, line, className, functionName);

  return error;
}

Mdt::Error Sql::Error::fromQSqlDatabase(const QSqlDatabase & db, const QString& file, int line, const QObject*const obj, const QString& functionName)
{
  Q_ASSERT(obj != nullptr);
  Q_ASSERT(obj->metaObject() != nullptr);

  return fromQSqlDatabase(db, file, line, obj->metaObject()->className(), functionName);
}

Mdt::Error::Level Sql::Error::levelFromQSqlErrorType(QSqlError::ErrorType type)
{
  switch(type){
    case QSqlError::NoError:
      return Mdt::Error::NoError;
    case QSqlError::ConnectionError:
    case QSqlError::StatementError:
    case QSqlError::TransactionError:
    case QSqlError::UnknownError:
      break;
  }
  return Mdt::Error::Critical;
}

QString Sql::Error::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}

}} // namespace Mdt{ namespace Sql{
