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
#ifndef MDT_CSV_PARSER_H
#define MDT_CSV_PARSER_H

#include "mdtCsvSettings.h"
#include "mdtCsvData.h"
#include "mdtCsvParserIterator.h"
#include <QString>
#include <memory>

#include <string>

template <typename InputIterator>
class mdtCsvParserTemplate;

/*! \brief CSV parser that acts on a QString as input
 */
class mdtCsvStringParser
{
 public:

  /*! \brief Default constructor
   */
  mdtCsvStringParser();

  /*! \brief Destructor
   */
  ~mdtCsvStringParser();

  /*! \internal Copy disabled
   */
  mdtCsvStringParser(const mdtCsvStringParser &) = delete;

  /*! \brief Set settings
   */
  void setSettings(const mdtCsvParserSettings & s);

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
   */
  mdtCsvRecord readLine();

 private:

  ///std::unique_ptr<mdtCsvParserTemplate<std::wstring::const_iterator> > pvImpl;
  std::unique_ptr<mdtCsvParserTemplate<mdtCsvParserQStringIterator> > pvImpl;
  ///std::wstring pvSource;
};

/// \todo Sandbox
// void mdtReadIteratorTestFunction();

#endif // #ifndef MDT_CSV_PARSER_H
