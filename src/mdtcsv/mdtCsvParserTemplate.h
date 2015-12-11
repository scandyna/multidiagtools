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
// #define BOOST_SPIRIT_DEBUG
// #define BOOST_SPIRIT_DEBUG_TRACENODE

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

#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include <QChar>
#include <QString>

#include <QVariant>

#include <QObject>

#include <QVector>

#include <string>

#include <iostream>
#include <QDebug>


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace boost { namespace spirit { namespace traits
{
  /*! \internal Make Qi recognize QString as a container
   */
  template <>
  struct is_container<QString> : mpl::true_
  {
  };

//   /*! \internal Expose QString's value_type
//    */
//   template <>
//   struct container_value<QString> : mpl::identity<QChar>
//   {
//   };

  /*! \internal Define how to insert a new element at the end of the QString container
   */
  template <typename T>
  struct push_back_container<QString, T>
  {
    /*! \internal call function
     */
    static bool call(QString & str, const QChar & c)
    {
      str.append(c);
      return true;
    }
  };

  /*! \internal Test if a QString is empty (required for debug)
  */
  template <>
  struct is_empty_container<QString>
  {
    static bool call(const QString & c)
    {
      return c.isEmpty();
    }
  };

  /*! \internal Define how to stream a QString (required for debug)
   */
  template <typename Out, typename Enable>
  struct print_attribute_debug<Out, QString, Enable>
  {
    static void call(Out & out, const QString & val)
    {
      out << val.toStdString();
    }
  };

  /*! \internal Define how to stream a QVariant (required for debug)
   */
  template <typename Out, typename Enable>
  struct print_attribute_debug<Out, QVariant, Enable>
  {
    static void call(Out & out, const QVariant & val)
    {
      out << val.toString().toStdString();
    }
  };

}}} // namespace boost { namespace spirit { namespace traits

/*! \brief CSV parser template
 *
 * This class implements the parser logic,
 *  and can act on different input containers
 *  that have STL compatible iterators.
 *
 * \tparam SourceIterator Type of iterator that will act on the source.
 * \note Including directly this header in a project can slow down compilation time
 * \sa mdtCsvStringParser
 * \sa mdtCsvFileParser
 * \note Some part of this API documentation refers to following standards:
 *       \li CSV-1203 available here: http://mastpoint.com/csv-1203
 *       \li RFC 4180 available here: https://tools.ietf.org/html/rfc4180
 */
template <typename SourceIterator>
 // requirse RandomAccessIterator<SourceIterator>
class mdtCsvParserTemplate
{
 public:

