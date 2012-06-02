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
#ifndef MDT_PARTITION_ATTRIBUTES_H
#define MDT_PARTITION_ATTRIBUTES_H

#include <QString>
#include <QStringList>

/*! \brief Get some attributes of a file system partition
 * 
 * Note: this class is not thread safe
 */
class mdtPartitionAttributes
{
 public:

  mdtPartitionAttributes();
  ~mdtPartitionAttributes();

  /*! \brief Set the path of partition on wich the infos must be retreived
   * 
   * On POSIX, path could be / , /home, ...<br>
   * On Windows, it can be C:\ , D:\.
   * \return False if partition attributes could not be fetsched.
   */
  bool setPath(const QString &path);

  /*! \brief Get the root path
   * 
   * For example, if /usr is on partition / , and /usr was passed
   *  to setPath() , this method will return /<br>
   * Similar on Wondows, if C:\something was passed to setPath() , C:/ is returned.<br>
   * On Windows, if nativeFormat is true, the \ separator is used, else, POSIX / separator
   *  is used in returned path.
   */
  QString rootPath(bool nativeFormat = false);

  /*! \brief Get the partition name
   * 
   * FIXME: clarify this
   */
  QString &name();

  /*! \brief Returns true if partition is read only
   */
  bool isReadOnly();

  /*!\brief Returns true if partition is writeable
   */
  bool isWriteable();

  /*! \brief Returns the name of partition filesystem.
   * 
   * Filesystem name could be ext3 , NTFS, ...
   */
  QString &fileSystem();

  /*! \brief Get the list of system's available partitions
   * 
   * On Linux, this return mounted partitions (reading /etc/mtab)<br>
   *  Note: could be fixed in future, when I understand something in DBus :)<br>
   * On Windows, returns the paths geven by QDir::drives()
   */
  QStringList availablePartitions();

 private:

  QString pvRootPath;
  QString pvName;
  bool pvIsReadOnly;
  QString pvFileSystem;
};

#endif  // #ifndef MDT_PARTITION_ATTRIBUTES_H
