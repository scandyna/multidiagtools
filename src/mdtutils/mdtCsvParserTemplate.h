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

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/bind.hpp>
#include <string>
#include <vector>

#include <iostream>
#include <QDebug>

using boost::phoenix::push_back;
using boost::spirit::ascii::space;

/*! \brief CSV parser grammar to read a line
 */
// template<typename Iterator>
// class mdtCsvParserLineGrammar : boost::spirit::qi::grammar<Iterator, std::string(), boost::spirit::ascii::space_type>
// {
//  public:
// 
//   /*! \brief Constructor
//    */
//   mdtCsvParserLineGrammar()
//    : mdtCsvParserLineGrammar::base_type(start)
//   {
//     using boost::spirit::qi::double_;
//     using boost::spirit::qi::char_;
//     using boost::spirit::qi::eol;
//     using boost::spirit::qi::lit;
// 
//     start = *char_;
//     ///start = *(char_) >> eol;
//     ///start = double_ % char_(',');
//     ///start = double_ >> *(lit(',') >> double_);
//     ///start = double_ | (lit(',') >> double_);
//   }
// 
//  ///private:
// 
//   boost::spirit::qi::rule<Iterator, std::string(), boost::spirit::ascii::space_type> start;
// };


/*! \brief CSV parser template
 *
 * This class implements the parser logic,
 *  and can act on different input containers
 *  that have STL compatible iterators.
 *
 * \tparam SourceIterator Type of iterator that will act on the source.
 * \note Including directly this header in a project can slow down compilation time
 * \sa mdtCsvStringParser
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
    using boost::spirit::qi::eol;
    ///using boost::spirit::qi::eof;
    
    pvRule = *char_ >> -eol;
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
    std::cout << "(1) Start: " << *begin << " , size: " << std::distance(begin, end) << std::endl;
    
    ///pvCurrentSourcePosition = boost::spirit::make_default_multi_pass(begin);
    ///pvSourceEnd = boost::spirit::make_default_multi_pass(end);
    pvCurrentSourcePosition = begin;
    pvSourceEnd = end;
    
    std::cout << "(2) Start: " << *pvCurrentSourcePosition << " , size: " << std::distance(pvCurrentSourcePosition, pvSourceEnd) << std::endl;
  }

  /*! \brief Read one line
   */
  bool readLine()
  {
    ///return boost::spirit::qi::phrase_parse(pvCurrentSourcePosition, pvSourceEnd, pvLineGrammar, space, pvLine);
    
    using boost::spirit::qi::char_;
    using boost::phoenix::ref;
    using boost::spirit::ascii::space;
    
    std::cout << "(3) Start: " << *pvCurrentSourcePosition << " , size: " << std::distance(pvCurrentSourcePosition, pvSourceEnd) << std::endl;
    
    ///std::string str("1234");
    
    ///bool ok = boost::spirit::qi::phrase_parse(pvCurrentSourcePosition, pvSourceEnd, *char_[boost::phoenix::push_back(boost::phoenix::ref(pvLine), _1)], space/**, pvLine*/);
    ///bool ok = boost::spirit::qi::parse(pvCurrentSourcePosition, pvSourceEnd, *char_, pvLine);
    bool ok = boost::spirit::qi::phrase_parse(pvCurrentSourcePosition, pvSourceEnd, pvRule, space, pvLine);

    ///bool ok = boost::spirit::qi::parse(str.cbegin(), str.cend(), *char_, pvLine);
    ///bool ok = boost::spirit::qi::parse(str.cbegin(), str.cend(), pvLineGrammar, pvLine);
    if(ok){
      std::cout << "elm: " << pvLine.size() << " , : " << pvLine << std::endl;
    }
    
    return ok;
  }

 private:

  ///boost::spirit::multi_pass<SourceIterator> pvCurrentSourcePosition;
  ///boost::spirit::multi_pass<SourceIterator> pvSourceEnd;
  SourceIterator pvCurrentSourcePosition;
  SourceIterator pvSourceEnd;
//   mdtCsvParserLineGrammar<boost::spirit::multi_pass<SourceIterator> > pvLineGrammar;
  
  ///std::vector<std::string> pvRowData;
  std::string pvLine;
  
  boost::spirit::qi::rule<SourceIterator, std::string(), boost::spirit::ascii::space_type> pvRule;
};

#endif // #ifndef MDT_CSV_PARSER_TEMPLATE_H
