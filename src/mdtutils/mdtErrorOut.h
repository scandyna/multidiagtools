#ifndef MDT_ERROR_OUT_H
#define MDT_ERROR_OUT_H


#include "mdtError.h"
#include <QMessageBox>
#include <QMutex>
#include <QFile>
#include <QObject>

/*! \brief Error output system
 * Helper for error output. Outputs are the stderr output (console), a GUI messagebox and a logfile.
 */
class mdtErrorOut : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Initialize the error output system
   * After this call, the log and dialog levels are defined to Warning and Error
   * This function must be called from main thread (GUI thread)
   */
  static bool init(const QString &logFile);
  
  /*! \brief Set the log levels mask
   *  The mask is a OR combinaison of mdtError::level_t flags
   *  \pre The system must be initialized. \see init()
   */
  static void setLogLevelsMask(int mask);

  /*! \brief Set the dialog display levels mask
   *  The mask is a OR combinaison of mdtError::level_t flags
   * \pre The system must be initialized. \see init()
   */
  static void setDialogLevelsMask(int mask);

  /*! \brief Add a error to the error output system
   * \pre The system must be initialized. \see init()
   */
  static void addError(mdtError &error);

  /*! \brief Get the instance of the error output system
   * This function can return a Null pointer if init() was never called.
   * This function, and returned pointer, should be used with care in multi-thread applicatopn !
   */
  static mdtErrorOut *instance();
  
  /*! \brief Get the log file path
   * \pre The system must be initialized. \see init()
   */
  static QString logFile();

  /*! \brief Destroy the error output system instance
   * This function must be called from main thread (GUI thread)
   */
  static void destroy();

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
  QFile pvLogFile;
  QMutex pvMutex;
  QMessageBox pvDialog;
  int pvLogLevelsMask;
  int pvDialogLevelsMask;
};

#endif  // #ifndef MDT_ERROR_OUT_H
