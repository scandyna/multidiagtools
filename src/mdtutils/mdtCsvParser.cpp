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
#include "mdtCsvParser.h"
#include "mdtCsvParserTemplate.h"
/**
 * For Sandbox
 */
#include <boost/spirit/home/support.hpp>
#include <boost/spirit/home/support/multi_pass.hpp>
// #include <boost/spirit/home/support/iterators/detail/functor_input_policy.hpp>


#include <boost/spirit/home/support/iterators/multi_pass_fwd.hpp>
#include <boost/spirit/home/support/iterators/detail/multi_pass.hpp>

#include <iostream>
#include <string>
/**
 * Sanbox
 */

// class mdtReadIterator
// {
//  public:
// 
//   typedef wchar_t result_type;
// 
//   mdtReadIterator()
//    : pvC('A')
//   {
//     std::cout << "mdtReadIterator::mdtReadIterator()" << std::endl;
//     pvBuffer.push_back('A');
//     pvFisrt = pvBuffer.cbegin();
//     pvLast = pvBuffer.cend();
//   }
// 
// //   mdtReadIterator(wchar_t c)
// //    : pvC(c)
// //   {
// //     std::cout << "mdtReadIterator::mdtReadIterator(" << c << ")" << std::endl;
// //   }
// 
//   result_type operator()()
//   {
//     std::wcout << "mdtReadIterator::operator()()" << std::endl;
//     
//     if(pvFisrt == pvLast){
//       return eof;
//     }
//     return *pvFisrt++;
//   }
// 
//   static result_type eof;
// 
//  private:
// 
//   result_type pvC;
//   std::wstring pvBuffer;
//   std::wstring::const_iterator pvFisrt, pvLast;
// };
// mdtReadIterator::result_type mdtReadIterator::eof = mdtReadIterator::result_type(4);


struct mdtCsvInputPolicy
{
  // Input is the same type as the first template parameter while instanciating the multi_pass
  template <typename Input>
  struct unique
  {
    typedef std::wstring::iterator::value_type value_type;
    typedef std::wstring::iterator::difference_type difference_type;
    typedef std::wstring::iterator::difference_type distance_type;
    typedef std::wstring::iterator::pointer pointer;
    typedef std::wstring::iterator::reference reference;

    unique()
    {
      std::cout << "mdtCsvInputPolicy::unique::unique()" << std::endl;
    }

    explicit unique(Input)
    {
      std::cout << "mdtCsvInputPolicy::unique::unique(Input)" << std::endl;
    }

    template <typename MultiPass>
    static void destroy(MultiPass &)
    {
      std::cout << "mdtCsvInputPolicy::unique::destroy()" << std::endl;
    }

    void swap(unique &)
    {
      std::cout << "mdtCsvInputPolicy::unique::swap()" << std::endl;
    }

    template <typename MultiPass>
    static typename MultiPass::reference get_input(MultiPass & mp)
    {
      std::cout << "mdtCsvInputPolicy::unique::get_input()" << std::endl;
      return mp.shared()->get_input();
    }

    template <typename MultiPass>
    static void advance_input(MultiPass & mp)
    {
      std::cout << "mdtCsvInputPolicy::unique::advance_input()" << std::endl;
      mp.shared()->advance_input();
    }

    template <typename MultiPass>
    static bool input_at_eof(const MultiPass & mp)
    {
      std::cout << "mdtCsvInputPolicy::unique::input_at_eof()" << std::endl;

      /*
       * See: http://sourceforge.net/p/spirit/mailman/spirit-general/thread/3C1A726A.9090003@caldera.com/
       *
       * Returning true, false, or whatever simply don't work (?!?)
       */
      static Input const end_iter;
      return mp.shared()->input_ == end_iter;
    }

    template <typename MultiPass>
    static bool input_is_valid(const MultiPass & mp, const value_type & t)
    {
      std::cout << "mdtCsvInputPolicy::unique::input_is_valid()" << std::endl;
      return mp.shared()->input_is_valid_;
    }
  };

  template <typename Input>
  struct shared
  {
    explicit shared(const Input & input)
     : input_(input),
       curtok_(0),
       input_is_valid_(false),
       count(0)
    {
      std::cout << "mdtCsvInputPolicy::shared::shared()" << std::endl;
    }

