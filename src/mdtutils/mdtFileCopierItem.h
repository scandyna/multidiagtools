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
#ifndef MDT_FILE_COPIER_ITEM_H
#define MDT_FILE_COPIER_ITEM_H

#include <QByteArray>
#include <QString>

/*! \brief This is a container class used by mdtFileCopier
 */
class mdtFileCopierItem
{
 public:

  mdtFileCopierItem(const QString & srcFilePath, const QString & destFilePath);
  ~mdtFileCopierItem();

  /*! \brief Set the source file path
   */
  void setSrcFilePath(const QString & filePath);

  /*! \brief Get the source file path
   */
  QString &srcFilePath();

  /*! \brief Set the source file name (without directory path)
   */
  void setSrcFileName(const QString & fileName);

  /*! \brief Get the source file name (without directory path)
   */
  QString &srcFileName();

  /*! \brief Set the source file'directory path
   */
  void setSrcDirectoryPath(const QString & directoryPath);

  /*! \brief Get the source file path
   */
  QString &srcDirectoryPath();

  /*! \brief Set the source file hash
   */
  void setSrcFileHash(const QByteArray &hash);

  /*! \brief Get the source file hash
   */
  QByteArray &srcFileHash();

  /*! \brief Set the file size
   */
  void setFileSize(qint64 size);

  /*! \brief Get the file size
   */
  qint64 fileSize();

  /*! \brief Set the destination file path
   */
  void setDestFilePath(const QString & filePath);

  /*! \brief Get the destination file path
   */
  QString &destFilePath();

  /*! \brief Set the destination file name (without directory path)
   */
  void setDestFileName(const QString & fileName);

  /*! \brief Get the destination file name (without directory path)
   */
  QString &destFileName();

  /*! \brief Set the destination file'directory path
   */
  void setDestDirectoryPath(const QString & directoryPath);

  /*! \brief Get the source file path
   */
  QString &destDirectoryPath();

  /*! \brief Set the destination file hash
   */
  void setDestFileHash(const QByteArray &hash);

  /*! \brief Get the destination file hash
   */
  QByteArray &destFileHash();

  /*! \brief Get text to display in progress dialog
   */
  QString &copyText();

  /*! \brief Set sync afetr copy flag
   * 
   * If sync is true, fsync() (or OS equivalent) will
   *  be called after file copy.
   */
  void setSyncAfterCopy(bool sync);

  /*! \brief Get sync afetr copy flag
   * 
   * \sa setSyncAfterCopy()
   */
  bool syncAfterCopy();

  /*! \brief Set the check after copy flag
   * 
   * If check is true, source and destination hash
   *  will be calculated (with SHA-1), and compared.
   *  If they don't match, the copy will be considered
   *  as failed.
   */
  void setCheckAfterCopy(bool check);

  /*! \brief Get the check after copy flag
   * 
   * \sa setCheckAfterCopy()
   */
  bool checkAfterCopy();

 private:
  
  QString pvSrcFilePath;      // Path to source file
  QString pvSrcFileName;      // Source file name (without directory path)
  QString pvSrcDirectoryPath; // Path of source file's directory
  QByteArray pvSrcFileHash;   // Hash of file (SHA or else)
  QString pvDestFilePath;
  QString pvDestFileName;     // Destination file name (without directory path)
  QString pvDestDirectoryPath;// Path of destination file's directory
  QByteArray pvDestFileHash;  // Hash of file (SHA or else)
  qint64 pvFileSize;
  QString pvCopyText;         // Text that will be displayed in progress dialog
  bool pvSyncAfterCopy;
  bool pvCheckAfterCopy;
};

#endif  // #ifndef MDT_FILE_COPIER_ITEM_H
