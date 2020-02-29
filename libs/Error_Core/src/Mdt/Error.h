/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_ERROR_H
#define MDT_ERROR_H

#include "MdtError_CoreExport.h"
#include <QExplicitlySharedDataPointer>
#include <QString>
#include <QLatin1String>
#include <QSharedData>
#include <QMetaType>
#include <type_traits>  // std::is_same
#include <typeindex>
#include <typeinfo>
#include <vector>

//#include <QDebug>

namespace Mdt{

  class Error;

  /*! \internal Generic error
   */
  struct GenericError
  {
  };

  /*! \internal Private base for Error
   */
  struct MDT_ERROR_CORE_EXPORT ErrorPrivateBase : public QSharedData
  {
    /*! \brief Constructor
    */
    ErrorPrivateBase(const std::type_info & ti)
    : level(0),
      userErrorType(ti),
      lineNumber(0)
    {
    }

    ErrorPrivateBase(const ErrorPrivateBase & other) = default;
    ErrorPrivateBase() = delete;
    ErrorPrivateBase & operator=(const ErrorPrivateBase &) = delete;
    ErrorPrivateBase(ErrorPrivateBase &&) = delete;

    // Needed for QExplicitlySharedDataPointer::clone()
    virtual ErrorPrivateBase *clone() const = 0;

    // Destructor must be virtual (else ErrorPrivate::~ErrorPrivate() is never called)
    virtual ~ErrorPrivateBase()
    {
    }

    short level;
    std::type_index userErrorType;
    QString text;
    QString informativeText;
    std::vector<Error> pvErrorStack;
    QString fileName;
    int lineNumber;
    QString functionName;
  };

  /*! \internal User defined error
   */
  template <typename T>
  struct ErrorPrivate : public ErrorPrivateBase
  {
    ErrorPrivate(const T & e)
    : ErrorPrivateBase(typeid(T)),
      error(e)
    {
    }

    ~ErrorPrivate()
    {
    }

    ErrorPrivate() = delete;


    ErrorPrivate & operator=(const ErrorPrivate &) = delete;
    ErrorPrivate(ErrorPrivate &&) = delete;

    // Because of template, copy constructor must be explicit ( used by clone() )
    ErrorPrivate(const ErrorPrivate & other)
    : ErrorPrivateBase(other),
      error(other.error)
    {
    }

    // Needed for QExplicitlySharedDataPointer::clone()
    ErrorPrivate *clone() const
    {
      return new ErrorPrivate(*this);
    }

    T error;
  };

} // namespace Mdt{

class QObject;
class QFileDevice;

/*! \brief Helper macro to build a Error with source file informations
 *
 * Typical usage:
 * \code
 * auto error = mdtErrorNew("Some error occured", Mdt::Error::Critical, "MyClass");
 * \endcode
 *
 * \note If you create a error for a QObject subclass, you should use mdtErrorNewQ()
 */
#define mdtErrorNew(text, level, className) Mdt::Error(static_cast<Mdt::GenericError>(Mdt::GenericError()), text, level, QString::fromLocal8Bit(__FILE__), __LINE__, QString::fromLatin1(className), QString::fromLatin1(__FUNCTION__))

/*! \brief Helper macro to build a user defined Error with source file informations
 *
 * Typical usage:
 * \code
 * auto error = mdtErrorNewT(-1, "Some error occured", Mdt::Error::Critical, "MyClass");
 * \endcode
 *
 * \note If you create a error for a QObject subclass, you should use mdtErrorNewTQ()
 */
#define mdtErrorNewT(error, text, level, className) Mdt::Error(error, text, level, QString::fromLocal8Bit(__FILE__), __LINE__, QString::fromLatin1(className), QString::fromLatin1(__FUNCTION__))

