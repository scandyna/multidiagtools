#ifndef MDT_PORT_H
#define MDT_PORT_H

#include <QObject>
#include <QString>

#ifdef Q_OS_UNIX
 #include "linux/mdtPortPosix.h"
 #define mdtPortSys mdtPortPosix
#endif

class mdtPort : public mdtPortSys
{
 Q_OBJECT

 public:

  /*! \brief Construct a new port object
   *  R/W timeouts are set to 500 ms
   */
  mdtPort(QObject *parent = 0);
  ~mdtPort();

  /*! \brief Open port
   * 
   * Note: this method locks the internal mutex
   * \param path Path to port
   * \return True if successfull
   */
  bool open(mdtPortConfig &cfg);


  /*! \brief Close the port
   * 
   * Note: this method locks the internal mutex
   */
  void close();
};

#endif  // #ifndef MDT_PORT_H
