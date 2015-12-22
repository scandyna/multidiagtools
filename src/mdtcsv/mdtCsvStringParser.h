/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_CSV_STRING_PARSER_H
#define MDT_CSV_STRING_PARSER_H

#include "mdtError.h"
#include "mdtExpected.h"
#include "mdtCsvSettings.h"
#include "mdtCsvData.h"
#include "mdtCsvStringParserIterator.h"
#include <QString>
#include <memory>

template <typename InputIterator>
class mdtCsvParserTemplate;

/*! \brief CSV parser that acts on a QString as input
 *
 * mdtCsvStringParser parses a CSV string.
 *  The CSV string can contain quoted section.
 *
 * \note Some part of this API documentation refers to following standards:
 *       \li CSV-1203 available here: http://mastpoint.com/csv-1203
 *       \li RFC 4180 available here: https://tools.ietf.org/html/rfc4180
 */
class mdtCsvStringParser
{
 public:

  /*! \brief Default constructor
   *
   * \pre csvSettings must be valid
   */
  mdtCsvStringParser(const mdtCsvParserSettings & csvSettings = mdtCsvParserSettings());

  // Destructor: required so that pvParser pointer has the definition for its destructor
  ~mdtCsvStringParser();

  /*! \brief Copy disabled
   */
  mdtCsvStringParser(const mdtCsvStringParser &) = delete;

  /*! \brief Assignement disabled
   */
  mdtCsvStringParser & operator=(const mdtCsvStringParser &) = delete;

  /*! \brief Set CSV source string
   *
   * \note Parsing will be done directly on source
   *       Make shure that it is not detached during parsing process
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
   */
  mdtExpected<mdtCsvRecord> readLine();

  /*! \brief Read the entire CSV string
   */
  mdtExpected<mdtCsvData> readAll();

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  mdtCsvStringParserIterator pvCurrentPosition;
  mdtCsvStringParserIterator pvEnd;
  std::unique_ptr<mdtCsvParserTemplate<mdtCsvStringParserIterator> > pvImpl;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CSV_STRING_PARSER_H