/*! \brief Helper macro to build a Error with source file informations
 *
 * This version accepts a QObject instance, which avoids typing class name explicitly.
 *  If you create a error for a object that is not a subclass of QObject,
 *  use mdtErrorNew()
 *
 * Typical usage:
 * \code
 * auto error = mdtErrorNewQ("Some error occured", Mdt::Error::Critical, this);
 * \endcode
 */
#define mdtErrorNewQ(text, level, obj) Mdt::Error(static_cast<Mdt::GenericError>(Mdt::GenericError()), text, level, QString::fromLocal8Bit(__FILE__), __LINE__, obj, QString::fromLatin1(__FUNCTION__))

/*! \brief Helper macro to build a user defined Error with source file informations
 *
 * This version accepts a QObject instance, which avoids typing class name explicitly.
 *  If you create a error for a object that is not a subclass of QObject,
 *  use mdtErrorNewT()
 *
 * Typical usage:
 * \code
 * auto error = mdtErrorNewTQ(-1, "Some error occured", Mdt::Error::Critical, this);
 * \endcode
 */
#define mdtErrorNewTQ(error, text, level, obj) Mdt::Error(error, text, level, QString::fromLocal8Bit(__FILE__), __LINE__, obj, QString::fromLatin1(__FUNCTION__))

/*! \brief Helper macro to set source file informations to a existing error
 *
 * \note If you create a error for a QObject subclass, you should use MDT_ERROR_SET_SRC_Q()
 */
#define MDT_ERROR_SET_SRC(e, className) e.setSource(QString::fromLocal8Bit(__FILE__), __LINE__, className, QString::fromLatin1(__FUNCTION__))

/*! \brief Helper macro to set source file informations to a existing error
 *
 * This version accepts a QObject instance, which avoids typing class name explicitly.
 *  If you create a error for a object that is not a subclass of QObject, use MDT_ERROR_SET_SRC() .
 */
#define MDT_ERROR_SET_SRC_Q(e, obj) e.setSource(QString::fromLocal8Bit(__FILE__), __LINE__, obj, QString::fromLatin1(__FUNCTION__))

/*! \brief Get a Mdt::Error from last error of given file device
 *
 * Typical usage:
 * \code
 * QFile file;
 * if(!file.open(path)){
 *   auto error = mdtErrorFromQFileDevice(file, "MyClass");
 * }
 * \endcode
 *
 * \sa mdtErrorFromQFileDeviceQ()
 */
#define mdtErrorFromQFileDevice(fileDevice, className) Mdt::Error::fromQFileDevice(fileDevice, QString::fromLocal8Bit(__FILE__), __LINE__, QString::fromLatin1(className), QString::fromLatin1(__FUNCTION__))

/*! \brief Get a Mdt::Error from last error of given file device
 *
 * This version avoid giving class name,
 *  but only works for QObject subclass.
 *
 * \sa mdtErrorFromQFileDevice()
 */
#define mdtErrorFromQFileDeviceQ(fileDevice, obj) Mdt::Error::fromQFileDevice(fileDevice, QString::fromLocal8Bit(__FILE__), __LINE__, obj, QString::fromLatin1(__FUNCTION__))

/*! \brief Get a Mdt::Error from last error of given file
 *
 * \sa mdtErrorFromQFileQ()
 */
#define mdtErrorFromQFile(file, className) Mdt::Error::fromQFileDevice(file, QString::fromLocal8Bit(__FILE__), __LINE__, QString::fromLatin1(className), QString::fromLatin1(__FUNCTION__))

/*! \brief Get a Mdt::Error from last error of given file
 *
 * This version avoid giving class name,
 *  but only works for QObject subclass.
 *
 * \sa mdtErrorFromQFile()
 */
#define mdtErrorFromQFileQ(file, obj) Mdt::Error::fromQFileDevice(file, QString::fromLocal8Bit(__FILE__), __LINE__, obj, QString::fromLatin1(__FUNCTION__))

namespace Mdt{

