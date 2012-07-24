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
#include "mdtPortLock.h"
#include "mdtError.h"
#include <QDir>
#include <QFile>
#include <errno.h>
#include <string.h>

//#include <QDebug>

mdtPortLock::mdtPortLock()
{
  // Find lock directories
  scanForLockDirectories();
  pvIsLocked = false;
  pvFd = -1;
}

mdtPortLock::~mdtPortLock()
{
  unlock();
}

int mdtPortLock::openLocked(const QString & portName, int flags)
{
  int i;

  pvPort.setFile(portName);

  // Search about lock files
  buildLockFilesList();
  for(i=0; i<pvLockfiles.size(); i++){
    if(pvLockfiles.at(i).exists()){
      mdtError e(MDT_PORT_IO_ERROR , "Port " + pvPort.absoluteFilePath() + " is locked (Lockfile" + pvLockfiles.at(i).filePath() + " exists)", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtPortLock");
      e.commit();
      /// NOTE: \todo Flag pvIsLocked : could be locked by another (other process, or other thread in this process), true seems not to be correct here !
      pvIsLocked = true;
      return -1;
    }
  }
  // No lock files found, create all
  for(i=0; i<pvLockfiles.size(); i++){
    if(!createLockFile(pvLockfiles.at(i))){
      return -1;
    }
  }
  // Try to open the port
  pvFd = ::open(pvPort.absoluteFilePath().toStdString().c_str(), flags);
  if(pvFd < 0){
    mdtError e(MDT_PORT_IO_ERROR, "Unable to open " + pvPort.absoluteFilePath(), mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtPortLock");
    e.commit();
    removeLockFiles();
    /// NOTE: \todo Flag pvIsLocked ?
    return -1;
  }
  // Lock the port
  pvLock.l_type = F_WRLCK;
  pvLock.l_whence = SEEK_SET;
  pvLock.l_start = 0;
  pvLock.l_len = 0;
  if(fcntl(pvFd, F_SETLK, &pvLock) < 0){
    mdtError e(MDT_PORT_IO_ERROR, "Unable to lock " + pvPort.absoluteFilePath(), mdtError::Error);
    e.setSystemError(errno, strerror(errno));
    MDT_ERROR_SET_SRC(e, "mdtPortLock");
    e.commit();
    removeLockFiles();
    ::close(pvFd);
    pvFd = -1;
    /// NOTE: \todo Flag pvIsLocked ?
    return -1;
  }
  // Ok, port is open and locked here
  pvIsLocked = true;

  return pvFd;
}

void mdtPortLock::unlock()
{
  if(!pvIsLocked){
    return;
  }
  if(pvFd < 0){
    return;
  }
  // Unlock port and remove lock files
  fcntl(pvFd, F_UNLCK, &pvLock);
  removeLockFiles();
  pvIsLocked = false;
}

bool mdtPortLock::isLocked()
{
  return pvIsLocked;
}

bool mdtPortLock::isLockedByAnother()
{
  return pvIsLocked && (pvFd < 0);
}

void mdtPortLock::scanForLockDirectories()
{
  QDir dir;
  QFileInfo info;

  // Try /var/lock
  dir.setPath("/var/lock");
  if(dir.exists()){
    info.setFile(dir.path());
    if(info.isWritable()){
      pvWritableLockDirectories << dir.path();
    }
  }
  // Try /tmp
  dir.setPath("/tmp");
  if(dir.exists()){
    info.setFile(dir.path());
    if(info.isWritable()){
      pvWritableLockDirectories << dir.path();
    }
  }
  // Warn if no directory was found
  if(pvWritableLockDirectories.size() < 1){
    mdtError e(MDT_PORT_IO_ERROR, "No lock directory found", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortLock");
    e.commit();
  }
}

void mdtPortLock::buildLockFilesList()
{
  QString lockFileName = "LCK.." + pvPort.fileName();

  // Build the lock files list
  pvLockfiles.clear();
  for(int i=0; i<pvWritableLockDirectories.size(); i++){
    pvLockfiles << QFileInfo(pvWritableLockDirectories.at(i) + "/" + lockFileName);
  }
}

/// NOTE: it could happen that another process creates the file between f.exists() and f.open() ..
bool mdtPortLock::createLockFile(const QFileInfo &file)
{
  QFile f(file.absoluteFilePath());

  // Check if file exists
  if(f.exists()){
    mdtError e(MDT_PORT_IO_ERROR, "File " + f.fileName() + " exists", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortLock");
    e.commit();
    pvIsLocked = true;
    return false;
  }
  // Try to create
  if(!f.open(QIODevice::ReadWrite)){
    mdtError e(MDT_PORT_IO_ERROR, "File " + f.fileName() + " cannot be created", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortLock");
    e.commit();
    return false;
  }
  pvCreatedLockFiles << file;
  f.close();

  return true;
}

void mdtPortLock::removeLockFiles()
{
  QFile f;

  for(int i=0; i<pvCreatedLockFiles.size(); i++){
    f.setFileName(pvCreatedLockFiles.at(i).absoluteFilePath());
    if(!f.remove()){
      mdtError e(MDT_PORT_IO_ERROR, "Cannot remove lock file " + pvCreatedLockFiles.at(i).absoluteFilePath(), mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtPortLock");
      e.commit();
    }
  }
  pvCreatedLockFiles.clear();
}
