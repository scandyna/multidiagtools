/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_PLAIN_FILE_READER_H
#define MDT_PLAIN_FILE_READER_H

#include "Mdt/Error.h"
#include <QByteArray>
#include <QString>

namespace Mdt{ namespace PlainText{

  /*! \brief FileReader is a helper class to read a file
   *
   * Reading a file can be done easily by using QFile and QTextStream,
   *  but adding some error checking introduces additionnal code every time.
   *  File reader does some checks for common cases and fills a error,
   *  avaibale in lastError().
   *
   * Typical usage:
   * \code
   * #include <Mdt/PlainText/FileReader.h>
   * #include <Mdt/ErrorDialog.h>
   *
   * using Mdt::PlainText::FileReader;
   * using Mdt::ErrorDialog;
   *
   * // In your function
   * FileReader reader;
   * QString filePath = "/some/path/to/file.txt";
   *
   * if(!reader.readFile(filePath, "UTF-8",2*1024*1024)){
   *   ErrorDialog dialog(reader.lastError());
   *   dialog.exec();
   *   return;
   * }
   * \endcode
   */
  class FileReader
  {
   public:

    /*! \brief EOL mode
     */
    enum EolMode
    {
      TextMode,   /*!< While reading, end of lines are translated to '\n'. */
      BinaryMode  /*!< While reading, the end of line are left as they are. */
    };

    /*! \brief Read a entire file
     */
    bool readFile(const QString & filePath, const QByteArray & encoding, qint64 maxSize, EolMode eolMode = TextMode);

    /*! \brief Get string data
     */
    QString stringData() const
    {
      return mStringData;
    }

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

    /*! \brief Set auto commit of errors
     *
     * If \a autoCommit is true,
     *  a error that occurs will be committed,
     *  else not.
     *  By default, error is not commited automatically.
     *
     * \sa Mdt::Error::commit()
     */
    void setAutoCommitError(bool autoCommit);

   private:

    bool mAutoCommitError = false;
    QString mStringData;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_FILE_READER_H
