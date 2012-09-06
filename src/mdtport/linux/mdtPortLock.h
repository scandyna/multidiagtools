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
#ifndef MDT_PORT_LOCK_H
#define MDT_PORT_LOCK_H

#include <QFileInfo>
#include <QFileInfoList>
#include <QString>
#include <QStringList>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

/// NOTE: \todo Implement UUCP format in LCK.. files
/*! \brief Open a port and lock it
 *
 * On Linux (or all POSIX ?) system, file locking is avaliable with fcntl().
 * But several applications uses LCK..name for locking.
 *
 * This class will try to find if a port is allready locked
 * using LCK..name system in several directories (/var/lock, /tmp, ...).
 *
 * If nothing is found, then port will be opened, and several lockfiles created.
 * The port is then locked with fcntl().
 */
class mdtPortLock
{
 public:

    mdtPortLock();
    ~mdtPortLock();

  /*! \brief Open a port locked
   * 
   * At first, the lock file name will be buit for
   * each lock directories found (searching in /tmp, /var/lock).
   * Then, if no lock file is found, the port will be opened with open().
   * Just after open, the port will be locked with fcntl().
   *
   * \param portName Path to port to open (f.ex. /dev/ttyS0)
   * \param flags Flags are simply passed to open() syscall (see open() man for details).
   * \return File descriptor returned by open() , or -1 on error.
   */
  int openLocked(const QString & portName, int flags);

  /*! \brief Unlock the port
   * 
   * Will remove lockfiles and unlock port
   *  with fcntl().
   *
   * Note: use close() if file is not used later, this is not done by this method.
   */
  void unlock();

  /*! \brief Returns true if port is locked by current instance.
   */
  bool isLocked();

  /*! \brief Returns true if port is locked by another instance.
   */
  bool isLockedByAnother();

 private:

  // Search in several directories that could contain
  // lockfiles (/var/lock , /tmp , ...)
  // All writeble directories are appended to pvWritableLockDirectories
  void scanForLockDirectories();

  // Build the lock file name reagrding pvPort name.
  // Then, build a lockfile for each lock directories found.
  void buildLockFilesList();

  // Create a lockfile
  bool createLockFile(const QFileInfo &file);

  // Remove created lock files
  void removeLockFiles();

  QFileInfo pvPort;                       // Port to lock
  int pvFd;                               // Port file descriptor
  QFileInfoList pvLockfiles;              // Lockfiles in realtion to pvPort
  QFileInfoList pvCreatedLockFiles;       // Lockfiles that where created
  QStringList pvWritableLockDirectories;  // Directories for lockfiles
  bool pvIsLocked;                        // Internall lock flag
  bool pvIsLockedByAnother;               // Internall lock flag
  struct flock pvLock;                    // Internall flock struct
};

#endif  // #ifndef MDT_PORT_LOCK_H
