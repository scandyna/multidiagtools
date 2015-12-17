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
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <mntent.h>
#include <sys/types.h>
#include <sys/statvfs.h>

//#include <QDebug>

bool mdtPartitionAttributes::setPath(const QString &path)
{
  FILE *fd;
  struct mntent *fs;
  struct statvfs vfs;
  QString mntDir;

  // Default values
  pvRootPath = "";
  pvName = "";
  pvIsReadOnly = false;
  pvFileSystem = "";

  // Open mtab FIXME: probably not POSIX, but only Linux ?
  fd = setmntent("/etc/mtab", "r");
  if(fd == 0){
    pvLastError.setError("Unable to open /mnt/mtab", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtPartitionAttributes");
    auto sysError = mdtErrorNewT(int, errno, strerror(errno), mdtError::Error, "mdtPartitionAttributes");
    sysError.commit();
    pvLastError.stackError(sysError);
    pvLastError.commit();
    return false;
  }
  // Travel mounted partitions and get attributes
  while((fs = getmntent(fd)) != 0){
    if(fs->mnt_fsname[0] == '/'){
      mntDir = fs->mnt_dir;
      if(mntDir == path){
        // Store attributes
        pvRootPath = fs->mnt_dir;
        pvName = fs->mnt_fsname;
        pvFileSystem = fs->mnt_type;
        // Get flags
        if(statvfs(fs->mnt_dir, &vfs) != 0){
          pvLastError.setError("statvfs() failed on " + pvRootPath, mdtError::Error);
          MDT_ERROR_SET_SRC(pvLastError, "mdtPartitionAttributes");
          auto sysError = mdtErrorNewT(int, errno, strerror(errno), mdtError::Error, "mdtPartitionAttributes");
          sysError.commit();
          pvLastError.stackError(sysError);
          pvLastError.commit();
          endmntent(fd);
          return false;
        }
        if(vfs.f_flag & ST_RDONLY){
          pvIsReadOnly = true;
        }else{
          pvIsReadOnly = false;
        }
        // End of attributes fetch - cleanup and return
        endmntent(fd);
        return true;
      }
    }
  }

  // Close mtab
  endmntent(fd);

  return false;
}

QStringList mdtPartitionAttributes::availablePartitions(const QStringList &ignoreList)
{
  QStringList partitionsList;
  FILE *fd;
  struct mntent *fs;

  // Open mtab FIXME: probably not POSIX, but only Linux ?
  fd = setmntent("/etc/mtab", "r");
  if(fd == 0){
    auto error = mdtErrorNew("Unable to open /mnt/mtab", mdtError::Error, "mdtPartitionAttributes");
    auto sysError = mdtErrorNewT(int, errno, strerror(errno), mdtError::Error, "mdtPartitionAttributes");
    sysError.commit();
    error.stackError(sysError);
    error.commit();
    return partitionsList;
  }
  // Travel mounted partitions
  while((fs = getmntent(fd)) != 0){
    // We ignore "virtual" FS , like sysfs
    if(fs->mnt_fsname[0] == '/'){
      // Add only if not in ignoreList
      if(!ignoreList.contains(fs->mnt_dir)){
        partitionsList << fs->mnt_dir;
      }
    }
  }

  // Close mtab
  endmntent(fd);

  return partitionsList;
}
