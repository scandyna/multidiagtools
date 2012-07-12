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
#ifndef MDT_FILE_COPIER_H
#define MDT_FILE_COPIER_H

#include <QThread>
#include <QString>
#include <QList>
#include <QLinkedList>
#include <QFileInfo>
#include <QMutex>
#include <QMessageBox>
#include <QProgressDialog>

class mdtFileCopierItem;

/*! \brief Helper class for file copy
 *
 * When files must be copied in a GUI application,
 * some common situation must be handled:
 *  - Display a message box when source file not exists, or cannot be read
 *  - Ask the user if allready existing destination file must be overwritten
 *  - Display a progress bar during a long copy
 *  - ...<br>
 * This class was made to simplify this process.<br>
 * The copy is done in a separated thread.
 * Notes:
 *  - For a simple and efficient copy, it's better to use
 *   system provided functions, QFile or others.
 *  - If a file exists at destination, user will be asked about what to do.
 *   But, the ask operation, and the effective copy are not a atomic operation.
 *   So it can happen that another process creates the destination, and that file is then overwritten
 *   during copy !
 */
class mdtFileCopier : public QThread
{
 Q_OBJECT 

 public:

  mdtFileCopier(QObject *parent = 0);
  ~mdtFileCopier();

  /*! \brief Permit direct destination overwrite
   * 
   * If set false, the user will be asked about what to do if destination allready exists.
   */
  void setDirectDestOverwrite(bool directOverwrite);

  /*! \brief Set the default setup
   * 
   * Default setup is:
   *  - Test mode is diseabled
   *  - Direct overwrite diseabled, user will be asked if destination allready exists
   * This method is called by constructor.
   * \sa setTestMode()
   * \sa setDirectDestOverwrite()
   */
  void setDefaultSetup();

  /*! \brief Add a source and destination file path to copy
   * 
   * At this point, the copy is enqueued. To process copy, call startCopy().
   * \sa startCopy()
   */
  void addCopy(const QString & srcFilePath, const QString & destFilePath, bool syncAfterCopy = false, bool checkAfterCopy = false);

  /*! \brief Start the copy
   *
   * For all enqueued copies, some checks will be done, when ok, copies will then processed.<br>
   * To know when copy is finished, use waitFinished(), or connect to the finished() signal.
   * \return true if copy will be processed, or false if no copy can be done, or user cancelled copy because of some problems.
   */
  bool startCopy();

  /*! \brief Wait until copies are done
   * 
   * This function will only return when copy process is done.
   * Internally, a couple of msleep()/process event will be called,
   *  so Qt's GUI event loop is not broken, and application will not freeze.
   * \return True on success, false on failure or abort (errors are displayed to the user, and logged with mdtError system)
   */
  bool waitFinished();

  /*! \brief If set true, message boxes will not be displayed, and some default decisions will be taken.
   * 
   * Used by unit tests. Please never use this flag without you know that it can happen !
   */
  void setTestMode(bool enable);

 public slots:

  /*! \brief Cancel running copy (used internally for progress dialog)
   */
  void cancelCopy();

 signals:

  /*! \brief Emitted when copy process is done.
   * 
   * \param success True on success, false on failure (errors are displayed to the user, and logged with mdtError system)
   */
  void finished(bool success);

  /*! \brief Emitted when new progress range is set (used internally for proegress dialog)
   */
  void newProgressRange(int min, int max);

  /*! \brief Emitted when new progress value is set (used internally for proegress dialog)
   */
  void newProgressValue(int value);

  /*! \brief Emitted when new progress label text is set (used internally for proegress dialog)
   */
  void newProgressLabelText(const QString &text);

  /*! \brief Emitted when copy is ended or aborted (used internally to display copy result)
   */
  void copyEnded(const QString &failedCopies, const QString &informativeText = QString());

 private slots:

  // Called by thread when copy is finished or cancelled to display result to the user
  void displayCopyResult(const QString &failedCopies, const QString &informativeText);

  // Set the end flags, display copy results and emit finished() signal
  void finish(const QString &failedCopies, const QString &informativeText);

 private:

  // Call the system sync to write data to device - Is called by thread, must not be called elsewhere.
  bool sync(FILE *f);

  // Thread method that processes copies
  void run();

  char *pvBuffer;
  size_t pvBufferSize;
  QFileInfo pvSrcFileInfo;
  QFileInfo pvDestFileInfo;
  QMutex pvMutex;
  QMessageBox *pvMessageBox;
  QProgressDialog *pvProgressDialog;
  QLinkedList<mdtFileCopierItem*> pvCopies;
  QList <mdtFileCopierItem*> pvCopiesInProcess;
  bool pvTestMode;
  bool pvDirectDestOverwrite;
  bool pvFinished;
  bool pvCopySuccessfull;
  bool pvCancelCopy;
};

#endif  // #ifndef MDT_FILE_COPIER_H
