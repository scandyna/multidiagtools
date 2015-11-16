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

#include "mdtCsvSettings.h"
#include "mdtCsvData.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
///#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/bind.hpp>
#include <string>
#include <vector>

#include <QVector>

#include <iostream>
#include <QDebug>

///using boost::phoenix::push_back;
///using boost::spirit::ascii::space;

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
    ///using boost::spirit::qi::eof;
    
    auto fieldSep = pvSettings.fieldSeparator;
    
    pvRecordRule = pvRecordPayload >> -eol;
    pvRecordPayload %= pvFieldPayload % lit(fieldSep);
    
    pvFieldPayload = *(char_ - eol - lit(fieldSep));
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

  /*! \brief Read one line
   */
  bool readLine()
  {
    using boost::spirit::qi::char_;
    using boost::phoenix::ref;
    ///using boost::spirit::ascii::space;

    ///bool ok = boost::spirit::qi::phrase_parse(pvCurrentSourcePosition, pvSourceEnd, pvLineRule, space, pvLine);
//     bool ok = boost::spirit::qi::parse(pvCurrentSourcePosition, pvSourceEnd, pvRecordRule, pvLine);
//     if(ok){
//       std::cout << "elm: " << pvLine.size() << " , : " << pvLine << std::endl;
//     }
    pvRecord.clear();
    bool ok = boost::spirit::qi::parse(pvCurrentSourcePosition, pvSourceEnd, pvRecordRule, pvRecord.rawColumnDataList);
    if(ok){
      ///std::cout << "elm: " << pvRowData.size() << std::endl;
      for(const auto & str : pvRecord.rawColumnDataList){
        std::cout << str << "|";
      }
      std::cout << std::endl;
    }

    return ok;
  }

 private:

  SourceIterator pvCurrentSourcePosition;
  SourceIterator pvSourceEnd;
  
  ///QVector<std::string> pvRowData;
  mdtCsvRecord pvRecord;
  ///std::string pvLine;
  
  ///boost::spirit::qi::rule<SourceIterator, std::string(), boost::spirit::ascii::space_type> pvLineRule;
  boost::spirit::qi::rule<SourceIterator, QVector<std::string>()> pvRecordRule;
  boost::spirit::qi::rule<SourceIterator, QVector<std::string>()> pvRecordPayload;
  
  boost::spirit::qi::rule<SourceIterator, std::string()> pvFieldPayload;
  
  mdtCsvParserSettings pvSettings;
};

#endif // #ifndef MDT_CSV_PARSER_TEMPLATE_H
