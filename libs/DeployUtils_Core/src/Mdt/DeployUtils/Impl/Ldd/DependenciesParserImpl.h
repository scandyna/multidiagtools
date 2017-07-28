/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_IMPL_LDD_DEPENDENCIES_PARSER_IMPL_H
#define MDT_DEPLOY_UTILS_IMPL_LDD_DEPENDENCIES_PARSER_IMPL_H

// Uncomment this line to enable parser debuging
#define BOOST_SPIRIT_DEBUG

// #include "Mdt/PlainText/Record.h"
// #include "Mdt/PlainText/RecordList.h"
// #include "Mdt/PlainText/StringRecord.h"
#include "Mdt/PlainText/BoostSpiritQtTraits.h"
// #include <boost/fusion/adapted/struct/adapt_struct.hpp>
// #include <boost/fusion/include/adapt_struct.hpp>
#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <QString>
#include <QStringList>
#include <string>
#include <vector>
#include <tuple>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Ldd{

  template<typename SourceIterator>
  struct DependenciesRecordGrammar : boost::spirit::qi::grammar<SourceIterator, QStringList()>
  {
    DependenciesRecordGrammar()
    : DependenciesRecordGrammar::base_type(mRecordRule)
    {
      namespace qi = boost::spirit::qi;
      // Parsers
      using qi::lit;
//       using qi::char_;
      using qi::eol;
      using boost::spirit::standard_wide::char_;

      ///mLibraryCommonName = +(char_ - lit(L' '));
      ///mLinuxVdso = lit(L"linux-vdso.so.1") | lit(L"linux-gate.so.1") | lit(L"linux-vdso32.so.1") | lit(L"linux-vdso64.so.1");
      ///mLibraryName = mLibraryCommonName | mLinuxVdso;
      mLibraryName = +(char_ - lit(L' '));
      mAddress = lit(L"(0x") >> +char_("0123456789abcdefABCDEF") >> lit(L')');
      mLibraryPath = +(char_ - lit(L" ("));
      mRecordRule = +lit('\t') >> mLibraryName >> lit(L" => ") >> -mLibraryPath >> lit(L' ') >> mAddress;
      
//       mCode = int_;
//       mClass = char_("MIABCD");
//       mText = *(char_ - eol);
//       mDate = char_('0','3') >> char_('0','9') >> char_('.') >> char_('0','1') >> char_('0','9') >> char_('.') >> char_('0','9') >> char_('0','9') >> char_('0','9') >> char_('0','9');
//       mTime = char_('0','2') >> char_('0','9') >> char_(':') >> char_('0','5') >> char_('0','9') >> char_(':') >> char_('0','5') >> char_('0','9');
//       mStartDateTime = mDate >> +lit(L' ') >> mTime;
//       mEndDateTime = mDate >> +lit(L' ') >> mTime;
//       mW = qi::bool_parser<bool, ParserImpl::ZsgWFlagBoolPolicies>() | qi::attr(false);
//       mRecordRule = mCode >> *lit(L' ') >> lit(L':') >> +lit(L' ') >> mClass >> lit(L' ') >> mText >> eol
//                     >> *lit(L' ') >> mStartDateTime >> lit(L", ") >> mEndDateTime >> *lit(L' ') >> mW;

      BOOST_SPIRIT_DEBUG_NODE(mLibraryCommonName);
      BOOST_SPIRIT_DEBUG_NODE(mLinuxVdso);
      BOOST_SPIRIT_DEBUG_NODE(mLibraryName);
      BOOST_SPIRIT_DEBUG_NODE(mLibraryPath);
      BOOST_SPIRIT_DEBUG_NODE(mAddress);
      BOOST_SPIRIT_DEBUG_NODE(mRecordRule);
//       BOOST_SPIRIT_DEBUG_NODE(mTime);
//       BOOST_SPIRIT_DEBUG_NODE(mStartDateTime);
//       BOOST_SPIRIT_DEBUG_NODE(mEndDateTime);
//       BOOST_SPIRIT_DEBUG_NODE(mW);
    }

   private:

//     boost::spirit::qi::rule<SourceIterator, int()> mCode;
//     boost::spirit::qi::rule<SourceIterator, QString()> mClass;
//     boost::spirit::qi::rule<SourceIterator, QString()> mText;
//     boost::spirit::qi::rule<SourceIterator, QString()> mDate;
//     boost::spirit::qi::rule<SourceIterator, QString()> mTime;
//     boost::spirit::qi::rule<SourceIterator, std::vector<QString>()> mStartDateTime;
//     boost::spirit::qi::rule<SourceIterator, std::vector<QString>()> mEndDateTime;
//     boost::spirit::qi::rule<SourceIterator, bool()> mW;
    boost::spirit::qi::rule<SourceIterator, QString()> mLibraryCommonName;
    boost::spirit::qi::rule<SourceIterator, QString()> mLinuxVdso;
    boost::spirit::qi::rule<SourceIterator, QString()> mLibraryName;
    boost::spirit::qi::rule<SourceIterator, QString()> mLibraryPath;
    boost::spirit::qi::rule<SourceIterator> mAddress;
    
    boost::spirit::qi::rule<SourceIterator, QStringList()> mRecordRule;
  };

  template<typename SourceIterator>
  class DependenciesParserTemplate
  {
  public:

    DependenciesParserTemplate()
    {
      namespace qi = boost::spirit::qi;
      // Parsers
      using qi::lit;
      using qi::eol;
      using boost::spirit::standard_wide::char_;
      using boost::spirit::standard_wide::space;

    }

    // Copy disabled
    DependenciesParserTemplate(const DependenciesParserTemplate &) = delete;
    DependenciesParserTemplate & operator=(const DependenciesParserTemplate &) = delete;
    // Move disabled
    DependenciesParserTemplate(DependenciesParserTemplate &&) = delete;
    DependenciesParserTemplate & operator=(DependenciesParserTemplate &&) = delete;

    bool parseAll(SourceIterator & first, SourceIterator last)
    {
//       return boost::spirit::qi::parse(first, last, mZsgFile, mData);
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Ldd{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_LDD_DEPENDENCIES_PARSER_IMPL_H
