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
#include "SQLiteAsyncQueryThreadWorker.h"
#include "SQLiteDatabase.h"

// #include <QDebug>

namespace Mdt{ namespace Sql{

SQLiteAsyncQueryThreadWorker::SQLiteAsyncQueryThreadWorker(const SQLiteConnectionParameters & parameters, const QString & connectionNamePrefix)
 : AbstractAsyncQueryThreadWorker(),
   mParameters(parameters),
   mConnectionNamePrefix(connectionNamePrefix)
{
}

bool SQLiteAsyncQueryThreadWorker::open()
{
  close();

  const auto connection = SQLiteDatabase::addConnection(mConnectionNamePrefix);
  if(!connection){
    emit openErrorOccured(connection.error());
    return false;
  }
  setConnection(*connection);

  SQLiteDatabase db(*connection, mParameters);
  if(!db.open()){
    emit openErrorOccured(db.lastError());
    return false;
  }
  emit openDone();

  return true;
}

void SQLiteAsyncQueryThreadWorker::processOpen()
{
  open();
}

}} // namespace Mdt{ namespace Sql{
