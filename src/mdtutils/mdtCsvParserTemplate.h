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

// Uncomment those lines to enable parser debuging
#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_DEBUG_TRACENODE

#include "mdtError.h"
#include "mdtCsvSettings.h"
#include "mdtCsvData.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/bind.hpp>
#include <QChar>
#include <QString>
#include <QObject>

#include <string>

#include <iostream>
#include <QDebug>

#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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
    namespace qi = boost::spirit::qi;
    namespace phoenix = boost::phoenix;
    // Parsers
    using qi::char_;
    using qi::lit;
    using qi::eol;
    using boost::spirit::ascii::space;
    // Error handling
//     using boost::spirit::qi::on_error;
//     using boost::spirit::qi::fail;
//     ///using boost::phoenix::val;
//     using boost::phoenix::bind;
//     
//     using namespace boost::spirit::qi::labels;
//     using boost::phoenix::construct;
//     using boost::phoenix::val;

    char fieldSep = pvSettings.fieldSeparator;
    char fieldQuote = pvSettings.fieldProtection;
    /*
     * Give our rules a name (used for error)
     */
    pvRecordRule.name("pvRecordRule");
    /*
     * Build the grammar
     * Sources:
     *  - CSV-1203
     */
    pvRecordRule = pvRecordPayload >> eol;
    ///pvRecordRule = pvRecordPayload > eol;
    pvRecordPayload %= pvFieldColumn % char_(fieldSep);
    pvFieldColumn =  pvUnprotectedField | pvProtectedField;
    pvProtectedField = lit(fieldQuote) >> pvFieldPayload >> lit(fieldQuote);
    pvFieldPayload = +pvAnychar;
    pvUnprotectedField %= pvRawFieldPayload;
    pvRawFieldPayload = pvSafechar | (pvSafechar >> *char_ >> pvSafechar);
    pvAnychar = pvChar | char_(fieldSep) | (char_(fieldQuote) >> char_(fieldQuote)) | space; // space matches space, CR, LF and other See std::isspace()
    pvChar = pvSafechar | char_(0x20);  // 0x20 == SPACE char
//     pvSafechar = char_(0x21, 0x7F) - char_(fieldSep) - char_(fieldQuote); /// \todo Should allow all except sep, quote and EOF
    std::string exclude = std::string(" ") + fieldSep + fieldQuote;
    pvSafechar = ~char_(exclude);

//     qi::on_success(pvRecordRule, phoenix::bind(&mdtCsvParserTemplate::displaySuccess, this, qi::_1, qi::_2, qi::_3));
//     qi::on_error<qi::fail>(pvRecordRule, phoenix::bind(&mdtCsvParserTemplate::myErrorHandler, this, qi::_1, qi::_2, qi::_3, qi::_4));
//     
//     qi::on_error<qi::fail>(pvRecordPayload, phoenix::bind(&mdtCsvParserTemplate::onRecordPayloadError, this, qi::_1, qi::_2, qi::_3, qi::_4));
//     qi::on_error<qi::fail>(pvFieldColumn, phoenix::bind(&mdtCsvParserTemplate::onRecordPayloadError, this, qi::_1, qi::_2, qi::_3, qi::_4));


     BOOST_SPIRIT_DEBUG_NODE(pvRecordRule);
//      BOOST_SPIRIT_DEBUG_NODE(pvRecordPayload);
//      BOOST_SPIRIT_DEBUG_NODE(pvFieldColumn);
//      BOOST_SPIRIT_DEBUG_NODE(pvProtectedField);
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

  void displaySuccess(SourceIterator & first, const SourceIterator & last, const SourceIterator & errorPos)
  {
    std::cout << "OK, first: " << *first << " , last: " << *last << " , errorPos: " << *errorPos << std::endl;
    std::cout << " -> Complete: " << std::string(first, last) << std::endl;
    std::cout << " -> Partial: " << std::string(first, errorPos) << std::endl;
  }

  void myErrorHandler(SourceIterator & first, const SourceIterator & last, const SourceIterator & errorPos, const boost::spirit::info & what)
  {
    /**
     * NOTE: we must differenciate the case of EOL not found in string and other error
     * NOTE: obove is wrong: this error function is ONLY called if a expectation point fails (not on other failure)
     */
    std::cout << "Error REC RULE: " << what << std::endl;
    std::cout << " -> At: first: " << *first << " , last: " << *last << " , errorPos: " << *errorPos << std::endl;
    std::cout << " -> " << std::string(first, last) << std::endl;
  }

  void onRecordPayloadError(SourceIterator & first, const SourceIterator & last, const SourceIterator & errorPos, const boost::spirit::info & what)
  {
    using boost::phoenix::construct;

    std::cout << "Error REC payload: " << what << std::endl;
    std::cout << " -> At: first: " << *first << " , last: " << *last << " , errorPos: " << *errorPos << std::endl;
    std::cout << " -> : " << construct<std::string>(first, last) << std::endl;
  }

  void onFieldColumnError(SourceIterator & first, const SourceIterator & last, const SourceIterator & errorPos, const boost::spirit::info & what)
  {
    using boost::phoenix::construct;

    std::cout << "Error FIELD column: " << what << std::endl;
    std::cout << " -> At: first: " << *first << " , last: " << *last << " , errorPos: " << *errorPos << std::endl;
    std::cout << " -> : " << construct<std::string>(first, last) << std::endl;
  }

  /*! \internal Copy disabled
   */
  mdtCsvParserTemplate(const mdtCsvParserTemplate &) = delete;

  /*! \brief Set settings
   */
  void setSettings(const mdtCsvParserSettings & s)
  {
    pvSettings = s;
  }

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
  mdtCsvRawRecord readLine()
  {
    mdtCsvRawRecord record;
    using boost::spirit::qi::char_;
    using boost::phoenix::ref;
    ///using boost::spirit::ascii::space;

    // Special if we reached the end of source, or source is empty
    /// \todo Check if this should be a error or not
    if(pvCurrentSourcePosition == pvSourceEnd){
      return record;
    }
    // Parse a line
    bool ok = boost::spirit::qi::parse(pvCurrentSourcePosition, pvSourceEnd, pvRecordRule, record.columnDataList);
    ///bool ok = boost::spirit::qi::parse(pvCurrentSourcePosition, pvSourceEnd, char_);
    if(!ok){
      record.setErrorOccured();
      pvCurrentSourcePosition = pvSourceEnd;
      /// Store error \todo Better message needed..
      QString msg = tr("Parsing error occured.");
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtCsvParserTemplate");
      pvLastError.commit();
      /// \todo Witch place should the error message be genarated ? F.ex. File parser should output file name..
    }

    return record;
  }

