/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtPartitionAttributes.h"
#include <QDir>

mdtPartitionAttributes::mdtPartitionAttributes()
{
  pvIsReadOnly = false;
}

mdtPartitionAttributes::~mdtPartitionAttributes()
{
}

// setPath() is implemented in system specific file

QString mdtPartitionAttributes::rootPath(bool nativeFormat)
{
#ifdef Q_OS_WIN
  if(nativeFormat){
    return QDir::toNativeSeparators(pvRootPath);
  }
#endif
  return pvRootPath;
}

QString &mdtPartitionAttributes::name()
{
  return pvName;
}

bool mdtPartitionAttributes::isReadOnly()
{
  return pvIsReadOnly;
}

bool mdtPartitionAttributes::isWriteable()
{
  return !pvIsReadOnly;
}

QString &mdtPartitionAttributes::fileSystem()
{
  return pvFileSystem;
}

// availablePartitions() is implemented in system specific file