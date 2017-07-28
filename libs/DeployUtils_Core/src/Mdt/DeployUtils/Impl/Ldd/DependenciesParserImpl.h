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
// #define BOOST_SPIRIT_DEBUG

#include "Mdt/PlainText/StringRecord.h"
#include "Mdt/PlainText/StringRecordList.h"
#include "Mdt/PlainText/BoostSpiritQtTraits.h"
#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <QString>
#include <string>
#include <vector>
#include <tuple>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Ldd{

  template<typename SourceIterator>
  struct DependenciesRecordGrammar : boost::spirit::qi::grammar<SourceIterator, PlainText::StringRecord()>
  {
    DependenciesRecordGrammar()
    : DependenciesRecordGrammar::base_type(mRecordRule)
    {
      namespace qi = boost::spirit::qi;
      // Parsers
      using qi::lit;
      using qi::eol;
      using boost::spirit::standard_wide::char_;

      mLibraryName = +(char_ - lit(L' '));
      mAddress = lit(L"(0x") >> +char_("0123456789abcdefABCDEF") >> lit(L')');
      mLibraryPath = +(char_ - lit(L" ("));
      mRecordRule = +lit('\t') >> mLibraryName >> lit(L" => ") >> -mLibraryPath >> -(lit(L' ') >> mAddress);

      BOOST_SPIRIT_DEBUG_NODE(mLibraryName);
      BOOST_SPIRIT_DEBUG_NODE(mLibraryPath);
      BOOST_SPIRIT_DEBUG_NODE(mAddress);
//       BOOST_SPIRIT_DEBUG_NODE(mRecordRule);
    }

   private:

    boost::spirit::qi::rule<SourceIterator, QString()> mLibraryName;
    boost::spirit::qi::rule<SourceIterator, QString()> mLibraryPath;
    boost::spirit::qi::rule<SourceIterator> mAddress;
    boost::spirit::qi::rule<SourceIterator, PlainText::StringRecord()> mRecordRule;
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

      mDependenciesRule = mRecord % eol;
    }

    // Copy disabled
    DependenciesParserTemplate(const DependenciesParserTemplate &) = delete;
    DependenciesParserTemplate & operator=(const DependenciesParserTemplate &) = delete;
    // Move disabled
    DependenciesParserTemplate(DependenciesParserTemplate &&) = delete;
    DependenciesParserTemplate & operator=(DependenciesParserTemplate &&) = delete;

    bool parseAll(SourceIterator & first, SourceIterator last)
    {
      return boost::spirit::qi::parse(first, last, mDependenciesRule, mRawDependencies);
    }

    Mdt::PlainText::StringRecordList rawDependencies() const
    {
      return mRawDependencies;
    }

   private:

    DependenciesRecordGrammar<SourceIterator> mRecord;
    boost::spirit::qi::rule<SourceIterator, PlainText::StringRecordList()> mDependenciesRule;
    Mdt::PlainText::StringRecordList mRawDependencies;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Ldd{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_LDD_DEPENDENCIES_PARSER_IMPL_H
