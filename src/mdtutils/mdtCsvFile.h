/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include <QChar>
#include <QTextCodec>
#include <QStringList>
#include <QList>
#include <QFile>

// Set OS native End Of Line sequence
#ifdef Q_OS_WIN
 #define MDT_NATIVE_EOL "\r\n"
#else
 #define MDT_NATIVE_EOL "\n"
#endif

/*! \brief Read and write CSV file
 * 
 * The default file encoding format is assumed UTF-8.
 * If another format is to use, give it at constructor.
 *
 * \todo Create a write from CSV method, and considere EOL problem.
 */
class mdtCsvFile : public QFile
{
 public:

  /*! \brief Constructor.
   *
   * \param fileEncoding File encoding format. Note that this does not affect QFile's methods.
   */
  mdtCsvFile(QObject *parent = 0, const QByteArray &fileEncoding = "UTF-8");
  ~mdtCsvFile();

  /*! \brief Read the file and store data
   * 
   * \param separator Separator to use (typical: ; )
   * \param protection Data protection (typical: " )
   * \param comment Comment (typical: # )
   * \return True on success, false else (see errorString() to know what happened in this case)
   * \pre separator, dataProtection and comment must not be the same
   */
  ///bool readLines(QByteArray separator = ";", QByteArray dataProtection = "", QByteArray comment = "#");

  /*! \brief Read the file and store data
   *
   * \param separator Separator to use (typical: ; )
   * \param dataProtection Data protection (typical: " )
   *                        Note: current version does not support protected EOL.
   * \param escapeChar Escape char (typical: \ )
   * \param comment Comment (typical: # )
   * \param eol End of line sequence. Usefull if given file was not written from running platform.
   *             Note that file must not be open with Text flag if this parameter is needed (se QFile::open() for details).
   * \return True on success
   * \pre separator, dataProtection and comment must not be the same
   * \todo Check if precondition is needed (see mdtAlgorithm).
   * \todo Fix problem with protected EOL (must also modify mdtAlgorithm)
   */
  bool readLines(const QString &separator = ";", const QString &dataProtection = "", const QString &comment = "#", const QChar &escapeChar = QChar(), QString eol = MDT_NATIVE_EOL);

  /*! \brief Parse a string (considered as a line in a CSV file)
   * 
   * \param separator Separator to use (typical: ; )
   * \param protection Data protection (typical: " )
   * \return The list of found fields
   * 
   * \todo Check new function in mdtAlgorithm
   */
  ///QStringList &parseLine(const QByteArray &line, const QByteArray &separator, const QByteArray &dataProtection);

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
  ///QStringList pvFields;
  QTextCodec *pvCodec;
};

#endif  // #ifndef MDT_CSV_FILE_H
