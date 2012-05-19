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
#ifndef MDT_CSV_FILE_H
#define MDT_CSV_FILE_H

#include <QString>
#include <QTextCodec>
#include <QStringList>
#include <QList>
#include <QFile>

/*! \brief Read and write CSV file
 * 
 * The default file encoding format is assumed UTF-8.
 * If another format is to use, give it at constructor.
 */
class mdtCsvFile : public QFile
{
 public:

  /*! \brief Constructor.
   */
  mdtCsvFile(QObject *parent = 0, QByteArray fileEncoding = "UTF-8");
  ~mdtCsvFile();

  /*! \brief Read the file and store data
   * 
   * \param separator Separator to use (typical: ; )
   * \param protection Data protection (typical: " )
   * \param comment Comment (typical: # )
   * \return True on success, false else (see errorString() to know what happened in this case)
   * \pre separator, dataProtection and comment must not be the same
   */
  bool readLines(QByteArray separator = ";", QByteArray dataProtection = "", QByteArray comment = "#");

  /*! \brief Parse a string (considered as a line in a CSV file)
   * 
   * \param separator Separator to use (typical: ; )
   * \param protection Data protection (typical: " )
   * \return The list of found fields
   */
  QStringList &parseLine(const QByteArray &line, const QByteArray &separator, const QByteArray &dataProtection);

  /*! \brief Clear readen data
   */
  void clear();

  /*! \brief Get a value
   * 
   * No value is available before readLines() was called.
   * If requested index (line, column) is empty, or does not exists, a empty string is returned.
   * The index check is done internally.
   */
  QString valueAt(int line, int column);

  /*! \brief Get content of entire file
   */
  QList<QStringList> &lines();

 private:

  QList<QStringList> pvLines;
  QStringList pvFields;
  QTextCodec *pvCodec;
};

#endif  // #ifndef MDT_CSV_FILE_H
