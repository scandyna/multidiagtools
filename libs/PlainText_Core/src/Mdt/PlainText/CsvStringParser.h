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
#ifndef MDT_PLAIN_TEXT_CSV_STRING_PARSER_H
#define MDT_PLAIN_TEXT_CSV_STRING_PARSER_H

#include "StringConstIterator.h"
#include "CsvParserSettings.h"
#include "Record.h"
#include "RecordList.h"
#include "Mdt/Expected.h"
#include "Mdt/Error.h"
#include <QString>
#include <memory>

namespace Mdt{ namespace PlainText{

  /*! \brief CSV parser that acts on a QString as input
   *
   * mdtCsvStringParser parses a CSV string.
   *  The CSV string can contain quoted section.
   *
   * \note Some part of this API documentation refers to following standards:
   *       \li CSV-1203 available here: http://mastpoint.com/csv-1203
   *       \li RFC 4180 available here: https://tools.ietf.org/html/rfc4180
   */
  class CsvStringParser
  {
   public:

    /*! \brief Default constructor
     */
    CsvStringParser();

    // Destructor: required so that mParser pointer has the definition for its destructor
    ~CsvStringParser();

    // Copy disabled
    CsvStringParser(const CsvStringParser &) = delete;
    CsvStringParser & operator=(const CsvStringParser &) = delete;
    // Move disabled
    CsvStringParser(CsvStringParser &&) = delete;
    CsvStringParser & operator=(CsvStringParser &&) = delete;

    /*! \brief Set CSV settings
     *
     * \pre \a settings must be valid
     */
    void setCsvSettings(const CsvParserSettings & settings);

    /*! \brief Set CSV source string
     *
     * \note Parsing will be done directly on source
     *       Make shure that it is not modified during parsing process
     *       (take a look at Qt documentation about STL style iterators invalidation for details)
     */
    void setSource(const QString & source);

    /*! \brief Check if parser is at end of the source
     */
    bool atEnd() const;

    /*! \brief Read one line
     *
     * If a end of line is in a quoted section,
     *  it will not be threated as end of line.
     *
     * This function can be used, f.ex.,
     *  to store a long string to some other
     *  storage device, without having to
     *  copy the entiere source before.
     *  Reading line by line can also
     *  save some memory.
     *
     * \pre CSV settings must be set before calling this function
     * \sa setCsvSettings()
     */
    Mdt::Expected<Record> readLine();

    /*! \brief Read the entire CSV string
    *
    * \pre CSV settings must be set before calling this function
    * \sa setCsvSettings()
    */
    Mdt::Expected<RecordList> readAll();

    /*! \brief Get last error
    */
    Mdt::Error lastError() const;

   private:

    StringConstIterator mCurrentPosition;
    StringConstIterator mEnd;
//     std::unique_ptr<CsvParserTemplate<StringConstIterator> > mParser;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_TEXT_CSV_STRING_PARSER_H
