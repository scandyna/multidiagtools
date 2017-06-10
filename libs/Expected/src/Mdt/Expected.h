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
#ifndef MDT_EXPECTED_H
#define MDT_EXPECTED_H

#include "Mdt/Error.h"
#include <utility>

namespace Mdt{

  /*! \brief Contains a value or a error
   *
   * Expected provides a basic support of expected value.
   *  This is inspired from A. Alexandrescu 's talk
   *  "Systematic Error Handling in C++" : 
   *   http://channel9.msdn.com/Shows/Going+Deep/C-and-Beyond-2012-Andrei-Alexandrescu-Systematic-Error-Handling-in-C
   *
   * Mdt::Expected is a very limited version of the concept.
   *  For more advanced and performant version,
   *  you should take a look at the official proposal:
   *  https://github.com/viboes/std-make/tree/master/doc/proposal/expected
   *
   * \tparam T Type of value
   */
  template <typename T>
  class Expected
  {
  public:

    /*! \brief Construct a empty expected
     *
     * A empty expected contains a null error
     *  (see Mdt::Error::isNull()).
     */
    Expected()
    : mHasValue(false),
      mError(Mdt::Error())
    {
    }

    /*! \brief Construct a expected with a value
     */
    Expected(const T & v)
    : mHasValue(true),
      mValue(v)
    {
    }

    /*! \brief Construct a expected with a value
     */
    Expected(T && v)
    : mHasValue(true),
      mValue(std::move(v))
    {
    }

    /*! \brief Construct a expected with a error
     */
    Expected(const Mdt::Error & e)
    : mHasValue(false),
      mError(e)
    {
    }

    /*! \brief Construct a expected with a error
     */
    Expected(Mdt::Error && e)
    : mHasValue(false),
      mError(std::move(e))
    {
    }

    /*! \brief Construct a copy of other
     */
    Expected(const Expected & other)
    : mHasValue(other.mHasValue)
    {
      if(mHasValue){
        new(&mValue) T(other.mValue);
      }else{
        new(&mError) Mdt::Error(other.mError);
      }
    }

    /*! \brief Construct by moving other
     */
    Expected(Expected && other)
    : mHasValue(other.mHasValue)
    {
      if(mHasValue){
        new(&mValue) T(std::move(other.mValue));
      }else{
        new(&mError) Mdt::Error(std::move(other.mError));
      }
    }

    /*! \brief Destructor
     */
    ~Expected()
    {
      freeValueOrError();
    }

    /*! \brief Assign a value
     */
    Expected & operator=(const T & v)
    {
      freeValueOrError();
      mHasValue = true;
      new(&mValue) T(v);
      return *this;
    }

    /*! \brief Assign a value
     */
    Expected & operator=(T && v)
    {
      freeValueOrError();
      mHasValue = true;
      new(&mValue) T(std::move(v));
      return *this;
    }

    /*! \brief Assign a error
     */
    Expected & operator=(const Mdt::Error & e)
    {
      freeValueOrError();
      mHasValue = false;
      new(&mError) Mdt::Error(e);
      return *this;
    }

    /*! \brief Assign a error
     */
    Expected & operator=(Mdt::Error && e)
    {
      freeValueOrError();
      mHasValue = false;
      new(&mError) Mdt::Error(std::move(e));
      return *this;
    }

    /*! \brief Assign a Expected
     */
    Expected & operator=(const Expected & other)
    {
      if(&other == this){
        return *this;
      }
      if(mHasValue){
        mValue.~T();
        if(other.mHasValue){
          new(&mValue) T(other.mValue);
        }else{
          new(&mError) Mdt::Error(other.mError);
        }
      }else{
        mError.~Error();
        if(other.mHasValue){
          new(&mValue) T(other.mValue);
        }else{
          new(&mError) Mdt::Error(other.mError);
        }
      }
      mHasValue = other.mHasValue;
      return *this;
    }

    /*! \brief Assign a Expected
     */
    Expected & operator=(Expected && other)
    {
      if(&other == this){
        return *this;
      }
      if(mHasValue){
        mValue.~T();
        if(other.mHasValue){
          new(&mValue) T(std::move(other.mValue));
        }else{
          new(&mError) Mdt::Error(std::move(other.mError));
        }
      }else{
        mError.~Error();
        if(other.mHasValue){
          new(&mValue) T(std::move(other.mValue));
        }else{
          new(&mError) Mdt::Error(std::move(other.mError));
        }
      }
      mHasValue = other.mHasValue;

      return *this;
    }

    /*! \brief Return true if a error was set
     */
    bool hasError() const
    {
      return !mHasValue;
    }

    /*! \brief Access error
     *
     * \pre this must contain a error
     */
    Mdt::Error & error()
    {
      Q_ASSERT(!mHasValue);
      return mError;
    }

    /*! \brief Access error (read only)
     *
     * \pre this must contain a error
     */
    const Mdt::Error & error() const
    {
      Q_ASSERT(!mHasValue);
      return mError;
    }

    /*! \brief Return true if a value was set
     */
    bool hasValue() const
    {
      return mHasValue;
    }

    /*! \brief Return true if a value was set
     */
    operator bool() const
    {
      return mHasValue;
    }

    /*! \brief Access value
     *
     * \pre this must contain a value
     */
    T & value()
    {
      Q_ASSERT(mHasValue);
      return mValue;
    }

    /*! \brief Access value (read only)
     *
     * \pre this must contain a value
     */
    const T & value() const
    {
      Q_ASSERT(mHasValue);
      return mValue;
    }

  private:

    void freeValueOrError()
    {
      if(mHasValue){
        mValue.~T();
      }else{
        mError.~Error();
      }
    }

    bool mHasValue;
    union
    {
      T mValue;
      Mdt::Error mError;
    };
  };

} // namespace Mdt{

#endif // #ifndef MDT_EXPECTED_H
