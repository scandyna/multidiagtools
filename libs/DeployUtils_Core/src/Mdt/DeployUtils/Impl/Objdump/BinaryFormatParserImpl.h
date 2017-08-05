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
#ifndef MDT_DEPLOY_UTILS_IMPL_OBJDUMP_BINARY_FORMAT_PARSER_IMPL_WINDOWS_H
#define MDT_DEPLOY_UTILS_IMPL_OBJDUMP_BINARY_FORMAT_PARSER_IMPL_WINDOWS_H

// Uncomment this line to enable parser debuging
#define BOOST_SPIRIT_DEBUG

#include "Mdt/PlainText/StringRecord.h"
// #include "Mdt/PlainText/StringRecordList.h"
#include "Mdt/PlainText/BoostSpiritQtTraits.h"
#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <QString>
#include <string>
#include <vector>
#include <tuple>


namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Objdump{

  template<typename SourceIterator>
  struct FileFormatGrammar : boost::spirit::qi::grammar<SourceIterator, QString()>
  {
    FileFormatGrammar()
    : FileFormatGrammar::base_type(mFormatRule)
    {
      namespace qi = boost::spirit::qi;
      // Parsers
//       using qi::lit;
//       using qi::eol;
      using boost::spirit::standard_wide::char_;
      using boost::spirit::standard_wide::string;

      mFormatName = string("elf64-x86-64") | string("elf32-i386") | string("elf32-iamcu") | string("elf32-x86-64") | string("a.out-i386-linux")
                  | string("elf64-l1om") | string("elf64-k1om") | string("elf64-little") | string("elf64-big") | string("elf32-little") | string("elf32-big")
                  | string("pei-i386") | string("pei-x86-64") | string("pe-x86-64") | string("pe-bigobj-x86-64") | string("pe-i386");
      mOtherText = *(char_ - mFormatName);
      mFormatRule = mOtherText >> mFormatName;

//       BOOST_SPIRIT_DEBUG_NODE(mFormatName);
//       BOOST_SPIRIT_DEBUG_NODE(mOtherText);
//       BOOST_SPIRIT_DEBUG_NODE(mFormatRule);
    }

   private:

    boost::spirit::qi::rule<SourceIterator, QString()> mFormatName;
    boost::spirit::qi::rule<SourceIterator> mOtherText;
    boost::spirit::qi::rule<SourceIterator, QString()> mFormatRule;
  };

  template<typename SourceIterator>
  struct ArchitectureGrammar : boost::spirit::qi::grammar<SourceIterator, QString()>
  {
    ArchitectureGrammar()
    : ArchitectureGrammar::base_type(mArchitectureRule)
    {
      namespace qi = boost::spirit::qi;
      // Parsers
//       using qi::lit;
//       using qi::eol;
      using boost::spirit::standard_wide::char_;
      using boost::spirit::standard_wide::string;

      mArchitectureName = string("i386:x86-64:intel")
                        | string("i386:x86-64:nacl")
                        | string("i386:x86-64")
                        | string("i386:x64-32:intel")
                        | string("i386:x64-32:nacl")
                        | string("i386:x64-32")
                        | string("i386:intel")
                        | string("i386:nacl")
                        | string("i386")
                        | string("iamcu:intel")
                        | string("iamcu")
                        | string("l1om:intel")
                        | string("l1om")
                        | string("k1om:intel")
                        | string("k1om")
                        | string("i8086");
      mOtherText = *(char_ - mArchitectureName);
      mArchitectureRule = mOtherText >> mArchitectureName;

//       BOOST_SPIRIT_DEBUG_NODE(mArchitectureName);
//       BOOST_SPIRIT_DEBUG_NODE(mOtherText);
//       BOOST_SPIRIT_DEBUG_NODE(mArchitectureRule);
    }

   private:

    boost::spirit::qi::rule<SourceIterator, QString()> mArchitectureName;
    boost::spirit::qi::rule<SourceIterator> mOtherText;
    boost::spirit::qi::rule<SourceIterator, QString()> mArchitectureRule;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Objdump{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_OBJDUMP_BINARY_FORMAT_PARSER_IMPL_WINDOWS_H