  /*! \brief Value class that contains a error
   *
   * Error contains only a pointer to (implicitly) shared data (also known as copy-on-write).
   *  As long as no error was set, no more memory is allocated.
   *  This allows to store a Error object with a few overhead.
   *
   * Concept of error stack
   *
   *  Imagine a case of a application that provides document editing functionnality,
   *  and the user wants to save a document.
   *  The application will probably call a helper function from its own library,
   *  which also calls a other system function from a onther part of the library,
   *  which finally calls a (maybe system dependant) low level function.
   *  The low level function fails (for some reason).
   *  How could the application provide the most usefull error message to the user ?
   *  Lets illustrate a possible call stack:
   *  <table class="srcdoc_td_left">
   *   <tr><th>Function</th><th>Error</th><th>Error message</th></tr>
   *   <tr><td>write()</td><td>EDQUOT (int)</td><td>Disk quota exhausted</td></tr>
   *   <tr><td>writeToFile()</td><td>DiskQuotaExhausted (enum)</td><td>Could not write to file 'document.txt' because disk quota exhausted</td></tr>
   *   <tr><td>saveDocument()</td><td></td><td>Could not save your work to 'document.txt'. This is because you reached the disk quota. Please try to save the document to a other place and contact your administrator to solve the problem.</td></tr>
   *  </table>
   * In above scenario, the application can build appropriate message because it knows what DiskQuotaExhausted means.
   * For some other errors (that the application currently not handles), it could also simply display the error returned from saveDocument().
   * To implement such error stack, simply, at each level, stack a error that a function returns to current error.
   * For example, in writeToFile(), if write() fails, we create a new Error, and return it.
   * Then, saveDocument() will fail, create its own Error object, and stack the one returned by writeToFile().
   * To stack a error, use stackError() , and use getErrorStack() to get stacked errors back.
   *
   * If you need to send Error object across threads with Qt signal/slot (queued),
   *  Error must be registered with qRegisterMetaType().
   *  This is allready done in Mdt::Application::init().
   *  If you don't use Mdt::Application, don't forget to call qRegisterMetaType<Mdt::Error>() in, f.ex., your main() function.
   */
  class MDT_ERROR_CORE_EXPORT Error
  {
  public:

    /*! \brief Error level
    */
    enum Level : short
    {
      NoError,   /*!< No error . */
      Info,      /*!< Just a information, application continues to work in normal way . */
      Warning,   /*!< Error that could be handled */
      Critical   /*!< Error that was not handled */
    };

    /*! \brief Construct a null error
    */
    explicit Error();

