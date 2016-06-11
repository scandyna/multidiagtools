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

#include "Mdt/Error.h"
#include <utility>

//#include <QDebug>

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
   *  https://github.com/ptal/expected
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
    : pvHasValue(false),
      pvError(Mdt::Error())
    {
    }

    /*! \brief Construct a expected with a value
     */
    Expected(const T & v)
    : pvHasValue(true),
      pvValue(v)
    {
    }

    /*! \brief Construct a expected with a value
     */
    Expected(T && v)
    : pvHasValue(true),
      pvValue(std::move(v))
    {
    }

    /*! \brief Construct a expected with a error
     */
    Expected(const Mdt::Error & e)
    : pvHasValue(false),
      pvError(e)
    {
    }

    /*! \brief Construct a expected with a error
     */
    Expected(Mdt::Error && e)
    : pvHasValue(false),
      pvError(std::move(e))
    {
    }

    /*! \brief Construct a copy of other
     */
    Expected(const Expected & other)
    : pvHasValue(other.pvHasValue)
    {
      if(pvHasValue){
        new(&pvValue) T(other.pvValue);
      }else{
        new(&pvError) Mdt::Error(other.pvError);
      }
    }

    /*! \brief Construct by moving other
     */
    Expected(Expected && other)
    : pvHasValue(other.pvHasValue)
    {
      if(pvHasValue){
        new(&pvValue) T(std::move(other.pvValue));
      }else{
        new(&pvError) Mdt::Error(std::move(other.pvError));
      }
    }

    /*! \brief Destructor
     */
    ~Expected()
    {
      if(pvHasValue){
        pvValue.~T();
      }else{
        pvError.~Error();
      }
    }

    /*! \brief Assign a value
     */
    Expected & operator=(const T & v)
    {
      if(!pvHasValue){
        pvError.~Error();
      }
      pvHasValue = true;
      new(&pvValue) T(v);
      return *this;
    }

    /*! \brief Assign a value
     */
    Expected & operator=(T && v)
    {
      if(!pvHasValue){
        pvError.~Error();
      }
      pvHasValue = true;
      new(&pvValue) T(std::move(v));
      return *this;
    }

    /*! \brief Assign a error
     */
    Expected & operator=(const Mdt::Error & e)
    {
      if(pvHasValue){
        pvValue.~T();
      }
      pvHasValue = false;
      new(&pvError) Mdt::Error(e);
      return *this;
    }

    /*! \brief Assign a error
     */
    Expected & operator=(Mdt::Error && e)
    {
      if(pvHasValue){
        pvValue.~T();
      }
      pvHasValue = false;
      new(&pvError) Mdt::Error(std::move(e));
      return *this;
    }

    /*! \brief Assign a Expected
     */
    Expected & operator=(const Expected & other)
    {
      if(&other == this){
        return *this;
      }
      pvHasValue = other.pvHasValue;
      if(pvHasValue){
        pvError.~Error();
        new(&pvValue) T(other.pvValue);
      }else{
        pvValue.~T();
        new(&pvError) Mdt::Error(other.pvError);
      }
      return *this;
    }

    /*! \brief Assign a Expected
     */
    Expected & operator=(Expected && other)
    {
      if(&other == this){
        return *this;
      }
      if(pvHasValue){
        pvValue.~T();
        if(other.pvHasValue){
          new(&pvValue) T(std::move(other.pvValue));
        }else{
          new(&pvError) Mdt::Error(std::move(other.pvError));
        }
      }else{
        pvError.~Error();
        if(other.pvHasValue){
          new(&pvValue) T(std::move(other.pvValue));
        }else{
          new(&pvError) Mdt::Error(std::move(other.pvError));
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
    Mdt::Error & error()
    {
      Q_ASSERT(!pvHasValue);
      return pvError;
    }

    /*! \brief Access error (read only)
     *
     * \pre this must contain a error
     */
    const Mdt::Error & error() const
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
      Mdt::Error pvError;
    };
  };

} // namespace Mdt{

#endif // #ifndef MDT_EXPECTED_H