    void advance_input()
    {
      input_is_valid_ = false;
      if(count >= 0){
        input_ = Input();
        return;
      }
      ++input_;
      ++count;
    }

    wchar_t & get_input()
    {
      if(!input_is_valid_){
        curtok_ = *input_;
        input_is_valid_ = true;
      }
      return curtok_;
    }

    Input input_;
    wchar_t curtok_;
    bool input_is_valid_;
    int count;
  };
};

using namespace boost::spirit;

typedef multi_pass<std::wstring::iterator, iterator_policies::default_policy<
                  iterator_policies::first_owner,
                  iterator_policies::no_check, 
                  mdtCsvInputPolicy, 
                  iterator_policies::split_std_deque> > mdt_reader_multi_pass_type;

// typedef multi_pass<std::string::iterator, iterator_policies::default_policy<
//                    iterator_policies::first_owner,
//                    iterator_policies::no_check,
//                    iterator_policies::input_iterator,
//                    iterator_policies::split_std_deque> > mdt_input_iterator;

typedef iterator_policies::default_policy<iterator_policies::first_owner,
                   iterator_policies::no_check,
                   /**iterator_policies::input_iterator,*/
                   mdtCsvInputPolicy,
                   iterator_policies::split_std_deque> somePolicy;

typedef multi_pass<std::wstring::iterator, somePolicy> mdt_input_iterator;

void mdtReadIteratorTestFunction()
{
  std::cout << "mdtReadIteratorTestFunction() ..." << std::endl;
  std::wstring str;
  str.push_back('A');
  str.push_back('B');
  str.push_back('\n');
  
//   mdt_reader_multi_pass_type first(str.begin());// = mdt_reader_multi_pass_type(str.begin());
//   mdt_reader_multi_pass_type last(str.end());// = mdt_reader_multi_pass_type(str.end());
//   mdt_input_iterator first(str.begin());
//   mdt_input_iterator last(str.end());

  mdt_input_iterator first = make_multi_pass<somePolicy, std::wstring::iterator>(std::wstring::iterator(str.begin()));
  mdt_input_iterator last = make_multi_pass<somePolicy, std::wstring::iterator>(std::wstring::iterator());

//   while(first != last){
//     std::wcout << "Val: " << *first << std::endl;
//     ++first;
//   }
  mdtCsvParserTemplate<mdt_input_iterator> parser;
//   
//   mdt_reader_multi_pass_type first = mdt_reader_multi_pass_type(mdtReadIterator());
//   mdt_reader_multi_pass_type last;
// 
//   parser.setSource(first, last);
//   
  mdtCsvRawRecord rec = parser.readLine(first, last);
  if(rec.errorOccured()){
    return;
  }
  for(const auto & data : rec.columnDataList){
    std::wcout << data << std::endl;
  }
  
}

/*
 * mdtCsvStringParser implementation
 */

mdtCsvStringParser::mdtCsvStringParser()
 : pvImpl(new mdtCsvParserTemplate<std::wstring::const_iterator>)
{
}

mdtCsvStringParser::~mdtCsvStringParser()
{
}

void mdtCsvStringParser::setSettings(const mdtCsvParserSettings & s)
{
  pvImpl->setSettings(s);
}

void mdtCsvStringParser::setSource(const QString & source)
{
  /*
   * We have to convert source to std::wstring type,
   * witch is supported by boost::spirit.
   * A try was made to include QString support in spirit,
   * but this disi not work.
   */
  pvSource = source.toStdWString();
  pvImpl->setSource(pvSource.cbegin(), pvSource.cend());
}

bool mdtCsvStringParser::atEnd() const
{
  return pvImpl->atEnd();
}

mdtCsvRecord mdtCsvStringParser::readLine()
{
  mdtCsvRecord record;
  mdtCsvRawRecord rawRecord;

  rawRecord = pvImpl->readLine();
  record.columnDataList.reserve(rawRecord.count());
  for(const auto & data : rawRecord.columnDataList){
    record.columnDataList.append(QString::fromStdWString(data));
  }

  return record;
}
