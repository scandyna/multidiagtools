/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_PLAIN_TEXT_CSV_FILE_PARSER_H
#define MDT_PLAIN_TEXT_CSV_FILE_PARSER_H

#include "CsvParserSettings.h"
#include "StringRecord.h"
#include "StringRecordList.h"
#include "FileInputIterator.h"
#include "FileMultiPassIterator.h"
#include "Mdt/Expected.h"
#include "Mdt/Error.h"
#include <QString>
#include <QByteArray>
#include <QFileInfo>
#include <QFile>
#include <memory>

namespace Mdt{ namespace PlainText{

  template <typename InputIterator>
  class CsvParserTemplate;

  /*! \brief CSV parser that acts on a file as input
   *
   * \note Some part of this API documentation refers to following standards:
   *       \li CSV-1203 available here: http://mastpoint.com/csv-1203
   *       \li RFC 4180 available here: https://tools.ietf.org/html/rfc4180
   */
  class CsvFileParser
  {
   public:

    /*! \brief Default constructor
     */
    CsvFileParser();

    // Destructor: required so that mParser pointer has the definition for its destructor
    ~CsvFileParser();

    // Copy disabled
    CsvFileParser(const CsvFileParser &) = delete;
    CsvFileParser & operator=(const CsvFileParser &) = delete;
    // Move disabled
    CsvFileParser(CsvFileParser &&) = delete;
    CsvFileParser & operator=(CsvFileParser &&) = delete;

    /*! \brief Set CSV settings
     *
     * \pre \a settings must be valid
     * \pre No file must currently be open
     */
    void setCsvSettings(const CsvParserSettings & settings);

    /*! \brief Open CSV file
     *
     * \param fileInfo Path to CSV file that must be open
     * \param encoding Encoding name of the CSV file format.
     *               Will use QTextCodec::codecForName() to get apropriate codec.
     * \return false if CSV file could not be open, or no codec could be found for given encoding,
     *         true if all goes well.
     */
    bool openFile(const QFileInfo & fileInfo, const QByteArray & encoding);

    /*! \brief Check if a CSV file is open
     */
    bool isOpen() const;

    /*! \brief Close CSV file
     */
    void closeFile();

    /*! \brief Check about end of file
     *
     * If no file is open, this function returns allways true
     */
    bool atEnd() const;

    /*! \brief Read one line in CSV file
     *
     * \pre CSV settings must be set before calling this function
     * \sa setCsvSettings()
     */
    Mdt::Expected<StringRecord> readLine();

    /*! \brief Read the entire CSV file
     *
     * \pre CSV settings must be set before calling this function
     * \sa setCsvSettings()
     */
    Mdt::Expected<StringRecordList> readAll();

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

   private:

    QFile mFile;
    FileInputIterator mFileIterator;
    std::unique_ptr< CsvParserTemplate<FileMultiPassIterator> > mParser;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_TEXT_CSV_FILE_PARSER_H
