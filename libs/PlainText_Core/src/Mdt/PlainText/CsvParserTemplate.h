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
#include "StringRecord.h"
#include "BoostSpiritQtTraits.h"
#include "Mdt/Error.h"
#include "Mdt/Expected.h"
#include <boost/spirit/include/qi.hpp>
#include <QCoreApplication>
#include <QChar>
#include <QString>
// #include <QVariant>
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

    /*! \brief Setup parser
     *
     * \pre \a settings must be valid
     */
    void setupParser(const CsvParserSettings & settings)
    {
      Q_ASSERT(settings.isValid());

      namespace qi = boost::spirit::qi;
      namespace phoenix = boost::phoenix;
      // Parsers
      using qi::lit;
      using qi::eol;
      using boost::spirit::standard_wide::char_;
      using boost::spirit::standard_wide::space;

      const char fieldSep = settings.fieldSeparator();
      const char fieldQuote = settings.fieldProtection();
      const bool parseExp = settings.parseExp();

      /*
       * Give our rules a name (used for error)
       */
      mRecordRule.name("RecordRule");
      /*
       * Build the grammar
       * This grammar is based on CSV-1203.
       * Some rules are also altered, to try to match RFC 4180 and unicode
       */
      mRecordRule = mRecordPayload >> -eol; // RFC 4180 do not need a end of line in last line
      mRecordPayload = mFieldColumn % char_(fieldSep);
      mFieldColumn = mProtectedField | mUnprotectedField;
      if(parseExp){
        mProtectedField = lit(fieldQuote) >> -lit('~') >> mFieldPayload >> lit(fieldQuote);
        mUnprotectedField = -lit('~') >> mRawFieldPayload;
      }else{
        mProtectedField = lit(fieldQuote) >> mFieldPayload >> lit(fieldQuote);
        mUnprotectedField = -char_('~') >> mRawFieldPayload; // mUnprotectedField = mRawFieldPayload causes runtime exception
      }
      mFieldPayload = *mAnychar;
      mRawFieldPayload = *mSafechar | (mSafechar >> *char_ >> mSafechar);
      // Character collections
      mAnychar = mChar | char_(fieldSep) | (char_(fieldQuote) >> lit(fieldQuote)) | space; // space matches space, CR, LF and other See std::isspace()
      mChar = mSafechar | char_(0x20);  // 0x20 == SPACE char
      ///std::string exclude = std::string(" \n\t\r") + fieldSep + fieldQuote;
      std::string exclude = std::string("\n\t\r") + fieldSep + fieldQuote;
      mSafechar = ~char_(exclude);

      /*
       * Parser nodes debug.
       * Don't forget to also uncomment #define BOOST_SPIRIT_DEBUG
       * Note: to display what id parsed,
       *       a certain amount of data is read from source.
       *       With mdtCsvFileParser, this causes
       *       it to be at eof after the first call of readLine().
       */
      //BOOST_SPIRIT_DEBUG_NODE(mRecordRule);
      //BOOST_SPIRIT_DEBUG_NODE(mRecordPayload);
      //BOOST_SPIRIT_DEBUG_NODE(mFieldColumn);
      //BOOST_SPIRIT_DEBUG_NODE(mProtectedField);
      //BOOST_SPIRIT_DEBUG_NODE(mUnprotectedField);
      //BOOST_SPIRIT_DEBUG_NODE(mFieldPayload);
      //BOOST_SPIRIT_DEBUG_NODE(mRawFieldPayload);
      //BOOST_SPIRIT_DEBUG_NODE(mAnychar);
      //BOOST_SPIRIT_DEBUG_NODE(mChar);
      //BOOST_SPIRIT_DEBUG_NODE(mSafechar);
    }

    /*! \brief Check if parser is valid
     *
     * Parser is valid once setupParser() was called at least once
     */
    bool isValid() const
    {
      return !mRecordRule.name().empty();
    }

    /*! \brief Read one line of CSV data
     *
     * \pre setupParser() must be called at least once before
     */
    Mdt::Expected<StringRecord> readLine(SourceIterator & first, const SourceIterator & last)
    {
      Q_ASSERT(first != last);
      Q_ASSERT_X(isValid(), "CsvParserTemplate", "parser setup never done");

      StringRecord record;

      bool ok = boost::spirit::qi::parse(first, last, mRecordRule, record);
      if(!ok){
        first = last;
        /// Store error \todo Better message needed..
        const auto msg = QCoreApplication::translate("Mdt::PlainText::CsvParserTemplate", "Parsing error occured.");
        auto error = mdtErrorNew(msg, Mdt::Error::Critical, "CsvParserTemplate");
        return error;
        /// \todo Which place should the error message be genarated ? F.ex. File parser should output file name..
      }

      return record;
    }

   private:

    boost::spirit::qi::rule<SourceIterator, StringRecord()> mRecordRule;
    boost::spirit::qi::rule<SourceIterator, StringRecord()> mRecordPayload;
    boost::spirit::qi::rule<SourceIterator, QString()> mFieldColumn;
    boost::spirit::qi::rule<SourceIterator, std::wstring()> mProtectedField;
    boost::spirit::qi::rule<SourceIterator, std::wstring()> mUnprotectedField;
    boost::spirit::qi::rule<SourceIterator, std::wstring()> mFieldPayload;
    boost::spirit::qi::rule<SourceIterator, std::wstring()> mRawFieldPayload;
    boost::spirit::qi::rule<SourceIterator, wchar_t()> mAnychar;
    boost::spirit::qi::rule<SourceIterator, wchar_t()> mChar;
    boost::spirit::qi::rule<SourceIterator, wchar_t()> mSafechar;
  };

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_TEXT_CSV_PARSER_TEMPLATE_H
