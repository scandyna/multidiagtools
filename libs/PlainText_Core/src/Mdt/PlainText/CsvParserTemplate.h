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
#ifndef MDT_PLAIN_TEXT_CSV_PARSER_TEMPLATE_H
#define MDT_PLAIN_TEXT_CSV_PARSER_TEMPLATE_H

// Uncomment this line to enable parser debuging
// #define BOOST_SPIRIT_DEBUG

// #include <QDebug>
// #include <iostream>

#include "CsvParserSettings.h"
#include "Record.h"
#include "BoostSpiritQtTraits.h"
#include "Mdt/Error.h"
#include "Mdt/Expected.h"
#include <boost/spirit/include/qi.hpp>
#include <QChar>
#include <QString>
#include <QVariant>
#include <QVector>
#include <string>
#include <vector>

namespace Mdt{ namespace PlainText{

  /*! \brief CSV parser template
   *
   * This class implements the parser logic,
   *  and can act on different input containers
   *  that have STL compatible iterators.
   *
   * \tparam SourceIterator Type of iterator that will act on the source.
   * \note Including directly this header in a project can slow down compilation time
   * \sa CsvStringParser
   * \sa CsvFileParser
   * \note Some part of this API documentation refers to following standards:
   *       \li CSV-1203 available here: http://mastpoint.com/csv-1203
   *       \li RFC 4180 available here: https://tools.ietf.org/html/rfc4180
   */
  template <typename SourceIterator>
  class CsvParserTemplate
  {
   public:

    /*! \brief Default constructor
     */
    CsvParserTemplate() = default;

    // Copy disabled
    CsvParserTemplate(const CsvParserTemplate &) = delete;
    CsvParserTemplate & operator=(const CsvParserTemplate &) = delete;
    // Move disabled
    CsvParserTemplate(CsvParserTemplate &&) = delete;
    CsvParserTemplate & operator=(CsvParserTemplate &&) = delete;

   private:

    
  };

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_TEXT_CSV_PARSER_TEMPLATE_H
