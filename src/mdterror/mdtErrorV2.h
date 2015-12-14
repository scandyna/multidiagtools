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

#include <QDebug>

/// \todo voir https://github.com/cheinan/any_config

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
     qDebug() << "mdtErrorPrivateBase() , ref: " << ref;
   }
   
   mdtErrorPrivateBase(const mdtErrorPrivateBase & other) = delete;
//     : QSharedData(other),
//       level(other.level),
//       userErrorType(other.userErrorType),
//       text(other.text)
//    {
//      qDebug() << "mdtErrorPrivateBase(other), ref: " << ref;
//    }

   mdtErrorPrivateBase() = delete;
   
   mdtErrorPrivateBase & operator=(const mdtErrorPrivateBase &) = delete;
   mdtErrorPrivateBase(mdtErrorPrivateBase &&) = delete;

   // Destructor must be virtual (else mdtErrorPrivate::~mdtErrorPrivate() is never called)
   virtual ~mdtErrorPrivateBase()
   {
     qDebug() << "~mdtErrorPrivateBase() , ref: " << ref << " , text: " << text;
   }

  /*! \brief Destructor
   */
  ///virtual ~mdtErrorPrivateBase() {}

  short level;
  std::type_index userErrorType;
  QString text;
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
  mdtErrorPrivate(const mdtErrorPrivate &) = delete;
  mdtErrorPrivate & operator=(const mdtErrorPrivate &) = delete;
  mdtErrorPrivate(mdtErrorPrivate &&) = delete;
  
  
  T error;
};


/*! \brief Value class that contain a error
 *
 * mdtError contains only a pointer to (implicitly) shared data.
 *  As long as no error was set, no more memory is allocated.
 *  This allows to store a mdtError object with a few overhead.
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

  /*! \brief Get error level
   *
   * If no error was set,
   *  NoError is returned.
   */
  Level level() const;

  /*! \brief Get error text
   */
  QString text() const;

 private:

  /*! \brief Init shared part
   */
  template <typename T>
  void initShared(const T & userDefinedError)
  {
    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    static_assert(std::is_copy_constructible<T>::value, "T must be copy constructible");

//     if(pvShared){
//       qDebug() << "initShared() , ref: " << pvShared->ref << " , detach";
//       pvShared.detach();
//     }else{
//       qDebug() << "initShared() , new";
//       pvShared = new mdtErrorPrivate<T>(userDefinedError);
//     }

    if(pvShared){
      qDebug() << "initShared() , ref: " << pvShared->ref << " , reset";
      pvShared.reset();
    }
    qDebug() << "initShared() , new";
    pvShared = new mdtErrorPrivate<T>(userDefinedError);

  }

  QExplicitlySharedDataPointer<mdtErrorPrivateBase> pvShared;
};

#endif // #ifndef MDT_ERROR_V2_H
