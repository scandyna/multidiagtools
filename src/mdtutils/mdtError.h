#ifndef MDT_ERROR_H
#define MDT_ERROR_H

#include <QString>
#include <QMetaType>

/*! \brief error list
 */
enum mdt_error_t{
  MDT_NO_ERROR = 0,
  MDT_UNDEFINED_ERROR,
  MDT_FILE_IO_ERROR,
  MDT_PORT_IO_ERROR,
  MDT_SERIAL_PORT_IO_ERROR,
  MDT_USB_IO_ERROR,
  MDT_TCP_IO_ERROR,
  MDT_FRAME_DECODE_ERROR,
  MDT_DEVICE_ERROR
};

/*! \brief Add source information to error
 * Source informations are file, file line and function name
 */
#define MDT_ERROR_SET_SRC(e, className) e.setSource(__FILE__, __LINE__, className, __FUNCTION__);

class mdtError
{
 public:

  enum level_t{
    NoError = 0x00,
    Info = 0x01,
    Warning = 0x02,
    Error = 0x04
  };

  mdtError();
  
  /*! \brief Construct new error
   */
  mdtError(int number, const QString &text, level_t level);

  /*! \brief Add system returned error (for example, errno)
   */
  void setSystemError(int number, const QString &text);

  /*! \brief Add the source of error
   *  It's possible to use the helper macro MDT_ERROR_SET_SRC()
   */
  void setSource(const QString &fileName, int fileLine, const QString &className, const QString &functionName);

  /*! \brief Send error to output
   */
  void commit();

  /*! \brief Error number
   */
  int number();

  /*! \brief Error text
   */
  QString text();

  /*! \brief Error level
   * \See level_t
   */
  level_t level();

  /*! \brief Error number returned from system if available
   */
  int systemNumber();

  /*! \brief Error text returned from system, if available
   */
  QString systemText();

  /*! \brief Error source function
   */
  QString functionName();

  /*! \brief Error source file
   */
  QString fileName();

  /*! \brief Error source line
   */
  int fileLine();

 private:

  int pvNumber;
  QString pvText;
  level_t pvLevel;
  int pvSystemNumber;
  QString pvSystemText;
  QString pvFunctionName;
  QString pvFileName;
  int pvFileLine;
};

Q_DECLARE_METATYPE(mdtError)

#endif  // #ifndef MDT_ERROR_H
