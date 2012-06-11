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
#include "mdtFileCopierItem.h"

mdtFileCopierItem::mdtFileCopierItem(const QString & srcFilePath, const QString & destFilePath)
{
  pvSrcFilePath = srcFilePath;
  pvDestFilePath = destFilePath;
  pvFileSize = 0;
  pvSyncAfterCopy = false;
  pvCheckAfterCopy = false;
}

mdtFileCopierItem::~mdtFileCopierItem()
{
}

void mdtFileCopierItem::setFileSize(qint64 size)
{
  pvFileSize = size;
}

void mdtFileCopierItem::setSrcFilePath(const QString & filePath)
{
  pvSrcFilePath = filePath;
}

QString &mdtFileCopierItem::srcFilePath()
{
  return pvSrcFilePath;
}

void mdtFileCopierItem::setSrcFileName(const QString & fileName)
{
  pvSrcFileName = fileName;
}

QString &mdtFileCopierItem::srcFileName()
{
  return pvSrcFileName;
}

void mdtFileCopierItem::setSrcFileHash(const QByteArray &hash)
{
  pvSrcFileHash = hash;
}

QByteArray &mdtFileCopierItem::srcFileHash()
{
  return pvSrcFileHash;
}

qint64 mdtFileCopierItem::fileSize()
{
  return pvFileSize;
}

void mdtFileCopierItem::setDestFilePath(const QString & filePath)
{
  pvDestFilePath = filePath;
}

QString &mdtFileCopierItem::destFilePath()
{
  return pvDestFilePath;
}

void mdtFileCopierItem::setDestFileHash(const QByteArray &hash)
{
  pvDestFileHash = hash;
}

QByteArray &mdtFileCopierItem::destFileHash()
{
  return pvDestFileHash;
}

void mdtFileCopierItem::setSyncAfterCopy(bool sync)
{
  pvSyncAfterCopy = sync;
}

bool mdtFileCopierItem::syncAfterCopy()
{
  return pvSyncAfterCopy;
}

void mdtFileCopierItem::setCheckAfterCopy(bool check)
{
  pvCheckAfterCopy = check;
}

bool mdtFileCopierItem::checkAfterCopy()
{
  return pvCheckAfterCopy;
}

