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
#include "mdtError.h"
#include "windows.h"
#include <QDir>
#include <QFileInfo>
#include <QByteArray>

//#include <QDebug>

bool mdtPartitionAttributes::setPath(const QString &path)
{
  char driveName[1024];
  char fs[256];
  const char *drive;
  int flags;
  QByteArray strPath;

  // Default values
  pvRootPath = "";
  pvName = "";
  pvIsReadOnly = false;
  pvFileSystem = "";

  // Get Windows native format of given path
  strPath = QDir::toNativeSeparators(path).toLocal8Bit();
  // See if the terminal \ exists
  if(strPath.size() == 2){
    strPath.append('\\');
  }
  drive = strPath.data();
  // Get attributes
  if(GetVolumeInformation(drive, driveName, 1023, 0, 0, (LPDWORD)&flags, fs, 255) == 0){
    mdtError e(MDT_FILE_IO_ERROR, "GetVolumeInformation() failed", mdtError::Error);
    e.setSystemErrorWinApi();
    MDT_ERROR_SET_SRC(e, "mdtPartitionAttributes");
    e.commit();
    return false;
  }
  // Get flags
  if(flags & FILE_READ_ONLY_VOLUME){
    pvIsReadOnly = true;
  }else{
    pvIsReadOnly = false;
  }
  // Store attributes
  pvRootPath = QDir::fromNativeSeparators(strPath);
  pvName = driveName;
  pvFileSystem = fs;

  return true;
}

QStringList mdtPartitionAttributes::availablePartitions(const QStringList &ignoreList)
{
  QStringList partitionsList;
  QFileInfoList infoList;
  int i;

  infoList = QDir::drives();
  for(i=0; i<infoList.size(); i++){
    // Add only if not in ignoreList
    if(!ignoreList.contains(infoList.at(i).absoluteFilePath(), Qt::CaseInsensitive)){
      partitionsList << infoList.at(i).absoluteFilePath();
    }
  }

  return partitionsList;
}
