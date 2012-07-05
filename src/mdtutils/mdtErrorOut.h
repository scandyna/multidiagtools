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
#ifndef MDT_ERROR_OUT_H
#define MDT_ERROR_OUT_H

#include "mdtError.h"
#include <QMessageBox>
#include <QThread>
#include <QMutex>
#include <QFile>
#include <QObject>
#include <QStringList>

class mdtErrorOutLogger;

/*! \brief Error output system
 * Helper for error output. Outputs are the stderr output (console), a GUI messagebox and a logfile.
 */
class mdtErrorOut : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Initialize the error output system
   *
   * After this call, the log and dialog levels are defined to Warning and Error
   * This function must be called from main thread (GUI thread)
   */
  static bool init(const QString &logFile);

  /*! \brief Set the log levels mask
   *
   *  The mask is a OR combinaison of mdtError::level_t flags
   *  \pre The system must be initialized. \see init()
   */
  static void setLogLevelsMask(int mask);

  /*! \brief Set the dialog display levels mask
   *
   *  The mask is a OR combinaison of mdtError::level_t flags
   * \pre The system must be initialized. \see init()
   */
  static void setDialogLevelsMask(int mask);

  /*! \brief Add a error to the error output system
   *
   * \pre The system must be initialized. \see init()
   */
  static void addError(mdtError &error);

  /*! \brief Get the instance of the error output system
   *
   * This function can return a Null pointer if init() was never called.
   * This function, and returned pointer, should be used with care in multi-thread applicatopn !
   */
  static mdtErrorOut *instance();

  /*! \brief Get the log file path
   *
   * \pre The system must be initialized. \see init()
   */
  static QString logFile();

  /*! \brief Destroy the error output system instance
   *
   * This function must be called from main thread (GUI thread)
   */
  static void destroy();

  /*! \brief Set the maximum log file size [Byte] before backup
   */
  static void setLogFileMaxSize(qint64 maxSize);

 private slots:

  void showDialog(mdtError error);

 signals:

  void sigShowDialog(mdtError error);

 private:

  // Constructors and destructor set private
  mdtErrorOut();
  ~mdtErrorOut();
  mdtErrorOut(const mdtErrorOut &orig);

  static mdtErrorOut *pvInstance;
  QMutex pvMutex;
  QMessageBox pvDialog;
  int pvLogLevelsMask;
  int pvDialogLevelsMask;
  mdtErrorOutLogger *pvLogger;
};

/*! \brief Put errors in the log file
 */
class mdtErrorOutLogger : public QThread
{
 public:

  mdtErrorOutLogger(QObject *parent = 0);
  ~mdtErrorOutLogger();

  /*! \brief Set the log file path
   *
   * \return True if file can be open for write, false else
   */
  bool setLogFilePath(const QString &path);

  /*! \brief Get log file path
   */
  QString logFilePath();

  /*! \brief Add a line to the log file
   */
  void putData(QString data);

  /*! \brief Set the maximum log file size [Byte] before backup
   */
  void setLogFileMaxSize(qint64 maxSize);

 private:

  // Diseable copy
  mdtErrorOutLogger(const mdtErrorOutLogger&);

  void run();

  QStringList pvDataToWrite;
  QFile *pvLogFile;
  QMutex *pvMutex;
  qint64 pvLogFileMaxSize;    // Maximum size before backup
};

#endif  // #ifndef MDT_ERROR_OUT_H
