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

#include <QExplicitlySharedDataPointer>
#include <QString>
#include <QSharedData>
#include <type_traits>  // std::is_same
#include <typeindex>
#include <typeinfo>
#include <vector>

//#include <QDebug>

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
   : level(0),
     userErrorType(ti),
     lineNumber(0)
   {
   }

  mdtErrorPrivateBase(const mdtErrorPrivateBase & other) = default;
  mdtErrorPrivateBase() = delete;
  mdtErrorPrivateBase & operator=(const mdtErrorPrivateBase &) = delete;
  mdtErrorPrivateBase(mdtErrorPrivateBase &&) = delete;

  // Needed for QExplicitlySharedDataPointer::clone()
  virtual mdtErrorPrivateBase *clone() const = 0;

  // Destructor must be virtual (else mdtErrorPrivate::~mdtErrorPrivate() is never called)
  virtual ~mdtErrorPrivateBase()
  {
  }

  short level;
  std::type_index userErrorType;
  QString text;
  QString informativeText;
  std::vector<mdtErrorV2> pvErrorStack;
  QString fileName;
  int lineNumber;
  QString functionName;
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
  }

  ~mdtErrorPrivate()
  {
  }

  mdtErrorPrivate() = delete;


  mdtErrorPrivate & operator=(const mdtErrorPrivate &) = delete;
  mdtErrorPrivate(mdtErrorPrivate &&) = delete;

  // Because of template, copy constructor must be explicit ( used by clone() )
  mdtErrorPrivate(const mdtErrorPrivate & other)
   : mdtErrorPrivateBase(other),
     error(other.error)
  {
  }

  // Needed for QExplicitlySharedDataPointer::clone()
  mdtErrorPrivate *clone() const
  {
    return new mdtErrorPrivate(*this);
  }

  T error;
};

class QObject;

/*! \brief Helper macro to build a mdtError with source file informations
 *
 * Typical usage:
 * \code
 * auto error = mdtErrorNew("Some error occured", mdtError::Error, "MyClass");
 * \endcode
 *
 * \note If you create a error for a QObject subclass, you should use mdtErrorNewQ()
 */
#define mdtErrorNew(text, level, className) mdtErrorV2(static_cast<mdtGenericError>(mdtGenericError()), text, level, __FILE__, __LINE__, className, __FUNCTION__)

/*! \brief Helper macro to build a user defined mdtError with source file informations
 *
 * Typical usage:
 * \code
 * auto error = mdtErrorNewT(int, -1, "Some error occured", mdtError::Error, "MyClass");
 * \endcode
 *
 * \note If you create a error for a QObject subclass, you should use mdtErrorNewTQ()
 */
#define mdtErrorNewT(T, error, text, level, className) mdtErrorV2(static_cast<T>(error), text, level, __FILE__, __LINE__, className, __FUNCTION__)

/*! \brief Helper macro to build a mdtError with source file informations
 *
 * This version accepts a QObject instance, witch avoids typing class name explicitly.
 *  If you create a error for a object that is not a subclass of QObject,
 *  use mdtErrorNew()
 *
 * Typical usage:
 * \code
 * auto error = mdtErrorNewQ("Some error occured", mdtError::Error, this);
 * \endcode
 */
#define mdtErrorNewQ(text, level, obj) mdtErrorV2(static_cast<mdtGenericError>(mdtGenericError()), text, level, __FILE__, __LINE__, obj, __FUNCTION__)

/*! \brief Helper macro to build a user defined mdtError with source file informations
 *
 * This version accepts a QObject instance, witch avoids typing class name explicitly.
 *  If you create a error for a object that is not a subclass of QObject,
 *  use mdtErrorNewT()
 *
 * Typical usage:
 * \code
 * auto error = mdtErrorNewTQ(int, -1, "Some error occured", mdtError::Error, this);
 * \endcode
 */
#define mdtErrorNewTQ(T, error, text, level, obj) mdtErrorV2(static_cast<T>(error), text, level, __FILE__, __LINE__, obj, __FUNCTION__)

