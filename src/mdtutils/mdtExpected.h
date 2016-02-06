/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_EXPECTED_H
#define MDT_EXPECTED_H

#include "mdtError.h"
#include <utility>

//#include <QDebug>

/*! \brief Contains a value or a error
 *
 * mdtExpected provides a basic support of expected value.
 *  This is inspired from A. Alexandrescu 's talk
 *  "Systematic Error Handling in C++" : 
 *   http://channel9.msdn.com/Shows/Going+Deep/C-and-Beyond-2012-Andrei-Alexandrescu-Systematic-Error-Handling-in-C
 *
 * mdtExpected is a very limited version of the concept.
 *  For more advanced and performant version,
 *  you should take a look at the official proposal:
 *  https://github.com/ptal/expected
 *
 * \tparam T Type of value
 */
/*
 * This class does not provide a default constructor.
 *  - What would be a default constructed expected ?
 *  - Would requier a flag for assignment operators,
 *    to know if pvError's destructor must be called or not
 */
template <typename T>
class mdtExpected
{
 public:

  /*! \brief Construct a empty expected
   *
   * A empty expected contains a null error
   *  (see mdtError::isNull()).
   */
  mdtExpected()
   : pvHasValue(false),
     pvError(mdtError())
  {
  }

  /*! \brief Construct a expected with a value
   */
  mdtExpected(const T & v)
   : pvHasValue(true),
     pvValue(v)
  {
  }

  /*! \brief Construct a expected with a value
   */
  mdtExpected(T && v)
   : pvHasValue(true),
     pvValue(std::move(v))
  {
  }

  /*! \brief Construct a expected with a error
   */
  mdtExpected(const mdtError & e)
   : pvHasValue(false),
     pvError(e)
  {
  }

  /*! \brief Construct a expected with a error
   */
  mdtExpected(mdtError && e)
   : pvHasValue(false),
     pvError(std::move(e))
  {
  }

  /*! \brief Construct a copy of other
   */
  mdtExpected(const mdtExpected & other)
   : pvHasValue(other.pvHasValue)
  {
    if(pvHasValue){
      new(&pvValue) T(other.pvValue);
    }else{
      new(&pvError) mdtError(other.pvError);
    }
  }

  /*! \brief Construct by moving other
   */
  mdtExpected(mdtExpected && other)
   : pvHasValue(other.pvHasValue)
  {
    if(pvHasValue){
      new(&pvValue) T(std::move(other.pvValue));
    }else{
      new(&pvError) mdtError(std::move(other.pvError));
    }
  }

  /*! \brief Destructor
   */
  ~mdtExpected()
  {
    if(pvHasValue){
      pvValue.~T();
    }else{
      pvError.~mdtError();
    }
  }

  /*! \brief Assign a value
   */
  mdtExpected & operator=(const T & v)
  {
    if(!pvHasValue){
      pvError.~mdtError();
    }
    pvHasValue = true;
    new(&pvValue) T(v);
    return *this;
  }

  /*! \brief Assign a value
   */
  mdtExpected & operator=(T && v)
  {
    if(!pvHasValue){
      pvError.~mdtError();
    }
    pvHasValue = true;
    new(&pvValue) T(std::move(v));
    return *this;
  }

  /*! \brief Assign a error
   */
  mdtExpected & operator=(const mdtError & e)
  {
    if(pvHasValue){
      pvValue.~T();
    }
    pvHasValue = false;
    new(&pvError) mdtError(e);
    return *this;
  }

  /*! \brief Assign a error
   */
  mdtExpected & operator=(mdtError && e)
  {
    if(pvHasValue){
      pvValue.~T();
    }
    pvHasValue = false;
    new(&pvError) mdtError(std::move(e));
    return *this;
  }

  /*! \brief Assign a mdtExpected
   */
  mdtExpected & operator=(const mdtExpected & other)
  {
    if(&other == this){
      return *this;
    }
    pvHasValue = other.pvHasValue;
    if(pvHasValue){
      pvError.~mdtError();
      new(&pvValue) T(other.pvValue);
    }else{
      pvValue.~T();
      new(&pvError) mdtError(other.pvError);
    }
    return *this;
  }

  /*! \brief Assign a mdtExpected
   */
  mdtExpected & operator=(mdtExpected && other)
  {
    if(&other == this){
      return *this;
    }
    if(pvHasValue){
      pvValue.~T();
      if(other.pvHasValue){
        new(&pvValue) T(std::move(other.pvValue));
      }else{
        new(&pvError) mdtError(std::move(other.pvError));
      }
    }else{
      pvError.~mdtError();
      if(other.pvHasValue){
        new(&pvValue) T(std::move(other.pvValue));
      }else{
        new(&pvError) mdtError(std::move(other.pvError));
      }
    }
    pvHasValue = other.pvHasValue;

    return *this;
  }

  /*! \brief Return true if a error was set
   */
  bool hasError() const
  {
    return !pvHasValue;
  }

  /*! \brief Access error
   *
   * \pre this must contain a error
   */
  mdtError & error()
  {
    Q_ASSERT(!pvHasValue);
    return pvError;
  }

  /*! \brief Access error (read only)
   *
   * \pre this must contain a error
   */
  const mdtError & error() const
  {
    Q_ASSERT(!pvHasValue);
    return pvError;
  }

  /*! \brief Return true if a value was set
   */
  bool hasValue() const
  {
    return pvHasValue;
  }

  /*! \brief Return true if a value was set
   */
  operator bool() const
  {
    return pvHasValue;
  }

  /*! \brief Access value
   *
   * \pre this must contain a value
   */
  T & value()
  {
    Q_ASSERT(pvHasValue);
    return pvValue;
  }

  /*! \brief Access value (read only)
   *
   * \pre this must contain a value
   */
  const T & value() const
  {
    Q_ASSERT(pvHasValue);
    return pvValue;
  }

 private:

  bool pvHasValue;
  union
  {
    T pvValue;
    mdtError pvError;
  };
};

#endif // #ifndef MDT_EXPECTED_H