    /*! \brief Construct a error with error and source set
    *
    * Calling this constructor directly is a bit long.
    *  Consider using mdtErrorNew() or mdtErrorNewT() macro.
    */
    template <typename T>
    explicit Error(const T & error, const QString & text, Level level,
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
    explicit Error(const T & error, const QString & text, Level level,
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
    Error(const Error &) = default;

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
      setError<GenericError>(GenericError(), text, level);
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

    /*! \brief Check if the user defined error is of type T
     */
    template<typename T>
    bool isErrorType() const
    {
      if(!pvShared){
        return false;
      }
      return std::type_index(typeid(T)) == pvShared->userErrorType;
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
      *       which we really not want here.
      */
      QString msg;
      if( !isErrorType<T>() ){
        // If we have caller informations, put it into failure message
        const QString functionName = pvShared->functionName;
        QString caller;
        if(!functionName.isEmpty()){
          caller = QString(QLatin1String(" Error object source: %1 in file %2, line %3"))
                   .arg(functionName, pvShared->fileName)
                   .arg(pvShared->lineNumber);
        }
        msg = QLatin1String("Requested type T is not the same as stored error type.") + caller;
      }
      Q_ASSERT_X(isErrorType<T>(), "T Error::error() const", msg.toStdString().c_str());
      Q_ASSERT(dynamic_cast<const ErrorPrivate<T>*>(pvShared.constData()) != nullptr);
  #endif // #ifndef QT_NO_DEBUG

      return static_cast<const ErrorPrivate<T>*>(pvShared.constData())->error;
    }

    /*! \brief Check if this error \a err
     *
     * Returns true if \a err is of the same type and value as the one this error contains,
     *  otherwise false.
     */
    template<typename T>
    bool isError(const T & err) const noexcept
    {
      if(!isErrorType<T>()){
        return false;
      }
      return (err == error<T>());
    }

    /*! \brief Update error text
    *
    * \sa stackError()
    */
    void updateText(const QString & text);

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
    *
    * \sa isNull()
    */
    Level level() const;

    /*! \brief Get error text
    *
    * If no error was set,
    *  a empty string is returned.
    *
    * \sa isNull()
    */
    QString text() const;

    /*! \brief Stack given error
    *
    * \pre this error and given error must not be null
    * \sa getErrorStack()
    */
    void stackError(const Error & error);

    /*! \brief Get error stack
    *
    * If no error was set,
    *  a empty stack is returned.
    *
    * \note The returned stack is rebuilt at each call.
    * \sa stackError()
    * \sa isNull()
    */
    std::vector<Error> getErrorStack() const;

    /*! \brief Add the source of error
    *
    * It's possible to use the helper macro MDT_ERROR_SET_SRC()
    *
    * \pre this error must not be null when calling this function
    */
    void setSource(const QString & fileName, int fileLine, const QString & className, const QString & functionName);

    /*! \brief Add the source of error
    *
    * Avoids typing className by hand.
    *  Note that this only works on functions that are
    *  QObject (subclasses).
    *
    * It's possible to use the helper macro MDT_ERROR_SET_SRC_Q()
    *
    * \pre this error must not be null when calling this function
    */
    void setSource(const QString & fileName, int fileLine, const QObject * const obj, const QString & functionName);

    /*! \brief Commit error
    *
    * Will use mdt::error::Logger to output the error.
    */
    void commit();

    /*! \brief Ger error source function
    *
    * If no error was set,
    *  a empty string is returned.
    *
    * \sa isNull()
    */
    QString functionName() const;

    /*! \brief Get error source file (name only)
    *
    * If no error was set,
    *  a empty string is returned.
    *
    * \sa isNull()
    */
    QString fileName() const;

    /*! \brief Get error source line
    */
    int fileLine() const;

    /*! \brief Get a Mdt::Error from last error in \a fileDevice
     *
     * \sa mdtErrorFromQFileDevice()
     */
    static Error fromQFileDevice(const QFileDevice & fileDevice, const QString & sourceCodeFile, int line, const QString & className, const QString & functionName);

    /*! \brief Get a Mdt::Error from last error in \a fileDevice
     *
     * \sa mdtErrorFromQFileDeviceQ()
     */
    static Error fromQFileDevice(const QFileDevice & fileDevice, const QString & sourceCodeFile, int line, const QObject * const obj, const QString & functionName);

    /*! \brief Get a new error that contains the user defined error, the level and the error stack from \a other
     *
     * \code
     * void MyClass::setLastErrorFromSqlQuery(const Mdt::Error & queryError)
     * {
     *   const QString msg = tr("Could not update a entry in Person table");
     *
     *   mLastError = Mdt::Error::newErrorFromOther(queryError);
     *   mLastError.updateText(msg);
     *   // Optional: set source
     *   MDT_ERROR_SET_SRC(mLastError, "MyClass");
     * }
     * \endcode
     */
    static Error newErrorFromOther(const Error & other);

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
      pvShared = new ErrorPrivate<T>(userDefinedError);
    }

    QExplicitlySharedDataPointer<ErrorPrivateBase> pvShared;
  };

} // namespace Mdt{

// Q_DECLARE_OPAQUE_POINTER(Error)
Q_DECLARE_METATYPE(Mdt::Error)

#endif // #ifndef MDT_ERROR_H
