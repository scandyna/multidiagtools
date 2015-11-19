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
#ifndef MDT_CSV_PARSER_TEMPLATE_H
#define MDT_CSV_PARSER_TEMPLATE_H

#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_DEBUG_TRACENODE
//#define BOOST_SPIRIT_DEBUG_PRINT_SOME 80


#include "mdtCsvSettings.h"
#include "mdtCsvData.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/bind.hpp>
#include <string>
///#include <vector>

#include <iostream>
#include <QDebug>

/*! \brief CSV parser template
 *
 * This class implements the parser logic,
 *  and can act on different input containers
 *  that have STL compatible iterators.
 *
 * \tparam SourceIterator Type of iterator that will act on the source.
 * \note Including directly this header in a project can slow down compilation time
 * \sa mdtCsvStringParser
 * \note Some part of this API documentation
 *       refers to CSV-1203 standard.
 *       CSV-1203 is a open standard available here: http://mastpoint.com/csv-1203
 */
template <typename SourceIterator>
class mdtCsvParserTemplate
{
 public:

  /*! \brief Default constructor
   */
  mdtCsvParserTemplate()
  {
    using boost::spirit::qi::char_;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::eol;
    using boost::spirit::ascii::space;
    
//     auto fieldSep = pvSettings.fieldSeparator;
//     auto fieldQuote = pvSettings.fieldProtection;
    /// \todo adapter settings si ok
    char fieldSep = ',';
    char fieldQuote = '\"';
    
//     char CR = 0x0D;
//     char LF = 0x0A;
//     char TAB = 0x09;
    /*
     * Build the grammar
     * Sources:
     *  - CSV-1203
     *  - http://stackoverflow.com/questions/7436481/how-to-make-my-split-work-only-on-one-real-line-and-be-capable-to-skip-quoted-pa/7462539#7462539
     */
    pvRecordRule = pvRecordPayload >> eol;
    pvRecordPayload %= pvFieldColumn % char_(fieldSep);
    pvFieldColumn =  pvUnprotectedField | pvProtectedField;
    ///pvProtectedField = char_(fieldQuote) >> pvFieldPayload >> char_(fieldQuote);
    pvProtectedField = lit(fieldQuote) >> pvFieldPayload >> lit(fieldQuote);
    pvFieldPayload = +pvAnychar;
    pvUnprotectedField %= pvRawFieldPayload;
    pvRawFieldPayload = pvSafechar | (pvSafechar >> *char_ >> pvSafechar);
    pvAnychar = pvChar | char_(fieldSep) | (char_(fieldQuote) >> char_(fieldQuote)) | space; // space matches space, CR, LF and other See std::isspace()
    pvChar = pvSafechar | char_(0x20);  // 0x20 == SPACE char
    //pvSafechar = char_(0x21, 0xFF) - lit(fieldSep) - lit(fieldQuote); 
    ///pvSafechar %= char_(0x21, 0xFF);
    pvSafechar = char_(0x21, 0x7F) - char_(fieldSep) - char_(fieldQuote);
    ///pvSafechar = char_("0-9a-zA-Z");
    
    ///pvRecordPayload %= pvFieldPayload % lit(fieldSep);
    ///pvUnprotectedField %= pvFieldPayload - eol - lit(fieldSep);
    ///pvFieldPayload = *(char_ - eol - lit(fieldSep) - lit(fieldQuote));
    
    
//     BOOST_SPIRIT_DEBUG_NODE(pvRecordRule);
//     BOOST_SPIRIT_DEBUG_NODE(pvRecordPayload);
    BOOST_SPIRIT_DEBUG_NODE(pvFieldColumn);
//     BOOST_SPIRIT_DEBUG_NODE(pvProtectedField);
//     BOOST_SPIRIT_DEBUG_NODE(pvUnprotectedField);
//     BOOST_SPIRIT_DEBUG_NODE(pvFieldPayload);
//     BOOST_SPIRIT_DEBUG_NODE(pvRawFieldPayload);
//     BOOST_SPIRIT_DEBUG_NODE(pvAnychar);
//     BOOST_SPIRIT_DEBUG_NODE(pvChar);
//     BOOST_SPIRIT_DEBUG_NODE(pvSafechar);
  }

  /*! \brief Destructor
   */
  ~mdtCsvParserTemplate()
  {
  }

  /*! \internal Copy disabled
   */
  mdtCsvParserTemplate(const mdtCsvParserTemplate &) = delete;

  /*! \brief Set source
   */
  void setSource(SourceIterator begin, SourceIterator end)
  {
    pvCurrentSourcePosition = begin;
    pvSourceEnd = end;
  }

  /*! \brief Check if parser is at end of the source
   */
  bool atEnd() const
  {
    if(pvCurrentSourcePosition == pvSourceEnd){
      return true;
    }
    /// \todo Check about EOF
    
    return false;
  }

  /*! \brief Read one line
   */
  mdtCsvRecord readLine()
  {
    mdtCsvRecord record;
    using boost::spirit::qi::char_;
    using boost::phoenix::ref;
    ///using boost::spirit::ascii::space;

    // Special if we reached the end of source, or source is empty
    /// \todo Check if this should be a error or not
    if(pvCurrentSourcePosition == pvSourceEnd){
      return record;
    }
    // Parse a line
    bool ok = boost::spirit::qi::parse(pvCurrentSourcePosition, pvSourceEnd, pvRecordRule, record.rawColumnDataList);
    if(!ok){
      record.setErrorOccured();
      /// \todo Store error
    }

    qDebug() << "record: ";
    for(int col = 0; col < record.count(); ++col){
      qDebug() << QString::fromStdString(record.rawColumnDataList.at(col));
    }

    return record;
  }

 private:

  SourceIterator pvCurrentSourcePosition;
  SourceIterator pvSourceEnd;
  boost::spirit::qi::rule<SourceIterator, QVector<std::string>()> pvRecordRule;
  boost::spirit::qi::rule<SourceIterator, QVector<std::string>()> pvRecordPayload;
  boost::spirit::qi::rule<SourceIterator, std::string()> pvFieldColumn;
  boost::spirit::qi::rule<SourceIterator, std::string()> pvProtectedField;
  boost::spirit::qi::rule<SourceIterator, std::string()> pvUnprotectedField;
  boost::spirit::qi::rule<SourceIterator, std::string()> pvFieldPayload;
  boost::spirit::qi::rule<SourceIterator, std::string()> pvRawFieldPayload;
  boost::spirit::qi::rule<SourceIterator, char()> pvAnychar;
  boost::spirit::qi::rule<SourceIterator, char()> pvChar;
  ///boost::spirit::qi::rule<SourceIterator, std::string()> pvSafechar;
  boost::spirit::qi::rule<SourceIterator, char()> pvSafechar;
  mdtCsvParserSettings pvSettings;
};

#endif // #ifndef MDT_CSV_PARSER_TEMPLATE_H
