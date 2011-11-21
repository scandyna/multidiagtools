#ifndef MDT_ERROR_H
#define MDT_ERROR_H


//class mdtErrorOut;
#include <QString>

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

  /*! \brief Construct new error
   */
  mdtError(int number, const QString &text, level_t level);

  /*! \brief Add system returned error (for example, errno)
   */
  void setSystemError(int number, const QString &text);

  /*! \brief Ass the source of error
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

#endif  // #ifndef MDT_ERROR_H