//   mdtCsvRawRecord readLine(SourceIterator & first, const SourceIterator & last)
//   {
//     mdtCsvRawRecord record;
//     using boost::spirit::qi::char_;
//     using boost::phoenix::ref;
//     ///using boost::spirit::ascii::space;
// 
//     // Special if we reached the end of source, or source is empty
//     /// \todo Check if this should be a error or not
// //     if(pvCurrentSourcePosition == pvSourceEnd){
// //       return record;
// //     }
//     // Parse a line
//     bool ok = boost::spirit::qi::parse(first, last, pvRecordRule, record.columnDataList);
//     ///bool ok = boost::spirit::qi::parse(pvCurrentSourcePosition, pvSourceEnd, char_);
//     if(!ok){
//       record.setErrorOccured();
//       pvCurrentSourcePosition = pvSourceEnd;
//       /// Store error \todo Better message needed..
//       QString msg = tr("Parsing error occured.");
//       pvLastError.setError(msg, mdtError::Error);
//       MDT_ERROR_SET_SRC(pvLastError, "mdtCsvParserTemplate");
//       pvLastError.commit();
//       /// \todo Witch place should the error message be genarated ? F.ex. File parser should output file name..
//     }
// 
//     return record;
//   }

 private:

  /*! \brief Translate (calls QObject::tr() )
   */
  QString tr(const char *sourceText)
  {
    return QObject::tr(sourceText);
  }

  /*! \brief Idées pour unicode:
   *  - std::vector de QChar ? (voir ce qu'il faut faire pour que QChar fonctionne..
   *   -> NOTE: QChar: 16 bit avec "tag" de version unicode (unicode actuel, dont UTF-8, peut être > 16bit)
   *  - Bricoler un genrre de stream QString -> std::string "à la demande"..
   *  - NOTE: revoir "compatibilité" UTF-8 pour les caractères ASCII (-> ' , ", \n, etc...)
   *   -> Cela voudrait dire: pvSafechar devrait tout accepter sauf sep et quote (et EOF)
   *   -> Il faudrait aussi s'assurer que la source soit UTF-8 (et pas UTF-16, UTF-32, ...)
   */

  SourceIterator pvCurrentSourcePosition;
  SourceIterator pvSourceEnd;
  boost::spirit::qi::rule<SourceIterator, QVector<std::wstring>()> pvRecordRule;
  boost::spirit::qi::rule<SourceIterator, QVector<std::wstring>()> pvRecordPayload;
  boost::spirit::qi::rule<SourceIterator, std::wstring()> pvFieldColumn;
  boost::spirit::qi::rule<SourceIterator, std::wstring()> pvProtectedField;
  boost::spirit::qi::rule<SourceIterator, std::wstring()> pvUnprotectedField;
  boost::spirit::qi::rule<SourceIterator, std::wstring()> pvFieldPayload;
  boost::spirit::qi::rule<SourceIterator, std::wstring()> pvRawFieldPayload;
  boost::spirit::qi::rule<SourceIterator, wchar_t()> pvAnychar;
  boost::spirit::qi::rule<SourceIterator, wchar_t()> pvChar;
  boost::spirit::qi::rule<SourceIterator, wchar_t()> pvSafechar;
  mdtCsvParserSettings pvSettings;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CSV_PARSER_TEMPLATE_H
