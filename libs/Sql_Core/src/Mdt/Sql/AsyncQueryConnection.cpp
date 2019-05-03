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
#include "AsyncQueryConnection.h"
#include "AbstractAsyncQueryThreadWorker.h"
#include "Mdt/Container/VariantRecord.h"
#include <QMetaType>

namespace Mdt{ namespace Sql{

AsyncQueryConnection::AsyncQueryConnection(QObject *parent)
 : QObject(parent)
{
  // qRegisterMetaType<T>() can be called multiple time
  qRegisterMetaType<Mdt::Container::VariantRecord>();

  connect(&mImpl, &AsyncQueryConnectionImpl::closed, this, &AsyncQueryConnection::closed);
}

void AsyncQueryConnection::submitClose()
{
  mImpl.submitClose();
}

void AsyncQueryConnection::close()
{
  mImpl.close();
}

}} // namespace Mdt{ namespace Sql{