/*! \brief Value class that contain a error
 *
 * mdtError contains only a pointer to (implicitly) shared data (also known as copy-on-write).
 *  As long as no error was set, no more memory is allocated.
 *  This allows to store a mdtError object with a few overhead.
 *
 * Concept of error stack
 *
 *  Imagine a case of a application that provides document editing functionnality,
 *  and the user wants to save a document.
 *  The application will probably call a helper function from its own library,
 *  witch also calls a other system function from a onther part of the library,
 *  witch finally calls a (maybe system dependant) low level function.
 *  The low level function fails (for some reason).
 *  How could the application provide the most usefull error message to the user ?
 *  Lets illustrate a possible call stack:
 *  <table border="1" cellpadding="5">
 *   <tr><th>Function</th><th>Error</th><th>Error message</th></tr>
 *   <tr><td>write()</td><td>EDQUOT (int)</td><td>Disk quota exhausted</td></tr>
 *   <tr><td>writeToFile()</td><td>DiskQuotaExhausted (enum)</td><td>Could not write to file 'document.txt' because disk quota exhausted</td></tr>
 *   <tr><td>saveDocument()</td><td></td><td>Could not save your work to 'document.txt'. This is because you reached the disk quota. Please try to save the document to a other place and contact your administrator to solve the problem.</td></tr>
 *  </table>
 * In above scenario, the application can build appropriate message because it knows what DiskQuotaExhausted means.
 * For some other errors (that the application currently not handles), it could also simply display the error returned from saveDocument().
 * To implement such error stack, simply, at each level, stack a error that a function returns to current error.
 * For example, in writeToFile(), if write() fails, we create a new mdtError, and return it.
 * Then, saveDocument() will fail, create its own mdtError object, and stack the one returned by writeToFile().
 * To stack a error, use stackError() , and use getErrorStack() to get stacked errors back.
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

  /*! \brief Construct a error with error and source set
   *
   * Calling this constructor directly is a bit long.
   *  Consider using mdtErrorNew() or mdtErrorNewT() macro.
   */
  template <typename T>
  mdtErrorV2(const T & error, const QString & text, Level level,
             const QString & fileName, int fileLine, const QString & className, const QString & functionName)
  {
    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    static_assert(std::is_copy_constructible<T>::value, "T must be copy constructible");

    setError<T>(error, text, level);
    setSource(fileName, fileLine, className, functionName);
  }

  /*! \brief Construct a error with error and source set
   *
   * Calling this constructor directly is a bit long.
   *  Consider using mdtErrorNewQ() or mdtErrorNewTQ() macro.
   */
  template <typename T>
  mdtErrorV2(const T & error, const QString & text, Level level,
             const QString & fileName, int fileLine, const QObject * const obj, const QString & functionName)
  {
    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    static_assert(std::is_copy_constructible<T>::value, "T must be copy constructible");
    Q_ASSERT(obj != nullptr);

    setError<T>(error, text, level);
    setSource(fileName, fileLine, obj, functionName);
  }

  /*! \brief Construct a copy of other error
   */
  // Copy construct is handled by QExplicitlySharedDataPointer
  mdtErrorV2(const mdtErrorV2 &) = default;

  //mdtErrorV2(mdtErrorV2 &&) = default;

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
#ifndef QT_NO_DEBUG
    /*
     * Requested type coherence check:
     * If we can, we provide caller information.
     * This seems somwhat ugly, but provides helpfull informations
     * (and is only compiled in debug)
     * Note: we use Q_ASSERT_X() because qFatal() requires to include QDebug,
     *       witch we really not want here.
     */
    QString msg;
    if(std::type_index(typeid(T)) != pvShared->userErrorType){
      // If we have caller informations, put it into failure message
      const QString functionName = pvShared->functionName;
      QString caller;
      if(!functionName.isEmpty()){
        caller = " mdtError object source: " + functionName + " in file " + pvShared->fileName + ", line " + QString::number(pvShared->lineNumber);
      }
      msg = "Requested type T is not the same as stored error type." + caller;
    }
    Q_ASSERT_X(std::type_index(typeid(T)) == pvShared->userErrorType, "T mdtError::error() const", msg.toStdString().c_str());
    Q_ASSERT(dynamic_cast<const mdtErrorPrivate<T>*>(pvShared.constData()) != nullptr);
#endif // #ifndef QT_NO_DEBUG

    return static_cast<const mdtErrorPrivate<T>*>(pvShared.constData())->error;
  }

  /*! \brief Set informative text
   *
   * Can be set to give the user a fuller description of the error.
   *  This is the same meaning than QMessageBox,
   *  and is used the same way in mdtErrorDialog.
   *
   * \pre this error must not be null when calling this function
   * \sa informativeText()
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
   * \sa getErrorStack()
   */
  void stackError(const mdtErrorV2 & error);

  /*! \brief Get error stack
   *
   * \note The returned stack is rebuilt at each call.
   * \sa stackError()
   */
  std::vector<mdtErrorV2> getErrorStack() const;

  /*! \brief Add the source of error
   *
   *  It's possible to use the helper macro MDT_ERROR_SET_SRC()
   *
   * \pre this error must not be null when calling this function
   */
  void setSource(const QString & fileName, int fileLine, const QString & className, const QString & functionName);
  
  void setSource(const QString & fileName, int fileLine, const QObject * const obj, const QString & functionName);

  /*! \brief Error source function
   */
  QString functionName() const;

  /*! \brief Error source file (name only)
   */
  QString fileName() const;

  /*! \brief Error source line
   */
  int fileLine() const;

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

#endif // #ifndef MDT_ERROR_V2_H