  /*! \brief Default constructor
   */
  mdtCsvParserTemplate(const mdtCsvParserSettings & csvSettings)
  {
    namespace qi = boost::spirit::qi;
    namespace phoenix = boost::phoenix;
    // Parsers
    ///using qi::char_;
    using qi::lit;
    using qi::eol;
    ///using boost::spirit::ascii::space;
    using boost::spirit::standard_wide::char_;
    using boost::spirit::standard_wide::space;

    char fieldSep = csvSettings.fieldSeparator;
    char fieldQuote = csvSettings.fieldProtection;
    bool parseExp = csvSettings.parseExp;

    /*
     * Give our rules a name (used for error)
     */
//     pvRecordRule.name("pvRecordRule");
//     pvRecordPayload.name("pvRecordPayload");
//     pvFieldColumn.name("pvFieldColumn");
//     pvProtectedField.name("pvProtectedField");
//     pvUnprotectedField.name("pvUnprotectedField");
//     pvFieldPayload.name("pvFieldPayload");
//     pvRawFieldPayload.name("pvRawFieldPayload");
//     pvAnychar.name("pvAnychar");
//     pvChar.name("pvChar");
//     pvSafechar.name("pvSafechar");
    /*
     * Build the grammar
     * This grammar is based on CSV-1203.
     * Some rules are also altered, to try to match RFC 4180 and unicode
     */
    pvRecordRule = pvRecordPayload >> -eol; // RFC 4180 do not need a end of line in last line
    ///pvRecordRule = pvRecordPayload >> eol;
    pvRecordPayload = pvFieldColumn % char_(fieldSep);
    pvFieldColumn = pvProtectedField | pvUnprotectedField;
    if(parseExp){
      pvProtectedField = lit(fieldQuote) >> -lit('~') >> pvFieldPayload >> lit(fieldQuote);
      pvUnprotectedField = -lit('~') >> pvRawFieldPayload;
    }else{
      pvProtectedField = lit(fieldQuote) >> pvFieldPayload >> lit(fieldQuote);
      pvUnprotectedField = -char_('~') >> pvRawFieldPayload; // pvUnprotectedField = pvRawFieldPayload causes runtime exception
    }
    pvFieldPayload = *pvAnychar;
    ///pvRawFieldPayload = *pvSafechar | (pvSafechar >> *char_ >> pvSafechar);
    pvRawFieldPayload = *pvSafechar | (pvSafechar >> *char_ >> pvSafechar);
    // Character collections
    pvAnychar = pvChar | char_(fieldSep) | (char_(fieldQuote) >> lit(fieldQuote)) | space; // space matches space, CR, LF and other See std::isspace()
    pvChar = pvSafechar | char_(0x20);  // 0x20 == SPACE char
    std::string exclude = std::string(" \n\t\r") + fieldSep + fieldQuote;
    pvSafechar = ~char_(exclude);

//     qi::on_success(pvRecordRule, phoenix::bind(&mdtCsvParserTemplate::displaySuccess, this, qi::_1, qi::_2, qi::_3));
//     qi::on_error<qi::fail>(pvRecordRule, phoenix::bind(&mdtCsvParserTemplate::myErrorHandler, this, qi::_1, qi::_2, qi::_3, qi::_4));


    BOOST_SPIRIT_DEBUG_NODE(pvRecordRule);
    BOOST_SPIRIT_DEBUG_NODE(pvRecordPayload);
    BOOST_SPIRIT_DEBUG_NODE(pvFieldColumn);
    BOOST_SPIRIT_DEBUG_NODE(pvProtectedField);
    BOOST_SPIRIT_DEBUG_NODE(pvUnprotectedField);
    BOOST_SPIRIT_DEBUG_NODE(pvFieldPayload);
    BOOST_SPIRIT_DEBUG_NODE(pvRawFieldPayload);
    BOOST_SPIRIT_DEBUG_NODE(pvAnychar);
    BOOST_SPIRIT_DEBUG_NODE(pvChar);
    BOOST_SPIRIT_DEBUG_NODE(pvSafechar);
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

//   void myErrorHandler(SourceIterator & first, const SourceIterator & last, const SourceIterator & errorPos, const boost::spirit::info & what)
//   {
//     /**
//      * NOTE: we must differenciate the case of EOL not found in string and other error
//      * NOTE: obove is wrong: this error function is ONLY called if a expectation point fails (not on other failure)
//      */
//     std::cout << "Error REC RULE: " << what << std::endl;
// //     std::cout << " -> At: first: " << *first << " , last: " << *last << " , errorPos: " << *errorPos << std::endl;
// //     std::cout << " -> " << std::string(first, last) << std::endl;
//   }

  /*! \internal Copy disabled
   */
  mdtCsvParserTemplate(const mdtCsvParserTemplate &) = delete;

  /*! \brief Read one line of CSV data
   */
  mdtCsvRecord readLine(SourceIterator & first, const SourceIterator & last)
  {
    mdtCsvRecord record;
    using boost::spirit::qi::char_;
    using boost::phoenix::ref;

    // Special if we reached the end of source, or source is empty
    /// \todo Check if this should be a error or not
    if(first == last){
      return record;
    }
    // Parse a line
    bool ok = boost::spirit::qi::parse(first, last, pvRecordRule, record.columnDataList);
    if(!ok){
      record.setErrorOccured();
      first = last;
      /// Store error \todo Better message needed..
      QString msg = tr("Parsing error occured.");
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtCsvParserTemplate");
      pvLastError.commit();
      /// \todo Witch place should the error message be genarated ? F.ex. File parser should output file name..
    }

    return record;
  }

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Translate (calls QObject::tr() )
   */
  QString tr(const char *sourceText)
  {
    return QObject::tr(sourceText);
  }

  boost::spirit::qi::rule<SourceIterator, QVector<QVariant>()> pvRecordRule;
  boost::spirit::qi::rule<SourceIterator, QVector<QVariant>()> pvRecordPayload;
  boost::spirit::qi::rule<SourceIterator, QString()> pvFieldColumn;
  boost::spirit::qi::rule<SourceIterator, std::wstring()> pvProtectedField;
  boost::spirit::qi::rule<SourceIterator, std::wstring()> pvUnprotectedField;
  boost::spirit::qi::rule<SourceIterator, std::wstring()> pvFieldPayload;
  boost::spirit::qi::rule<SourceIterator, std::wstring()> pvRawFieldPayload;
  boost::spirit::qi::rule<SourceIterator, wchar_t()> pvAnychar;
  boost::spirit::qi::rule<SourceIterator, wchar_t()> pvChar;
  boost::spirit::qi::rule<SourceIterator, wchar_t()> pvSafechar;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CSV_PARSER_TEMPLATE_H
