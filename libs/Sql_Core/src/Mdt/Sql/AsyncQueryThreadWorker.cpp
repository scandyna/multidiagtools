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
#include "AsyncQueryThreadWorker.h"
#include "Error.h"
#include "Mdt/Container/VariantRecord.h"
#include <boost/graph/buffer_concepts.hpp>
#include <QLatin1String>
#include <QSqlQuery>
#include <QSqlRecord>

using Mdt::Container::VariantRecord;

namespace Mdt{ namespace Sql{

AsyncQueryThreadWorker::AsyncQueryThreadWorker(QObject* parent)
 : QObject(parent),
   mDatabase( std::make_unique<QSqlDatabase>() )
{
}

AsyncQueryThreadWorker::~AsyncQueryThreadWorker()
{
  const auto connectionName = mDatabase->connectionName();

  mDatabase->close();
  mDatabase.reset(nullptr);
  QSqlDatabase::removeDatabase(connectionName);
}

bool AsyncQueryThreadWorker::setup(const ConnectionParameters & parameters)
{
  const auto connectionName = generateConnectionName( QSqlDatabase::connectionNames() );

  *mDatabase = QSqlDatabase::addDatabase( parameters.driverName(), connectionName );
  if(!mDatabase->isValid()){
    const auto msg = tr("Could not add a database connection with a driver of type %1")
                     .arg( parameters.driverName() );
    mSetupError = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    mSetupError.stackError( mdtErrorFromQSqlDatabaseQ(*mDatabase, this) );
    return false;
  }

  parameters.setupDatabase(*mDatabase);
  if(!mDatabase->open()){
    const auto msg = tr("Could not open database '%1'")
                     .arg(parameters.databaseName());
    mSetupError = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    mSetupError.stackError( mdtErrorFromQSqlDatabaseQ(*mDatabase, this) );
    return false;
  }

  return true;
}

QString AsyncQueryThreadWorker::generateConnectionName(const QStringList & existingNames)
{
  QString name;
  int n = 0;

  do{
    ++n;
    name = QLatin1String("MdtAsyncQueryConnection_") + QString::number(n);
  }while(existingNames.contains(name));

  return name;
}

void AsyncQueryThreadWorker::processQuery(const QVariant & query, int instanceId)
{
  if(query.canConvert<UpdateStatement>()){
    processUpdateStatement( query.value<UpdateStatement>(), instanceId );
  }else if(query.canConvert<QString>()){
    processSqlSelectStatement(query.toString(), instanceId);
  }else{
    const auto msg = tr("Requested a query of unknown type '%1'")
                    .arg( query.typeName() );
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    emit errorOccured(error, instanceId);
  }
}

VariantRecord AsyncQueryThreadWorker::variantRecordFromQuery(const QSqlQuery& query, int columnCount)
{
  Q_ASSERT(columnCount == query.record().count());

  VariantRecord record(columnCount);

  for(int col = 0; col < columnCount; ++col){
    record.setValue( col, query.value(col) );
  }

  return record;
}

void AsyncQueryThreadWorker::processSqlSelectStatement(const QString & sql, int instanceId)
{
  QSqlQuery query( database() );
  query.setForwardOnly(true);

  if(!query.exec(sql)){
    const auto msg = tr("Could not execute query: '%1'")
                     .arg(sql);
    auto error = mdtErrorNewQ(msg, Mdt::Error::Critical, this);
    error.stackError( mdtErrorFromQSqlQueryQ(query, this) );
    emit errorOccured(error, instanceId);
    return;
  }

  const int columnCount = query.record().count();
  while(query.next()){
    emit newRecordAvailable( variantRecordFromQuery(query, columnCount), instanceId );
  }
  /// \todo emit done() ?
}

void AsyncQueryThreadWorker::processUpdateStatement(const UpdateStatement & statement, int instanceId)
{

}

}} // namespace Mdt{ namespace Sql{
