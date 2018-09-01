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
#include "AsyncQuery.h"

using Mdt::Container::VariantRecord;

namespace Mdt{ namespace Sql{

AsyncQuery::AsyncQuery(int instanceId, QObject* parent)
 : QObject(parent),
   mInstanceId(instanceId)
{
}

void AsyncQuery::submitQuery(const QVariant & query)
{
  emit queryRequested(query, mInstanceId);
}

void AsyncQuery::setNewRecordAvailable(const VariantRecord & record, int instanceId)
{
  if(instanceId == mInstanceId){
    emit newRecordAvailable(record);
  }
}

void AsyncQuery::setErrorOccured(const Mdt::Error & error, int instanceId)
{
  if(instanceId == mInstanceId){
    emit errorOccured(error);
  }
}

}} // namespace Mdt{ namespace Sql{
