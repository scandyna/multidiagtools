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
#ifndef MDT_ERROR_V2_H
#define MDT_ERROR_V2_H

#include <QString>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>
#include <type_traits>  // std::is_same
#include <typeindex>
#include <typeinfo>
#include <vector>

#include <QDebug>

class mdtErrorV2;

/*! \internal Generic error
 */
struct mdtGenericError
{
};

/*! \internal Private base for mdtError
 */
struct mdtErrorPrivateBase : public QSharedData
{
  /*! \brief Constructor
   */
  mdtErrorPrivateBase(const std::type_info & ti)
   : userErrorType(ti)
   {
     qDebug() << "mdtErrorPrivateBase()";
   }

   ///mdtErrorPrivateBase(const mdtErrorPrivateBase & other) = delete;
   mdtErrorPrivateBase(const mdtErrorPrivateBase & other) = default;
   mdtErrorPrivateBase() = delete;
   mdtErrorPrivateBase & operator=(const mdtErrorPrivateBase &) = delete;
   mdtErrorPrivateBase(mdtErrorPrivateBase &&) = delete;

  virtual mdtErrorPrivateBase *clone() const = 0;
   
   // Destructor must be virtual (else mdtErrorPrivate::~mdtErrorPrivate() is never called)
   virtual ~mdtErrorPrivateBase()
   {
     qDebug() << "~mdtErrorPrivateBase()";
   }

  short level;
  std::type_index userErrorType;
  QString text;
  QString informativeText;
  std::vector<mdtErrorV2> pvErrorStack;
};

/*! \internal User defined error
 */
template <typename T>
struct mdtErrorPrivate : public mdtErrorPrivateBase
{
  mdtErrorPrivate(const T & e)
  : mdtErrorPrivateBase(typeid(T)),
    error(e)
  {
    qDebug() << "mdtErrorPrivate()";
  }

  ~mdtErrorPrivate()
  {
    qDebug() << "~mdtErrorPrivate()";
  }

  mdtErrorPrivate() = delete;
  ///mdtErrorPrivate(const mdtErrorPrivate &) = delete;

  mdtErrorPrivate(const mdtErrorPrivate & other)
   : mdtErrorPrivateBase(other),
     error(other.error)
  {
    qDebug() << "--*--*-> mdtErrorPrivate(copy)";
  }

  mdtErrorPrivate & operator=(const mdtErrorPrivate &) = delete;
  mdtErrorPrivate(mdtErrorPrivate &&) = delete;

  mdtErrorPrivate *clone() const
  {
    return new mdtErrorPrivate(*this);
  }
  
  T error;
};


/*! \brief Value class that contain a error
 *
 * mdtError contains only a pointer to (implicitly) shared data.
 *  As long as no error was set, no more memory is allocated.
 *  This allows to store a mdtError object with a few overhead.
 *
 * 
 */
class mdtErrorV2
{
 public:

  /*! \brief Error level
   */
  enum Level : short
  {
    NoError,   /*!< No error . */
    Info,      /*!< Just a information, application continues to work in normal way . */
    Warning,   /*!< Error that could be handled */
    Error      /*!< Error that was not handled */
  };

  /*! \brief Construct a null error
   */
  mdtErrorV2();

  /*! \brief Construct a copy of other error
   */
  // Copy construct is handled by QExplicitlySharedDataPointer
  mdtErrorV2(const mdtErrorV2 &) = default;

  ///mdtErrorV2(mdtErrorV2 &&) = default;

  /*! \brief Check if error is null
   *
   * Returns true as long as no error was set,
   *  or after clear was called.
   */
  bool isNull() const
  {
    if(!pvShared){
      return true;
    }
    return false;
  }

  /*! \brief Clear error
   *
   * Will also free internal resources.
   *  After clear, the error is null.
   */
  void clear();

  /*! \brief Set error
   */
  void setError(const QString & text, Level level)
  {
    setError<mdtGenericError>(mdtGenericError(), text, level);
  }

  /*! \brief Set user defined error
   *
   * Setting error will clear it first.
   *
   * \tparam T Type of user defined error.
   * \param error Error of type T that will be stored.
   * \pre error type T must be default and copy constructible.
   */
  template <typename T>
  void setError(const T & error, const QString & text, Level level)
  {
    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    static_assert(std::is_copy_constructible<T>::value, "T must be copy constructible");

    initShared<T>(error);
    pvShared->text = text;
    pvShared->level = level;
  }

  /*! \brief Get user defined error
   *
   * If no error was set,
   *  a default constructed error of type T is returned.
   *
   * \pre Requested type T must match the one that was stored with setError()
   */
  template <typename T>
  T error() const
  {
    static_assert(!std::is_reference<T>::value, "Getting a reference type of stored error is not possible");

    if(!pvShared){
      return T();
    }
    Q_ASSERT_X(std::type_index(typeid(T)) == pvShared->userErrorType, "T mdtError::error() const", "Requested type T is not the same as stored error type");
    Q_ASSERT(dynamic_cast<const mdtErrorPrivate<T>*>(pvShared.constData()) != nullptr);

    return static_cast<const mdtErrorPrivate<T>*>(pvShared.constData())->error;
  }

  /*! \brief Set informative text
   *
   * Can be set to give the user a fuller description of the error.
   *  This is the same meaning than QMessageBox,
   *  and is used the same way in mdtErrorDialog.
   *
   * \pre this error must not be null when calling this function
   * \sa setInformativeText()
   */
  void setInformativeText(const QString & text);

  /*! \brief Get informative text
   *
   * \sa setInformativeText()
   */
  QString informativeText() const;

  /*! \brief Get error level
   *
   * If no error was set,
   *  NoError is returned.
   */
  Level level() const;

  /*! \brief Get error text
   */
  QString text() const;

  /*! \brief Stack given error
   *
   * \pre this error and given error must not be null
   */
  void stackError(const mdtErrorV2 & error);

  /*! \brief Get error stack
   *
   * \note The returned stack is rebuilt at each call.
   */
  std::vector<mdtErrorV2> getErrorStack() const;

 private:

  /*! \brief Init shared part
   */
  template <typename T>
  void initShared(const T & userDefinedError)
  {
    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    static_assert(std::is_copy_constructible<T>::value, "T must be copy constructible");

    if(pvShared){
      pvShared.reset();
    }
    pvShared = new mdtErrorPrivate<T>(userDefinedError);
  }

  QExplicitlySharedDataPointer<mdtErrorPrivateBase> pvShared;
};

template <>
mdtErrorPrivateBase *QExplicitlySharedDataPointer<mdtErrorPrivateBase>::clone()
{
  qDebug() << "--------- Clone from " << d->text << ", type: " << d->userErrorType.hash_code();
  ///auto *ptr = new mdtErrorPrivate<d->userErrorType>(d->userErrorType());
  ///auto *ptr = new mdtErrorPrivateBase(*d);
  auto *ptr = d->clone();
  qDebug() << "----------> clone is " << ptr->text << ", type: " << ptr->userErrorType.hash_code();
  
  return ptr;
  ///return new mdtErrorPrivate<mdtGenericError>(mdtGenericError());
}

#endif // #ifndef MDT_ERROR_V2_H
