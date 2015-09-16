/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtSqlCopierCodec.h"
#include "mdtAbstractSqlCopierCodec.h"
#include "mdtSqlCopierCodecSqlite.h"

#include <QDebug>

mdtSqlCopierCodec::mdtSqlCopierCodec()
 : pvImpl(new mdtAbstractSqlCopierCodec)
{
}

mdtSqlCopierCodec::~mdtSqlCopierCodec()
{
}

void mdtSqlCopierCodec::setSettings(const mdtSqlCopierCodecSettings & cs)
{
  Q_ASSERT(pvImpl);

  // Check if it's needed to create a other instance
  if(pvImpl->settings().type() == cs.type()){
    pvImpl->setSettings(cs);
    return;
  }
  // Create new instnace and store settings
  switch(cs.type()){
    case mdtSqlCopierCodecSettings::UnknownCodec:
      pvImpl.reset(new mdtAbstractSqlCopierCodec);
      pvImpl->setSettings(cs);
      break;
    case mdtSqlCopierCodecSettings::SqliteCodec:
      pvImpl.reset(new mdtSqlCopierCodecSqlite);
      pvImpl->setSettings(cs);
      break;
  }
}

mdtSqlCopierCodecSettings mdtSqlCopierCodec::settings() const
{
  Q_ASSERT(pvImpl);
  return pvImpl->settings();
}

bool mdtSqlCopierCodec::openTarget()
{
  Q_ASSERT(pvImpl);
  return pvImpl->openTarget();
}

void mdtSqlCopierCodec::close()
{
  Q_ASSERT(pvImpl);
  pvImpl->close();
}

QStringList mdtSqlCopierCodec::fieldNameList() const
{
  Q_ASSERT(pvImpl);
  return pvImpl->fieldNameList();
}

mdtError mdtSqlCopierCodec::lastError() const
{
  Q_ASSERT(pvImpl);
  return pvImpl->lastError();
}
