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
#ifndef MDT_PLAIN_TEXT_BOOST_SPIRIT_QT_TRAITS_H
#define MDT_PLAIN_TEXT_BOOST_SPIRIT_QT_TRAITS_H

#include "StringConstIterator.h"
#include <QChar>
#include <QString>
#include <QVariant>
#include <boost/spirit/home/qi/detail/assign_to.hpp>
#include <boost/spirit/include/support_container.hpp>
#include <algorithm>
#include <iterator>

namespace boost { namespace spirit { namespace traits
{
  /*! \internal Tell Qi how to store a result from a parser to a QString
   */
  template<>
  struct assign_to_attribute_from_iterators<QString, Mdt::PlainText::StringConstIterator, void>
  {
    static void call(const Mdt::PlainText::StringConstIterator & first, const Mdt::PlainText::StringConstIterator & last, QString & attr)
    {
      std::copy( first, last, std::back_inserter(attr) );
    }
  };

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
      out << "QString(" << val.toStdString() << ")";
    }
  };

  /*! \internal Test if a QVariant is empty (required for debug)
  */
  template <>
  struct is_empty_container<QVariant>
  {
    static bool call(const QVariant & v)
    {
      if(v.type() == QVariant::String){
        return v.toString().isEmpty();
      }
      return v.isNull();
    }
  };

  /*! \internal Define how to stream a QVariant (required for debug)
   */
  template <typename Out, typename Enable>
  struct print_attribute_debug<Out, QVariant, Enable>
  {
    static void call(Out & out, const QVariant & val)
    {
      out << "QVariant(" << val.toString().toStdString() << ")";
    }
  };

}}} // namespace boost { namespace spirit { namespace traits

#endif // #ifndef MDT_PLAIN_TEXT_BOOST_SPIRIT_QT_TRAITS_H
