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

/*! \brief
 */
class mdtFileCopierItem
{
 public:

  mdtFileCopierItem(const QString & srcFilePath, const QString & destFilePath);
  ~mdtFileCopierItem();

  /*! \brief
   */
  void setFileSize(qint64 size);

  /*! \brief
   */
  void setSrcFilePath(const QString & filePath);

  /*! \brief
   */
  QString &srcFilePath();

  /*! \brief
   */
  void setSrcFileName(const QString & fileName);

  /*! \brief
   */
  QString &srcFileName();

  /*! \brief
   */
  void setSrcFileHash(const QByteArray &hash);

  /*! \brief
   */
  QByteArray &srcFileHash();

  /*! \brief
   */
  qint64 fileSize();

  /*! \brief
   */
  void setDestFilePath(const QString & filePath);

  /*! \brief
   */
  QString &destFilePath();

  /*! \brief
   */
  void setDestFileHash(const QByteArray &hash);

  /*! \brief
   */
  QByteArray &destFileHash();

  /*! \brief
   */
  void setSyncAfterCopy(bool sync);

  /*! \brief
   */
  bool syncAfterCopy();

  /*! \brief
   */
  void setCheckAfterCopy(bool check);

  /*! \brief
   */
  bool checkAfterCopy();

 private:
  
  QString pvSrcFilePath;      // Path to source file
  QString pvSrcFileName;      // Source file name (without directory path)
  QByteArray pvSrcFileHash;   // Hash of file (SHA or else)
  QString pvDestFilePath;
  QByteArray pvDestFileHash;  // Hash of file (SHA or else)
  qint64 pvFileSize;
  bool pvSyncAfterCopy;
  bool pvCheckAfterCopy;
};

#endif  // #ifndef MDT_FILE_COPIER_ITEM_H
