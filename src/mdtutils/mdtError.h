/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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

#include <QString>
#include <QMetaType>
#include <QMessageBox>

class QObject;

/*! \brief error list
 *
 * \todo Check if this is not obselete ?
 *    Errors are very specific, and per module/class error enum 
 *     seems to be better than a central, very big and incoherent enum ...
 */
enum mdt_error_t{
  MDT_NO_ERROR = 0,             /*!< No error */
  MDT_UNDEFINED_ERROR = -1000,  /*!< Unknow error */
  MDT_FILE_IO_ERROR,            /*!< File I/O error */
  MDT_PORT_IO_ERROR,            /*!< Unknow port I/O error */
  MDT_PORT_QUEUE_EMPTY_ERROR,   /*!< A I/O Queue is empty \sa mdtPortThread */
  MDT_SERIAL_PORT_IO_ERROR,
  MDT_USB_IO_ERROR,
  MDT_TCP_IO_ERROR,
  MDT_FRAME_DECODE_ERROR,
  MDT_FRAME_ENCODE_ERROR,
  MDT_DEVICE_ERROR,
  MDT_PARSE_ERROR,              /*!< Parsing error */
  MDT_DATABASE_ERROR,           /*!< Database error */
  MDT_QM_FILE_LOAD_ERROR,       /*!< A translation file (.qm) could not be loaded */
  MDT_MEMORY_ALLOC_ERROR,       /*!< A memory allocation failed */
  MDT_GUI_ERROR                 /*!< GUI error (widget not found, ...) */
};

/*! \brief Add source information to error
 *
 * Source informations are file, file line and function name
 */
#define MDT_ERROR_SET_SRC(e, className) e.setSource(__FILE__, __LINE__, className, __FUNCTION__);

/*! \brief mdtError contains informations about a error that occured
 *
 * Currently, a mdtError contains several informations:
 *  - text (mandatory) : a description of the error that occured .
 *          Access: mdtError(const QString&, level_t) , text() .
 *  - level (mandatory) : the severity of the error .
 *          Access: mdtError(const QString&, level_t) , level() , levelIcon() .
 *  - systemNumber (optional) : error number returned by a system (or other library) call .
 *          Access: setSystemError() , systemNumber() , systemErrorString() .
 *  - systemText (optional) : error text number returned by a system (or other library) call .
 *          Access: setSystemError() , systemText() , systemErrorString() .
 *  - informativeText (optional) : can be used the same way than QMessageBox .
 *         Access : setInformativeText() , informativeText() .
 *
 * number : currently used with mdt_error_t enum, but it seems that this is not a good idea,
 *           and this will change once a better one is found ...
 *
 * This class contains the information of a error. To just put it to standard defined outputs,
 *  use commit() .
 *
 * Note: no copy constrctor/operator are defined, because only primitve and copiable objects are used,
 *  but a mdtError can be copied .
 */
class mdtError
{
 public:

  /*! \brief Error level
   */
  enum level_t{
    NoError = 0x00,   /*!< No error . */
    Info = 0x01,      /*!< Just a information, application continues to work in normal way . */
    Warning = 0x02,   /*!< Error that could be handled */
    Error = 0x04      /*!< Error that was not handled */
  };

  /*! \brief Constructor
   */
  mdtError();

  /*! \brief Construct new error
   *
   * \deprecated If no good idea falls in for the usage of number , this constrctor should become obselete .
   */
  mdtError(int number, const QString &text, level_t level);

  /*! \brief Construct new error
   */
  mdtError(const QString &text, level_t level);

  /*! \brief Set error
   *
   * Will also clear current error .
   */
  void setError(const QString &text, level_t level);

  /*! \brief Update error text
   *
   * Will set error text, but does not clear error .
   */
  void updateText(const QString & text);

  /*! \brief Clear
   */
  void clear();

  /*! \brief Add system returned error (for example, errno)
   */
  void setSystemError(int number, const QString &text);

  /*! \brief Add system error (for Windows  API)
   *
   *  When using Windows API, the standard mechanism is GetLastError() and FormatMessage().
   *  Calling this method will do this internally.
   */
#ifdef Q_OS_WIN
  void setSystemErrorWinApi();
#endif

  /*! \brief Add the source of error
   *
   *  It's possible to use the helper macro MDT_ERROR_SET_SRC()
   */
  void setSource(const QString &filePath, int fileLine, const QString &className, const QString &functionName);

  /*! \brief Send error to output
   */
  void commit();

  /*! \brief Error number
   *
   * \deprecated If no good idea falls in for the usage of number , this method should become obselete .
   */
  int number() const;

  /*! \brief Error text
   */
  QString text() const;

  /*! \brief Error level
   * \See level_t
   */
  level_t level() const;

  /*! \brief Map the level to a QMessageBox Icon
   */
  QMessageBox::Icon levelIcon() const;

  /*! \brief Error number returned from system if available
   */
  int systemNumber() const;

  /*! \brief Error text returned from system, if available
   */
  QString systemText() const;

  /*! \brief Error number and text from system, if available
   *
   * If obj is set, translation is used.
   *  (We not want to use the static QObject::tr() ).
   */
  QString systemErrorString(QObject *obj = 0) const;

  /*! \brief Set informative text
   */
  void setInformativeText(const QString &text);

  /*! \brief Get informative text
   */
  QString informativeText() const;

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

  int pvNumber;
  QString pvText;
  QString pvInformativeText;
  level_t pvLevel;
  int pvSystemNumber;
  QString pvSystemText;
  QString pvFunctionName;
  QString pvFileName;
  int pvFileLine;
};

Q_DECLARE_METATYPE(mdtError)

#endif  // #ifndef MDT_ERROR_H
