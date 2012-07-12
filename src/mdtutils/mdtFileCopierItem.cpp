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
#include <QObject>

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

void mdtFileCopierItem::setSrcDirectoryPath(const QString & directoryPath)
{
  pvSrcDirectoryPath = directoryPath;
}

QString &mdtFileCopierItem::srcDirectoryPath()
{
  return pvSrcDirectoryPath;
}

void mdtFileCopierItem::setSrcFileHash(const QByteArray &hash)
{
  pvSrcFileHash = hash;
}

QByteArray &mdtFileCopierItem::srcFileHash()
{
  return pvSrcFileHash;
}

void mdtFileCopierItem::setFileSize(qint64 size)
{
  pvFileSize = size;
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

void mdtFileCopierItem::setDestFileName(const QString & fileName)
{
  pvDestFileName = fileName;
}

QString &mdtFileCopierItem::destFileName()
{
  return pvDestFileName;
}

void mdtFileCopierItem::setDestDirectoryPath(const QString & directoryPath)
{
  pvDestDirectoryPath = directoryPath;
}

QString &mdtFileCopierItem::destDirectoryPath()
{
  return pvDestDirectoryPath;
}

void mdtFileCopierItem::setDestFileHash(const QByteArray &hash)
{
  pvDestFileHash = hash;
}

QByteArray &mdtFileCopierItem::destFileHash()
{
  return pvDestFileHash;
}

QString &mdtFileCopierItem::copyText()
{
  pvCopyText = "\n";
  pvCopyText += QObject::tr("Copying file ");
  pvCopyText += pvSrcFileName + "\n\n";
  pvCopyText += QObject::tr("From: \t");
  pvCopyText += pvSrcDirectoryPath + "\n";
  pvCopyText += QObject::tr("To:   \t");
  if(pvSrcFileName != pvDestFileName){
    pvCopyText += pvDestFilePath + "\n\n";
  }else{
    pvCopyText += pvDestDirectoryPath + "\n\n";
  }
  pvCopyText += QObject::tr("Sync after copy: ");
  // Add infos
  if(pvSyncAfterCopy){
    pvCopyText += QObject::tr("on");
  }else{
    pvCopyText += QObject::tr("off");
  }
  pvCopyText += "      ";
  pvCopyText += QObject::tr("Verify after copy: ");
  if(pvCheckAfterCopy){
    pvCopyText += QObject::tr("on");
  }else{
    pvCopyText += QObject::tr("off");
  }

  return pvCopyText;
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

