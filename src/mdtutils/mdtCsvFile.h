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

  /*! \brief Close
   *
   * Will do some cleanup and call QFile's close method.
   *
   * Note: readen data are not cleared. Call clear() explicitly for this.
   */
  void close();

  /*! \brief Read the file and store data
   * 
   * \param separator Separator to use (typical: ; )
   * \param protection Data protection (typical: " )
   * \param comment Comment (typical: # )
   * \return True on success, false else (see errorString() to know what happened in this case)
   * \pre separator, dataProtection and comment must not be the same
   */
  ///bool readLines(QByteArray separator = ";", QByteArray dataProtection = "", QByteArray comment = "#");

  /*! \brief Write a line of data
   *
   * \param line List of strings containing line data.
   * \param separator Separator to use (typical: ; )
   * \param dataProtection Data protection (typical: " ).
   *                        Note: current version does not support protected EOL.
   * \param escapeChar Escape char (typical: \ ).
   *                    If not empty, if a line item contains a same sequence than dataProtection will be prefixed with escapeChar.
   *                    Note: separator is not escaped.
   * \param eol End of line sequence. Usefull if given file is not supposed to be readen from running platform.
   *             Note that file must not be open with Text flag if this parameter is needed (se QFile::open() for details).
   * \return True on success
   */
  bool writeLine(const QStringList &line, const QString &separator = ";", const QString &dataProtection = "", const QChar &escapeChar = QChar(), QString eol = MDT_NATIVE_EOL);

  /*! \brief Read a line of data
   *
   * Note: do not use atEnd() to check if data are available anymore,
   *  but hasMoreLines().
   *
   * \param dataProtection Data protection (typical: " ).
   * \param comment Comment (typical: # ). Each line that beginns with comment is ignored.
   * \param escapeChar Escape char (typical: \ ). Note: has only effect to escape dataProtection.
   * \param eol End of line sequence. Usefull if given file was not written from running platform.
   * \return Line of data.
   * \todo Add suport of commented lines
   */
  QByteArray readLine(const QString &dataProtection = "", const QString &comment = "#", const QChar &escapeChar = QChar(), QByteArray eol = MDT_NATIVE_EOL);

  /*! \brief Check if readLine() has more lines available
   */
  bool hasMoreLines() const;

  /*! \brief Read the file and store data
   *
   * \param separator Separator to use (typical: ; )
   * \param dataProtection Data protection (typical: " )
   *                        Note: current version does not support protected EOL.
   * \param escapeChar Escape char (typical: \ ). Note: has only effect to escape dataProtection.
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

  /*! \brief Set the readLine() method's buffer size
   *
   * Used by unit testing.
   *  A value < 1 means default value (8192).
   */
  void setReadLineBufferSize(int size);

 private:

  /*! \brief Check if a data protection section begins
   *
   * \param line Line of data to check about data protection beginning.
   * \param dataProtection Data protection (typical: " )
   * \param escapeChar Escape char (typical: \ ). Note: has only effect to escape dataProtection.
   */
  bool dataProtectionSectionBegins(const QByteArray &line, const QString &dataProtection, const QChar &escapeChar);

  /*! \brief Read until a data protection is reached
   *
   * \param buffer Buffer in witch data will be appended.
   *                Note that data can be leave after possibly found data protection.
   * \param dpIndex Index of found data protection. Will be -1 if never found.
   * \param dataProtection Data protection (typical: " )
   * \param escapeChar Escape char (typical: \ ). Note: has only effect to escape dataProtection.
   */
  void readUntilDataProtection(QByteArray &buffer, int &dpIndex, const QString &dataProtection, const QChar &escapeChar);

  /*! \brief Get index of the beginning of a commented line
   *
   * Example of buffer containing a start of a commented line (with # as comment):
   *  - #ABCD
   *  - \n#ABCD
   *  - \\n #ABCD
   *  - ABCD\n#EFGH
   *  - ABCD\n #EFGH
   * Example of buffer not containing a start of a commented line (with # as comment):
   *  - "#ABCD
   *  - A#BCD
   *  - \\nA#BCD
   *
   * \param line Line to parse
   * \param comment Comment (typical: # )
   * \return Index of the beginning of a commented line, or -1 if not found.
   */
  int indexOfCommentedLineBeginning(const QByteArray &line, const QString &comment);

  QList<QStringList> pvLines;
  ///QStringList pvFields;
  QTextCodec *pvCodec;
  QByteArray pvReadLineBuffer;  // Used by readLine() to store what is after EOL
  int pvReadLineBufferSize;     // Used by readLine()
  /// \todo Overload open() to clear this buffer !
};

#endif  // #ifndef MDT_CSV_FILE_H
