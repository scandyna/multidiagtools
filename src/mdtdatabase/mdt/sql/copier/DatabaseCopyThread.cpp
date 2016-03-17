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
#include "DatabaseCopyThread.h"
#include "DatabaseCopierTableMapping.h"
#include "CopyHelper.h"
#include "mdt/sql/Database.h"
#include "mdt/sql/error/Error.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

namespace mdt{ namespace sql{ namespace copier{

DatabaseCopyThread::DatabaseCopyThread(QObject *parent)
 : CopyThread(parent)
{
}

void DatabaseCopyThread::startCopy(const DatabaseMapping & mapping)
{
  Q_ASSERT(!isRunning());

  pvMapping = mapping;
  resetCopyProgress();
  start();
}

int64_t DatabaseCopyThread::calculateSourceTableSize(const DatabaseCopierTableMapping * const tm, const QSqlDatabase & sourceDatabase) const
{
  Q_ASSERT(tm != nullptr);

  QSqlQuery query(sourceDatabase);
  auto sql = CopyHelper::getSourceTableCountSql(tm, sourceDatabase);

  if(!query.exec()){
    auto error = mdtErrorNewQ(tr("Executing query to get source table count failed"), mdtError::Warning, this);
    error.stackError(mdt::sql::error::fromQSqlError(query.lastError()));
    error.commit();
    return -1;
  }
  if(!query.next()){
    return -1;
  }

  return query.value(0).toLongLong();
}

void DatabaseCopyThread::calculateTableSizes(const QSqlDatabase & sourceDatabase)
{
  for(int i = 0; i < pvMapping.tableMappingCount(); ++i){
    const auto tm = pvMapping.tableMapping(i);
    Q_ASSERT(tm);
    if(tm->mappingState() == TableMapping::MappingComplete){
      auto size = calculateSourceTableSize(tm.get(), sourceDatabase);
      setTableSize(i, size);
    }
  }
}

void DatabaseCopyThread::run()
{
  using mdt::sql::Database;

  QString sourceConnectionName;
  QString destinationConnectionName;
  {
    ///mdtProgressValue<int64_t> globalProgress;
    ///int64_t totalSize;
    auto sourceDatabase = createConnection(pvMapping.sourceDatabase());
    sourceConnectionName = sourceDatabase.connectionName();
    QSqlDatabase destinationDatabase;
    if(Database::isSameDatabase(sourceDatabase, pvMapping.destinationDatabase())){
      destinationDatabase = sourceDatabase;
    }else{
      destinationDatabase = createConnection(pvMapping.destinationDatabase());
    }
    destinationConnectionName = destinationDatabase.connectionName();

    // Check that we are successfully connected to both databases
    if(!sourceDatabase.isOpen()){
      return;
    }
    if(!destinationDatabase.isOpen()){
      return;
    }
    
    sleep(1);
    
    setTableSize(0, 50);
    setTableSize(1, 50);
    qDebug() << "Total size: " << calculateTotalCopySize();
    
    for(int i = 0; i < 50; ++i){
      msleep(50);
      incrementTableCopyProgress(0);
      msleep(100);
      incrementTableCopyProgress(1);
    }
    
    
  }
  QSqlDatabase::removeDatabase(sourceConnectionName);
  QSqlDatabase::removeDatabase(destinationConnectionName);
}

}}} // namespace mdt{ namespace sql{ namespace copier{
